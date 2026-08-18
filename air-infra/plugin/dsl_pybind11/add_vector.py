from air_dsl import *

dsl = DSL()
v = Vector(dsl)
spos = SPOS(0, 1, 1, 0)

# create type
f32 = dsl.getPrimType(PrimTypeEnum.FLOAT_32)
f32_arr_ty = dsl.getArrayType("f32_arr", f32, [4], spos)

f = dsl.newFunc("add_vector", spos)

f_sig_ty = dsl.newSigType()
dsl.addParm("a", f32_arr_ty, f_sig_ty, spos)
dsl.addParm("b", f32_arr_ty, f_sig_ty, spos)
dsl.addRet(f32_arr_ty, f_sig_ty, spos)
dsl.setSigComplete(f_sig_ty)
dsl.newEntryPoint(f_sig_ty, f, spos)
formal_a = dsl.Formal(0)
formal_b = dsl.Formal(1)
var_c = dsl.newVar("c", f32_arr_ty, spos)
node_a = dsl.Ld(formal_a, spos)
node_b = dsl.Ld(formal_b, spos)
node_add = v.add(node_a, node_b, spos)
dsl.St(node_add, var_c, spos)
node_c = dsl.Ld(var_c, spos)
dsl.Retv(node_c, spos)
ir = dsl.getCurFuncScope().toString()
print(ir)




