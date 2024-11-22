#ifndef FHE_TEST_E2E_DRIVER_H
#error  "e2e_driver.h must be included"
#endif

#ifdef FHE_TEST_E2E_DRIVER_INC
#error "e2e_driver.inc must be included only once"
#endif
#define FHE_TEST_E2E_DRIVER_INC

//<! @brief ir builder for e2e test
class IR_BUILDER {
public:
  typedef std::vector< std::pair<TYPE_PTR, const char*> > PARM_VEC;

  IR_BUILDER(GLOB_SCOPE* glob) : _glob(glob) {
    _spos = glob->Unknown_simple_spos();
  }

  TYPE_PTR S32_type() {
    return _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  }

  TYPE_PTR Arr_f32(size_t elem) {
    TYPE_PTR etype  = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
    STR_PTR ty_name = _glob->New_str("array");
    ARB_PTR arb     = _glob->New_arb(1, 0, elem, 1);
    return _glob->New_arr_type(ty_name, etype, arb, _spos);
  }

  FUNC_SCOPE* New_func(const char* name, const PARM_VEC& parm) {
    STR_PTR name_str = _glob->New_str(name);
    FUNC_PTR func = _glob->New_func(name_str, _spos);
    func->Set_parent(_glob->Comp_env_id());
    SIGNATURE_TYPE_PTR sig = _glob->New_sig_type();
    _glob->New_ret_param(_glob->Prim_type(PRIMITIVE_TYPE::VOID), sig);
    for (auto arg : parm) {
      _glob->New_param(arg.second, arg.first, sig, _spos);
    }
    sig->Set_complete();
    ENTRY_PTR entry =
      _glob->New_global_entry_point(sig, func, name_str, _spos);
    FUNC_SCOPE* scope = &_glob->New_func_scope(func);
    _func = scope;
    _cntr = &scope->Container();
    _cntr->New_func_entry(_spos);
    return scope;
  }

  ADDR_DATUM_PTR Formal(uint32_t idx) const {
    return _func->Formal(idx);
  }

  ADDR_DATUM_PTR New_var(TYPE_PTR type, const char* name) {
    STR_PTR name_str = _glob->New_str(name);
    return _func->New_var(type, name_str, _spos);
  }

  NODE_PTR New_cust_node(air::base::OPCODE opc, TYPE_PTR rtype) {
    return _cntr->New_cust_node(opc, rtype, _spos);
  }

  NODE_PTR New_ld(ADDR_DATUM_PTR var) {
    return _cntr->New_ld(var, _spos);
  }

  NODE_PTR New_const_s32(int32_t val) {
    return _cntr->New_intconst(S32_type(), val, _spos);
  }

  STMT_PTR New_st(NODE_PTR node, ADDR_DATUM_PTR var) {
    return _cntr->New_st(node, var, _spos);
  }

  STMT_PTR New_retv(NODE_PTR node) {
    return _cntr->New_retv(node, _spos);
  }

  void Append(STMT_PTR stmt) {
    _cntr->Stmt_list().Append(stmt);
  }

private:
  GLOB_SCOPE* _glob;
  FUNC_SCOPE* _func;
  CONTAINER*  _cntr;
  SPOS        _spos;
};

// E2E_DRIVER::Init_opcode
void fhe::test::E2E_DRIVER::Init_opcode() {
  bool ret;
  ret = air::core::Register_core();
  AIR_ASSERT(ret);
  ret = nn::core::Register_nn();
  AIR_ASSERT(ret);
  ret = nn::vector::Register_vector_domain();
  AIR_ASSERT(ret);
  ret = fhe::sihe::Register_sihe_domain();
  AIR_ASSERT(ret);
  ret = fhe::ckks::Register_ckks_domain();
  AIR_ASSERT(ret);
  ret = fhe::poly::Register_polynomial();
  AIR_ASSERT(ret);
}

// main
int main() {
  int            argc   = 2;
  const char*    argv[] = {"driver", "/dev/null"};
  fhe::test::E2E_DRIVER driver;
  driver.Init(argc, (char**)argv);
  driver.Pre_run();
  driver.Run();
  driver.Post_run();
  driver.Fini();

  return 0;
}
