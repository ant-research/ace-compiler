// One SEAL application graph, exported and executed by the clib driver.
#include "seal/seal.h"
#include "clib.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <string>
#include <iostream>

void ResNet_cifar10_seal_sparse(size_t, size_t, size_t);
void ResNet_cifar100_seal_sparse(size_t, size_t, size_t);
static long layers=20, dataset=10, sample=0;
static long option(const char* name, long fallback) {
    const char* s=std::getenv(name);
    if (!s) return fallback;
    size_t used=0; long value=std::stol(s, &used);
    if (s[used]) throw std::invalid_argument(std::string("invalid ")+name);
    return value;
}
static void validate_data() {
    const auto root=std::filesystem::path(CNN_DATA_ROOT);
    for (const auto& entry:std::filesystem::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file() || entry.path().extension()!=".txt") continue;
        std::ifstream in(entry.path()); double x; size_t count=0;
        while (in>>x) { if (!std::isfinite(x)) throw std::runtime_error("nonfinite data: "+entry.path().string()); ++count; }
        if (!in.eof() || !count) throw std::runtime_error("invalid/empty data: "+entry.path().string());
        if (entry.path().filename()=="test_values.txt" && count!=3*3072)
            throw std::runtime_error("expected exactly three input samples");
        if (entry.path().filename()=="test_label.txt" && count!=3)
            throw std::runtime_error("expected exactly three labels");
    }
}
static void Main_graph(clib::CONTEXT ctx) {
    const auto info=clib::Context_info(ctx);
    if(info.ring_degree!=65536 || info.q_limbs!=58) throw std::runtime_error("FHE-MP-CNN requires N65536/Q58");
    seal::ClibSession session(ctx);
    validate_data();
    clib::Begin_stage(ctx, "resnet");
    if (dataset==10) ResNet_cifar10_seal_sparse(layers,sample,sample);
    else ResNet_cifar100_seal_sparse(layers,sample,sample);
    clib::End_stage(ctx);
}
int main(int argc,char** argv) {
    try {
        layers=option("CNN_LAYERS",20); dataset=option("CNN_DATASET",10);
        sample=option("CNN_START",0);
        if (!((layers==20 && dataset==10)||(layers==32 && dataset==100)))
            throw std::invalid_argument("supported models: ResNet20/CIFAR-10 and ResNet32/CIFAR-100");
        if (sample<0 || sample>2 || option("CNN_END",sample)!=sample)
            throw std::invalid_argument("select one fixed sample: 0, 1 or 2");
        // Upstream text reports are references, kept alongside the actual ANT outputs.
        for(int i=1;i+1<argc;++i) if(std::string(argv[i])=="--output") {
            std::filesystem::create_directories(argv[i+1]);
            setenv("CNN_RESULT_DIR",argv[i+1],1);
        }
        return clib::Run_application(argc,argv,Main_graph);
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
}
