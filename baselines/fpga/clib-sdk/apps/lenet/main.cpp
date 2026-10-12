// Trained CIFAR-10 LeNet with AvgPool. Application code using only clib.h.
#include "clib.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
using namespace clib;
static constexpr int slots=32768, baby_step=64;

static std::vector<double> read_values(const std::string& name, size_t count) {
    std::ifstream file(std::string(CLIB_APP_DATA_DIR)+"/"+name);
    std::vector<double> v; double x;
    while(file>>x) v.push_back(x);
    if(!file.eof() || v.size()!=count) throw std::runtime_error("incorrect data: "+name);
    return v;
}
static std::vector<double> padded(const std::vector<double>& x) {
    auto v=x; v.resize(slots,0.); return v;
}
#ifndef CLIB_LENET_SAMPLE
#define CLIB_LENET_SAMPLE 0
#endif
static std::string sample(const std::string& stage) {
    return "sample"+std::to_string(CLIB_LENET_SAMPLE)+"."+stage;
}
struct Entry { int slot; double weight; };
using Diagonals=std::map<int,std::vector<Entry>>;
struct Shape {int channels,height,width; int size() const{return channels*height*width;}};
static void edge(Diagonals& d,int out,int in,double weight) {
    d[(in-out+slots)%slots].push_back({out,weight});
}
static Diagonals conv_weights(const std::vector<double>& weights,Shape input,int outputs) {
    Diagonals d;int oh=input.height-4,ow=input.width-4;
    for(int co=0;co<outputs;++co) for(int ci=0;ci<input.channels;++ci)
        for(int ky=0;ky<5;++ky) for(int kx=0;kx<5;++kx)
            for(int h=0;h<oh;++h) for(int w=0;w<ow;++w)
                edge(d,(co*oh+h)*ow+w,(ci*input.height+h+ky)*input.width+w+kx,
                     weights[((co*input.channels+ci)*5+ky)*5+kx]);
    return d;
}
static Diagonals pool_weights(Shape input) {
    Diagonals d;int oh=input.height/2,ow=input.width/2;
    for(int c=0;c<input.channels;++c) for(int h=0;h<oh;++h) for(int w=0;w<ow;++w)
        for(int dy=0;dy<2;++dy) for(int dx=0;dx<2;++dx)
            edge(d,(c*oh+h)*ow+w,(c*input.height+2*h+dy)*input.width+2*w+dx,.25);
    return d;
}
static Diagonals fc_weights(const std::vector<double>& weights,int inputs,int outputs) {
    Diagonals d;
    for(int o=0;o<outputs;++o) for(int i=0;i<inputs;++i) edge(d,o,i,weights[o*inputs+i]);
    return d;
}
static std::vector<double> bias_values(const std::vector<double>& bias,int spatial) {
    std::vector<double> v(slots,0.);
    for(size_t c=0;c<bias.size();++c) for(int j=0;j<spatial;++j) v[c*spatial+j]=bias[c];
    return v;
}
// BSGS: d=g+b. Rotate the diagonal right by g before multiplication,
// then rotate the group sum left by g. Baby rotations are reused across groups.
static std::vector<double> group_plain(const std::vector<Entry>& entries,int giant) {
    std::vector<double> values(slots,0.);
    for(const auto& e:entries) values[(e.slot+giant)%slots]+=e.weight;
    return values;
}
static CIPHER linear(CONTEXT ctx,CIPHER input,const Diagonals& diagonals,
                     const std::vector<double>& bias,const std::string& name) {
    const auto q=Cipher_info(ctx,input).q_limbs;
    std::map<int,CIPHER> baby;
    for(const auto& item:diagonals) {
        int b=item.first%baby_step;
        if(baby.count(b)) continue;
        CIPHER r=New_ciph(ctx);
        if(b==0) Copy_ciph(ctx,r,input); else Rotate_ciph(ctx,r,input,b);
        baby[b]=r;
    }
    CIPHER sum=New_ciph(ctx),group=New_ciph(ctx),term=New_ciph(ctx);
    CIPHER next=New_ciph(ctx),rotated=New_ciph(ctx),scaled=New_ciph(ctx),output=New_ciph(ctx);
    bool have_sum=false;
    auto it=diagonals.begin();
    while(it!=diagonals.end()) {
        const int giant=it->first/baby_step*baby_step;
        bool have_group=false;
        while(it!=diagonals.end() && it->first/baby_step*baby_step==giant) {
            auto values=group_plain(it->second,giant);
            auto plain=Encode_plain(ctx,(name+"/w_"+std::to_string(it->first)).c_str(),values.data(),slots,q);
            Mul_plain(ctx,term,baby.at(it->first%baby_step),plain);
            Free_plain(plain);
            if(!have_group) {std::swap(group,term);have_group=true;}
            else {Add_ciph(ctx,next,group,term);std::swap(group,next);}
            ++it;
        }
        if(giant==0) Copy_ciph(ctx,rotated,group); else Rotate_ciph(ctx,rotated,group,giant);
        if(!have_sum) {std::swap(sum,rotated);have_sum=true;}
        else {Add_ciph(ctx,next,sum,rotated);std::swap(sum,next);}
    }
    Rescale_ciph(ctx,scaled,sum);
    if(bias.empty()) Copy_ciph(ctx,output,scaled);
    else {
        auto plain=Encode_plain(ctx,(name+"/bias").c_str(),bias.data(),slots,q-2);
        Add_plain(ctx,output,scaled,plain); Free_plain(plain);
    }
    for(auto& item:baby) Free_ciph(item.second);
    Free_ciph(sum);Free_ciph(group);Free_ciph(term);Free_ciph(next);Free_ciph(rotated);Free_ciph(scaled);
    return output;
}

// Q24 -> Q22 normalization; p7 -> Q16, p15 -> Q8; bootstrap -> Q24; p13 and product -> Q14.
static CIPHER polynomial_relu(CONTEXT ctx,CIPHER input,double range,const std::string& name,int count) {
    Begin_stage(ctx,(name+"/polynomial1").c_str());
    CIPHER t[101]{}; for(auto& c:t)c=New_ciph(ctx);
    CIPHER term=New_ciph(ctx),product=New_ciph(ctx);
    CIPHER3 wide=New_ciph3(ctx);
    std::vector<double> values(slots,1.0/range);
    PLAIN plain=Encode_plain(ctx,(name+"/normalize").c_str(),values.data(),slots,24);
    Mul_plain(ctx,term,input,plain); Rescale_ciph(ctx,t[0],term); Free_plain(plain);
    // Q20: mul.
    Mul_ciph(ctx,wide,t[0],t[0]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[1],product);
    // Q20: integer.
    Mul_Integer(ctx,t[2],t[1],2);
    Free_ciph(t[1]);
    // Q20: add_plain.
    std::fill(values.begin(),values.end(),-1);
    plain=Encode_plain(ctx,(name+"/minus_one_20").c_str(),values.data(),slots,20);
    Add_plain(ctx,t[3],t[2],plain);
    Free_plain(plain);
    Free_ciph(t[2]);
    // Q18: mul.
    Mul_ciph(ctx,wide,t[3],t[3]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[4],product);
    // Q18: integer.
    Mul_Integer(ctx,t[5],t[4],2);
    Free_ciph(t[4]);
    // Q18: add_plain.
    std::fill(values.begin(),values.end(),-1);
    plain=Encode_plain(ctx,(name+"/minus_one_18").c_str(),values.data(),slots,18);
    Add_plain(ctx,t[6],t[5],plain);
    Free_plain(plain);
    Free_ciph(t[5]);
    // Q20: pmul.
    std::fill(values.begin(),values.end(),-3.8091994324966305);
    plain=Encode_plain(ctx,(name+"/coef_2").c_str(),values.data(),slots,22);
    Mul_plain(ctx,term,t[0],plain); Rescale_ciph(ctx,t[7],term);
    Free_plain(plain);
    // Q18: mul.
    Mul_ciph(ctx,wide,t[3],t[7]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[8],product);
    Free_ciph(t[7]);
    // Q20: pmul.
    std::fill(values.begin(),values.end(),2.4609408687709102);
    plain=Encode_plain(ctx,(name+"/coef_3").c_str(),values.data(),slots,22);
    Mul_plain(ctx,term,t[0],plain); Rescale_ciph(ctx,t[9],term);
    Free_plain(plain);
    // Q18: modswitch.
    Modswitch(ctx,t[10],t[9],18);
    Free_ciph(t[9]);
    // Q18: add.
    Add_ciph(ctx,t[11],t[8],t[10]);
    Free_ciph(t[8]);
    Free_ciph(t[10]);
    // Q16: mul.
    Mul_ciph(ctx,wide,t[6],t[11]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[12],product);
    Free_ciph(t[6]);
    Free_ciph(t[11]);
    // Q20: pmul.
    std::fill(values.begin(),values.end(),-1.4303047945436642);
    plain=Encode_plain(ctx,(name+"/coef_4").c_str(),values.data(),slots,22);
    Mul_plain(ctx,term,t[0],plain); Rescale_ciph(ctx,t[13],term);
    Free_plain(plain);
    // Q18: mul.
    Mul_ciph(ctx,wide,t[3],t[13]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[14],product);
    Free_ciph(t[3]);
    Free_ciph(t[13]);
    // Q20: pmul.
    std::fill(values.begin(),values.end(),2.9446619353537646);
    plain=Encode_plain(ctx,(name+"/coef_5").c_str(),values.data(),slots,22);
    Mul_plain(ctx,term,t[0],plain); Rescale_ciph(ctx,t[15],term);
    Free_plain(plain);
    // Q18: modswitch.
    Modswitch(ctx,t[16],t[15],18);
    Free_ciph(t[15]);
    // Q18: add.
    Add_ciph(ctx,t[17],t[14],t[16]);
    Free_ciph(t[14]);
    Free_ciph(t[16]);
    // Q16: modswitch.
    Modswitch(ctx,t[18],t[17],16);
    Free_ciph(t[17]);
    // Q16: add.
    Add_ciph(ctx,t[19],t[12],t[18]);
    Free_ciph(t[12]);
    Free_ciph(t[18]);
    // Q14: mul.
    Mul_ciph(ctx,wide,t[19],t[19]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[20],product);
    // Q14: integer.
    Mul_Integer(ctx,t[21],t[20],2);
    Free_ciph(t[20]);
    // Q14: add_plain.
    std::fill(values.begin(),values.end(),-1);
    plain=Encode_plain(ctx,(name+"/minus_one_14").c_str(),values.data(),slots,14);
    Add_plain(ctx,t[22],t[21],plain);
    Free_plain(plain);
    Free_ciph(t[21]);
    // Q12: mul.
    Mul_ciph(ctx,wide,t[22],t[22]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[23],product);
    // Q12: integer.
    Mul_Integer(ctx,t[24],t[23],2);
    Free_ciph(t[23]);
    // Q12: add_plain.
    std::fill(values.begin(),values.end(),-1);
    plain=Encode_plain(ctx,(name+"/minus_one_12").c_str(),values.data(),slots,12);
    Add_plain(ctx,t[25],t[24],plain);
    Free_plain(plain);
    Free_ciph(t[24]);
    // Q10: mul.
    Mul_ciph(ctx,wide,t[25],t[25]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[26],product);
    // Q10: integer.
    Mul_Integer(ctx,t[27],t[26],2);
    Free_ciph(t[26]);
    // Q10: add_plain.
    std::fill(values.begin(),values.end(),-1);
    plain=Encode_plain(ctx,(name+"/minus_one_10").c_str(),values.data(),slots,10);
    Add_plain(ctx,t[28],t[27],plain);
    Free_plain(plain);
    Free_ciph(t[27]);
    // Q14: pmul.
    std::fill(values.begin(),values.end(),-0.00016002805372508758);
    plain=Encode_plain(ctx,(name+"/coef_9").c_str(),values.data(),slots,16);
    Mul_plain(ctx,term,t[19],plain); Rescale_ciph(ctx,t[29],term);
    Free_plain(plain);
    // Q12: mul.
    Mul_ciph(ctx,wide,t[22],t[29]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[30],product);
    Free_ciph(t[29]);
    // Q14: pmul.
    std::fill(values.begin(),values.end(),0.0030324784741275579);
    plain=Encode_plain(ctx,(name+"/coef_10").c_str(),values.data(),slots,16);
    Mul_plain(ctx,term,t[19],plain); Rescale_ciph(ctx,t[31],term);
    Free_plain(plain);
    // Q12: modswitch.
    Modswitch(ctx,t[32],t[31],12);
    Free_ciph(t[31]);
    // Q12: add.
    Add_ciph(ctx,t[33],t[30],t[32]);
    Free_ciph(t[30]);
    Free_ciph(t[32]);
    // Q10: mul.
    Mul_ciph(ctx,wide,t[25],t[33]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[34],product);
    Free_ciph(t[33]);
    // Q14: pmul.
    std::fill(values.begin(),values.end(),0.13658283611943375);
    plain=Encode_plain(ctx,(name+"/coef_11").c_str(),values.data(),slots,16);
    Mul_plain(ctx,term,t[19],plain); Rescale_ciph(ctx,t[35],term);
    Free_plain(plain);
    // Q14: modswitch.
    Modswitch(ctx,t[36],t[19],14);
    // Q12: mul.
    Mul_ciph(ctx,wide,t[36],t[22]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[37],product);
    Free_ciph(t[22]);
    Free_ciph(t[36]);
    // Q12: integer.
    Mul_Integer(ctx,t[38],t[37],2);
    Free_ciph(t[37]);
    // Q12: modswitch.
    Modswitch(ctx,t[39],t[19],12);
    // Q12: sub.
    Sub_ciph(ctx,t[40],t[38],t[39]);
    Free_ciph(t[38]);
    Free_ciph(t[39]);
    // Q10: pmul.
    std::fill(values.begin(),values.end(),-0.022197358582167027);
    plain=Encode_plain(ctx,(name+"/coef_12").c_str(),values.data(),slots,12);
    Mul_plain(ctx,term,t[40],plain); Rescale_ciph(ctx,t[41],term);
    Free_plain(plain);
    // Q10: modswitch.
    Modswitch(ctx,t[42],t[35],10);
    Free_ciph(t[35]);
    // Q10: add.
    Add_ciph(ctx,t[43],t[42],t[41]);
    Free_ciph(t[41]);
    Free_ciph(t[42]);
    // Q10: add.
    Add_ciph(ctx,t[44],t[34],t[43]);
    Free_ciph(t[34]);
    Free_ciph(t[43]);
    // Q8: mul.
    Mul_ciph(ctx,wide,t[28],t[44]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[45],product);
    Free_ciph(t[28]);
    Free_ciph(t[44]);
    // Q14: pmul.
    std::fill(values.begin(),values.end(),0.56852311346089535);
    plain=Encode_plain(ctx,(name+"/coef_13").c_str(),values.data(),slots,16);
    Mul_plain(ctx,term,t[19],plain); Rescale_ciph(ctx,t[46],term);
    Free_plain(plain);
    // Q10: pmul.
    std::fill(values.begin(),values.end(),-0.55587853162151368);
    plain=Encode_plain(ctx,(name+"/coef_14").c_str(),values.data(),slots,12);
    Mul_plain(ctx,term,t[40],plain); Rescale_ciph(ctx,t[47],term);
    Free_plain(plain);
    // Q10: modswitch.
    Modswitch(ctx,t[48],t[46],10);
    Free_ciph(t[46]);
    // Q10: add.
    Add_ciph(ctx,t[49],t[48],t[47]);
    Free_ciph(t[47]);
    Free_ciph(t[48]);
    // Q10: modswitch.
    Modswitch(ctx,t[50],t[25],10);
    Free_ciph(t[25]);
    // Q8: mul.
    Mul_ciph(ctx,wide,t[50],t[49]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[51],product);
    Free_ciph(t[49]);
    Free_ciph(t[50]);
    // Q14: pmul.
    std::fill(values.begin(),values.end(),1.614771079042868);
    plain=Encode_plain(ctx,(name+"/coef_15").c_str(),values.data(),slots,16);
    Mul_plain(ctx,term,t[19],plain); Rescale_ciph(ctx,t[52],term);
    Free_plain(plain);
    Free_ciph(t[19]);
    // Q10: pmul.
    std::fill(values.begin(),values.end(),-0.61640835832945129);
    plain=Encode_plain(ctx,(name+"/coef_16").c_str(),values.data(),slots,12);
    Mul_plain(ctx,term,t[40],plain); Rescale_ciph(ctx,t[53],term);
    Free_plain(plain);
    Free_ciph(t[40]);
    // Q10: modswitch.
    Modswitch(ctx,t[54],t[52],10);
    Free_ciph(t[52]);
    // Q10: add.
    Add_ciph(ctx,t[55],t[54],t[53]);
    Free_ciph(t[53]);
    Free_ciph(t[54]);
    // Q8: modswitch.
    Modswitch(ctx,t[56],t[55],8);
    Free_ciph(t[55]);
    // Q8: add.
    Add_ciph(ctx,t[57],t[51],t[56]);
    Free_ciph(t[51]);
    Free_ciph(t[56]);
    // Q8: add.
    Add_ciph(ctx,t[58],t[45],t[57]);
    Free_ciph(t[45]);
    Free_ciph(t[57]);
    // Q2: modswitch.
    Modswitch(ctx,t[59],t[58],2);
    Free_ciph(t[58]);
    // Q24: bootstrap.
    End_stage(ctx);
    Begin_stage(ctx,(name+"/middle_bootstrap").c_str());
    Bootstrap(ctx,t[60],t[59],24);
    End_stage(ctx);
    auto expected=padded(read_values(sample(name+"_middle_bootstrap"),count));
    Save_output(ctx,(name+"_middle_bootstrap").c_str(),t[60],expected.data(),slots,1e-3);
    Begin_stage(ctx,(name+"/polynomial2").c_str());
    Free_ciph(t[59]);
    // Q22: mul.
    Mul_ciph(ctx,wide,t[60],t[60]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[61],product);
    // Q22: integer.
    Mul_Integer(ctx,t[62],t[61],2);
    Free_ciph(t[61]);
    // Q22: add_plain.
    std::fill(values.begin(),values.end(),-1);
    plain=Encode_plain(ctx,(name+"/minus_one_22").c_str(),values.data(),slots,22);
    Add_plain(ctx,t[63],t[62],plain);
    Free_plain(plain);
    Free_ciph(t[62]);
    // Q20: mul.
    Mul_ciph(ctx,wide,t[63],t[63]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[64],product);
    // Q20: integer.
    Mul_Integer(ctx,t[65],t[64],2);
    Free_ciph(t[64]);
    // Q20: add_plain.
    std::fill(values.begin(),values.end(),-1);
    plain=Encode_plain(ctx,(name+"/minus_one_20").c_str(),values.data(),slots,20);
    Add_plain(ctx,t[66],t[65],plain);
    Free_plain(plain);
    Free_ciph(t[65]);
    // Q18: mul.
    Mul_ciph(ctx,wide,t[66],t[66]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[67],product);
    // Q18: integer.
    Mul_Integer(ctx,t[68],t[67],2);
    Free_ciph(t[67]);
    // Q18: add_plain.
    std::fill(values.begin(),values.end(),-1);
    plain=Encode_plain(ctx,(name+"/minus_one_18").c_str(),values.data(),slots,18);
    Add_plain(ctx,t[69],t[68],plain);
    Free_plain(plain);
    Free_ciph(t[68]);
    // Q22: pmul.
    std::fill(values.begin(),values.end(),0.00011111191620300779*range);
    plain=Encode_plain(ctx,(name+"/coef_18").c_str(),values.data(),slots,24);
    Mul_plain(ctx,term,t[60],plain); Rescale_ciph(ctx,t[70],term);
    Free_plain(plain);
    // Q20: modswitch.
    Modswitch(ctx,t[71],t[70],20);
    Free_ciph(t[70]);
    // Q18: mul.
    Mul_ciph(ctx,wide,t[66],t[71]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[72],product);
    Free_ciph(t[71]);
    // Q22: pmul.
    std::fill(values.begin(),values.end(),0.0065079951902107308*range);
    plain=Encode_plain(ctx,(name+"/coef_19").c_str(),values.data(),slots,24);
    Mul_plain(ctx,term,t[60],plain); Rescale_ciph(ctx,t[73],term);
    Free_plain(plain);
    // Q22: modswitch.
    Modswitch(ctx,t[74],t[60],22);
    // Q20: mul.
    Mul_ciph(ctx,wide,t[74],t[63]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[75],product);
    Free_ciph(t[63]);
    Free_ciph(t[74]);
    // Q20: integer.
    Mul_Integer(ctx,t[76],t[75],2);
    Free_ciph(t[75]);
    // Q20: modswitch.
    Modswitch(ctx,t[77],t[60],20);
    // Q20: sub.
    Sub_ciph(ctx,t[78],t[76],t[77]);
    Free_ciph(t[76]);
    Free_ciph(t[77]);
    // Q18: pmul.
    std::fill(values.begin(),values.end(),-0.00093710692703472703*range);
    plain=Encode_plain(ctx,(name+"/coef_20").c_str(),values.data(),slots,20);
    Mul_plain(ctx,term,t[78],plain); Rescale_ciph(ctx,t[79],term);
    Free_plain(plain);
    // Q18: modswitch.
    Modswitch(ctx,t[80],t[73],18);
    Free_ciph(t[73]);
    // Q18: add.
    Add_ciph(ctx,t[81],t[80],t[79]);
    Free_ciph(t[79]);
    Free_ciph(t[80]);
    // Q18: add.
    Add_ciph(ctx,t[82],t[72],t[81]);
    Free_ciph(t[72]);
    Free_ciph(t[81]);
    // Q16: mul.
    Mul_ciph(ctx,wide,t[69],t[82]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[83],product);
    Free_ciph(t[69]);
    Free_ciph(t[82]);
    // Q22: pmul.
    std::fill(values.begin(),values.end(),0.10567447181736148*range);
    plain=Encode_plain(ctx,(name+"/coef_21").c_str(),values.data(),slots,24);
    Mul_plain(ctx,term,t[60],plain); Rescale_ciph(ctx,t[84],term);
    Free_plain(plain);
    // Q18: pmul.
    std::fill(values.begin(),values.end(),-0.036910639403499486*range);
    plain=Encode_plain(ctx,(name+"/coef_22").c_str(),values.data(),slots,20);
    Mul_plain(ctx,term,t[78],plain); Rescale_ciph(ctx,t[85],term);
    Free_plain(plain);
    // Q18: modswitch.
    Modswitch(ctx,t[86],t[84],18);
    Free_ciph(t[84]);
    // Q18: add.
    Add_ciph(ctx,t[87],t[86],t[85]);
    Free_ciph(t[85]);
    Free_ciph(t[86]);
    // Q18: modswitch.
    Modswitch(ctx,t[88],t[66],18);
    Free_ciph(t[66]);
    // Q16: mul.
    Mul_ciph(ctx,wide,t[88],t[87]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[89],product);
    Free_ciph(t[87]);
    Free_ciph(t[88]);
    // Q22: pmul.
    std::fill(values.begin(),values.end(),0.63341398437092888*range);
    plain=Encode_plain(ctx,(name+"/coef_23").c_str(),values.data(),slots,24);
    Mul_plain(ctx,term,t[60],plain); Rescale_ciph(ctx,t[90],term);
    Free_plain(plain);
    Free_ciph(t[60]);
    // Q18: pmul.
    std::fill(values.begin(),values.end(),-0.20785971588109659*range);
    plain=Encode_plain(ctx,(name+"/coef_24").c_str(),values.data(),slots,20);
    Mul_plain(ctx,term,t[78],plain); Rescale_ciph(ctx,t[91],term);
    Free_plain(plain);
    Free_ciph(t[78]);
    // Q18: modswitch.
    Modswitch(ctx,t[92],t[90],18);
    Free_ciph(t[90]);
    // Q18: add.
    Add_ciph(ctx,t[93],t[92],t[91]);
    Free_ciph(t[91]);
    Free_ciph(t[92]);
    // Q16: modswitch.
    Modswitch(ctx,t[94],t[93],16);
    Free_ciph(t[93]);
    // Q16: add.
    Add_ciph(ctx,t[95],t[89],t[94]);
    Free_ciph(t[89]);
    Free_ciph(t[94]);
    // Q16: add.
    Add_ciph(ctx,t[96],t[83],t[95]);
    Free_ciph(t[83]);
    Free_ciph(t[95]);
    // Q16: modswitch.
    Modswitch(ctx,t[97],t[0],16);
    Free_ciph(t[0]);
    // Q14: mul.
    Mul_ciph(ctx,wide,t[97],t[96]);
    Relin(ctx,product,wide); Rescale_ciph(ctx,t[98],product);
    Free_ciph(t[96]);
    // Q14: pmul.
    std::fill(values.begin(),values.end(),0.5*range);
    plain=Encode_plain(ctx,(name+"/coef_25").c_str(),values.data(),slots,16);
    Mul_plain(ctx,term,t[97],plain); Rescale_ciph(ctx,t[99],term);
    Free_plain(plain);
    Free_ciph(t[97]);
    // Q14: add.
    Add_ciph(ctx,t[100],t[98],t[99]);
    Free_ciph(t[98]);
    Free_ciph(t[99]);
    CIPHER output=t[100];t[100]=nullptr;
    Free_ciph_array(t,101);Free_ciph(term);Free_ciph(product);Free_ciph3(wide);
    End_stage(ctx);
    return output;
}

static void save(CONTEXT ctx,const std::string& name,CIPHER cipher,const std::string& reference,int count) {
    auto expected=padded(read_values(sample(reference),count));
    Save_output(ctx,name.c_str(),cipher,expected.data(),slots,1e-3);
}
static CIPHER activate(CONTEXT ctx,CIPHER input,double range,const std::string& name,
                       const std::string& reference,int count) {
    // Bootstrap requires Q2; discard surplus limbs before refreshing each preactivation.
    if(Cipher_info(ctx,input).q_limbs!=2) {
        auto reduced=New_ciph(ctx);Modswitch(ctx,reduced,input,2);Free_ciph(input);input=reduced;
    }
    Begin_stage(ctx,(name+"/bootstrap").c_str());
    CIPHER refreshed=New_ciph(ctx);Bootstrap(ctx,refreshed,input,24);
    End_stage(ctx);Free_ciph(input);
    save(ctx,name+"_bootstrap",refreshed,reference,count);
    CIPHER result=polynomial_relu(ctx,refreshed,range,name,count);
    Free_ciph(refreshed);
    save(ctx,name,result,name,count);
    return result;
}
static CIPHER stage(CONTEXT ctx,CIPHER input,const Diagonals& d,const std::vector<double>& bias,
                    const std::string& name,int count) {
    Begin_stage(ctx,name.c_str());CIPHER output=linear(ctx,input,d,bias,name);End_stage(ctx);
    Free_ciph(input);save(ctx,name,output,name,count);
    std::printf("%s Q=%u\n",name.c_str(),Cipher_info(ctx,output).q_limbs);std::fflush(stdout);
    return output;
}
void Main_graph(CONTEXT ctx) {
    auto info=Context_info(ctx);
    if(info.ring_degree!=65536 || info.q_limbs!=58) throw std::runtime_error("LeNet requires N65536/Q58");
    auto ranges=read_values("ranges.txt",4);
    // Check the fixed reference inputs against calibrated domains before exporting.
    const char* preactivations[]={"conv1","conv2","fc1","fc2"};
    const int counts[]={4704,1600,120,84};
    for(int i=0;i<4;++i) for(double v:read_values(sample(preactivations[i]),counts[i]))
        if(std::abs(v)>ranges[i]) throw std::runtime_error("ReLU input exceeds calibrated range");
    auto values=padded(read_values(sample("input"),3072));
    CIPHER x=Encrypt_input(ctx,"input",values.data(),slots,4);

    // Conv1 3x32x32 -> 6x28x28; Q4 -> Q2 -> bootstrap Q24 -> ReLU Q14.
    x=stage(ctx,x,conv_weights(read_values("conv1.weight",6*3*25),{3,32,32},6),
            bias_values(read_values("conv1.bias",6),28*28),"conv1",4704);
    x=activate(ctx,x,ranges[0],"relu1","conv1",4704);
    x=stage(ctx,x,pool_weights({6,28,28}),{},"pool1",1176); // Q14 -> Q12.

    // Conv2 6x14x14 -> 16x10x10, then AvgPool -> 16x5x5.
    x=stage(ctx,x,conv_weights(read_values("conv2.weight",16*6*25),{6,14,14},16),
            bias_values(read_values("conv2.bias",16),100),"conv2",1600);
    x=activate(ctx,x,ranges[1],"relu2","conv2",1600);
    x=stage(ctx,x,pool_weights({16,10,10}),{},"pool2",400);

    // Flatten is already represented by the contiguous NCHW slots.
    x=stage(ctx,x,fc_weights(read_values("fc1.weight",120*400),400,120),
            bias_values(read_values("fc1.bias",120),1),"fc1",120);
    x=activate(ctx,x,ranges[2],"relu3","fc1",120);
    x=stage(ctx,x,fc_weights(read_values("fc2.weight",84*120),120,84),
            bias_values(read_values("fc2.bias",84),1),"fc2",84);
    x=activate(ctx,x,ranges[3],"relu4","fc2",84);
    x=stage(ctx,x,fc_weights(read_values("fc3.weight",10*84),84,10),
            bias_values(read_values("fc3.bias",10),1),"fc3",10);
    Free_ciph(x);
}

// Plain verification of the same BSGS packing against independent spatial loops.
static void compare(const std::string& name,const Diagonals& diagonals,
                    const std::vector<double>& x,const std::vector<double>& bias,
                    const std::vector<double>& reference) {
    std::vector<double> actual(slots,0.);
    for(const auto& item:diagonals) {
        int g=item.first/baby_step*baby_step,b=item.first%baby_step;
        auto values=group_plain(item.second,g);
        for(int j=0;j<slots;++j) actual[j]+=values[(j+g)%slots]*x[(j+g+b)%slots];
    }
    double error=0.;
    for(int j=0;j<slots;++j) {
        if(!bias.empty())actual[j]+=bias[j];
        error=std::max(error,std::abs(actual[j]-reference[j]));
    }
    if(error>1e-10)throw std::runtime_error("packing failed: "+name+" error="+std::to_string(error));
    std::printf("PASS %s packing: %zu diagonals, %d slots, max_abs=%.4g\n",name.c_str(),diagonals.size(),slots,error);
}
static void check_packing() {
    struct Conv {const char *name,*input;Shape shape;int outputs;};
    for(auto spec: {Conv{"conv1","input",{3,32,32},6},Conv{"conv2","pool1",{6,14,14},16}}) {
        auto weights=read_values(std::string(spec.name)+".weight",spec.outputs*spec.shape.channels*25);
        int oh=spec.shape.height-4,ow=spec.shape.width-4;
        auto bias=bias_values(read_values(std::string(spec.name)+".bias",spec.outputs),oh*ow);
        auto diagonals=conv_weights(weights,spec.shape,spec.outputs);
        for(int test=0;test<3;++test) {
            auto x=padded(read_values(sample(spec.input),spec.shape.size()));
            if(test==1)for(int j=0;j<spec.shape.size();++j)x[j]=(j%31-15)/64.;
            if(test==2) {std::fill(x.begin(),x.end(),0.);for(int c=0;c<spec.shape.channels;++c){x[c*spec.shape.height*spec.shape.width]=c+1.;x[(c+1)*spec.shape.height*spec.shape.width-1]=-c-1.;}}
            auto expected=bias;
            for(int co=0;co<spec.outputs;++co)for(int h=0;h<oh;++h)for(int w=0;w<ow;++w)
                for(int ci=0;ci<spec.shape.channels;++ci)for(int ky=0;ky<5;++ky)for(int kx=0;kx<5;++kx)
                    expected[(co*oh+h)*ow+w]+=x[(ci*spec.shape.height+h+ky)*spec.shape.width+w+kx]*weights[((co*spec.shape.channels+ci)*5+ky)*5+kx];
            compare(std::string(spec.name)+"/case"+std::to_string(test),diagonals,x,bias,expected);
            if(test==0)compare(spec.name,diagonals,x,bias,padded(read_values(sample(spec.name),spec.outputs*oh*ow)));
        }
    }
    struct Pool {const char *name,*input;Shape shape;};
    for(auto spec:{Pool{"pool1","relu1",{6,28,28}},Pool{"pool2","relu2",{16,10,10}}}) {
        auto x=padded(read_values(sample(spec.input),spec.shape.size()));
        compare(spec.name,pool_weights(spec.shape),x,{},padded(read_values(sample(spec.name),spec.shape.size()/4)));
    }
    struct FC {const char *name,*input;int inputs,outputs;};
    for(auto spec:{FC{"fc1","pool2",400,120},FC{"fc2","relu3",120,84},FC{"fc3","relu4",84,10}}) {
        auto weights=read_values(std::string(spec.name)+".weight",spec.inputs*spec.outputs);
        compare(spec.name,fc_weights(weights,spec.inputs,spec.outputs),padded(read_values(sample(spec.input),spec.inputs)),
                bias_values(read_values(std::string(spec.name)+".bias",spec.outputs),1),padded(read_values(sample(spec.name),spec.outputs)));
    }
}
static void report_prediction() {
    std::ifstream file("result/fc3.json");
    if(!file)throw std::runtime_error("missing result/fc3.json; run make first");
    std::string text((std::istreambuf_iterator<char>(file)),{});
    for(char& c:text)if(c=='[' || c==']' || c==',')c=' ';
    std::istringstream stream(text);std::vector<double> actual;double v;
    while(stream>>v)actual.push_back(v);
    if(actual.size()!=slots)throw std::runtime_error("invalid output slot count");
    auto reference=read_values(sample("fc3"),10),original=read_values(sample("torch"),10);
    int label=int(read_values(sample("label"),1)[0]);
    auto top=[](const std::vector<double>& x){return int(std::max_element(x.begin(),x.begin()+10)-x.begin());};
    double fhe_error=0.,approximation_error=0.;
    for(int i=0;i<10;++i) {
        if(!std::isfinite(actual[i]))throw std::runtime_error("nonfinite output");
        fhe_error=std::max(fhe_error,std::abs(actual[i]-reference[i]));
        approximation_error=std::max(approximation_error,std::abs(reference[i]-original[i]));
    }
    std::printf("sample=%d label=%d encrypted_top1=%d polynomial_top1=%d torch_top1=%d\n",
                CLIB_LENET_SAMPLE,label,top(actual),top(reference),top(original));
    std::printf("logits: FHE max_abs_error=%.9g; polynomial approximation max_abs_error=%.9g\n",fhe_error,approximation_error);
    if(fhe_error>1e-3 || top(actual)!=top(reference) || top(actual)!=top(original))
        throw std::runtime_error("prediction acceptance failed");
}
int main(int argc,char** argv) {
    if(argc==2 && std::string(argv[1])=="--check-packing") {
        try {check_packing();return 0;} catch(const std::exception& e) {std::fprintf(stderr,"%s\n",e.what());return 1;}
    }
    if(argc==2 && std::string(argv[1])=="--report") {
        try {report_prediction();return 0;} catch(const std::exception& e) {std::fprintf(stderr,"%s\n",e.what());return 1;}
    }
    return Run_application(argc,argv,Main_graph);
}
