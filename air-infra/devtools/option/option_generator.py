#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
OPTION generator which reads yaml file as input and generate C++ code
"""

import os
import sys
import argparse
import logging
import re

from enum import Enum, unique

import yaml  # noqa T484

# ASCII character and combination
TILDE = "~"
LCB = "{"    # left curly bracket
RCB = "}"    # right curly bracket
CBS = "{}"   # curly brackets
LRB = "("    # left round bracket
RRB = ")"    # right round bracket
RBS = "()"   # round brackets
LSB = "["   # left square bracket
RSB = "]"   # right square bracket
SBS = "[]"  # square brackets
SEMICOLON = ";"
COLON = ":"
COMMA = ","
AMPERSAND = "&"
UNDERSCORE = "_"
EQUAL = "="
MEMBER_OPERATOR = "."
BACKSLASH = "\\"
ASTERISK = "*"
P_TO = "->"
SLASH = "/"


SPACE1 = " "     # 1 space
SPACE2 = "  "    # 2 space
SPACE4 = "    "  # 4 space

PRAGMA_CHANGEABLE = 0

ARRAY_SIZE_TEMPLATE = "sizeof(%s)/sizeof(%s[0]),"


@unique
class CppCs(Enum):
    """
    C++ comment symbol
    """
    TWO_S = "//"
    SAB = "/*"
    SAE = "*/"


@unique
class CppOp(Enum):
    """
    C++ operator
    """
    SRO = "::"  # scope resolution operator
    ADDR = "&"
    OTPT = "<<"
    QM = "?"


@unique
class CppKw(Enum):
    """
    C++ keyword
    """
    STRUCT = "struct"
    PUBLIC = "public"
    STATIC = "static"
    CONST = "const"
    VOID = "void"
    RETURN = "return"
    BOOL = "bool"
    SIZEOF = "sizeof"
    TRUE = "true"
    FALSE = "false"
    ELSE = "else"
    INT64 = "int64_t"
    UINT64 = "uint64_t"
    DOUBLE = "double"
    IFNDEF = "#ifndef"
    ENDIF = "#endif"
    DEFINE = "#define"
    INCLD = "#include"
    USING = "using"
    NMS = "namespace"
    THIS = "this"


@unique
class CppSC(Enum):
    """
    C++ standard class name
    """
    STR = "std::string"
    STR_VIEW = "std::string_view"
    OSTREAM = "std::ostream"


@unique
class CppFunc(Enum):
    """
    C++ standard function
    """
    ENDL = "std::endl"


@unique
class SelfDi(Enum):
    """
    Self defined variable names and file path
    """
    CTX = "ctx"
    CFG = "cfg"     # config
    OTS = "os"       # output object
    DECL = "DECLARE"
    ACS = "ACCESS_API"
    DCCAA = "DECLARE_COMMON_CONFIG_ACCESS_API"
    HSUFFIX = "_H"
    CNSUFFIX = "_OPTION_CONFIG"
    OHF = "option_config.h"
    OCXXF = "option_config.inc"
    YES = "Yes"
    NO = "No"


@unique
class AcDcn(Enum):
    """
    Ant compiler defined class name, should be consistent with the symbols
    in air-infra/include/air/util/option.h file
    """
    OPD = "OPTION_DESC"
    ODH = "OPTION_DESC_HANDLE"
    OPG = "OPTION_GRP"
    COMC = "COMMON_CONFIG"
    AUOM = "air::util::OPTION_MGR"
    AUCC = "air::util::COMMON_CONFIG"
    AUDC = "air::driver::DRIVER_CTX"


@unique
class AcDns(Enum):
    """
    Ant compiler defined namespace
    in air-infra/include/air/util/option.h file
    """
    AIR_UTIL = "air::util"


@unique
class AcDmn(Enum):
    """
    Ant compiler defined macro name, should be consistent with the symbols
    in air-infra/include/air/driver/common_config.h file
    """
    DCC = "DECLARE_COMMON_CONFIG"


@unique
class AcOk(Enum):
    """
    Ant compiler option kind, should be consistent with the symbols
    in air-infra/include/air/util/option.h file
    """
    INVALID = "air::util::K_INVALID"
    NONE = "air::util::K_NONE"
    BOOL = "air::util::K_BOOL"
    INT64 = "air::util::K_INT64"
    UINT64 = "air::util::K_UINT64"
    STR = "air::util::K_STR"
    DOUBLE = "air::util::K_DOUBLE"


@unique
class AcOvm(Enum):
    """
    Ant compiler option value maker, should be consistent with the symbols
    in air-infra/include/air/util/option.h file
    """
    NONE = "air::util::V_NONE"
    SPACE = "air::util::K_SPACE"
    EQUAL = "air::util::V_EQUAL"
    NONE_SPACE = "air::util::V_NONE_SPACE"


@unique
class AcOmf(Enum):
    """
    Ant compiler option manager function name, should be consistent with the symbols
    in air-infra/include/air/util/option.h file
    """
    RTO = "Register_top_level_option"
    RGO = "Register_option_group"


@unique
class ODF(Enum):
    """
    default option function
    """
    ROP = "Register_options"
    UOP = "Update_options"
    PRINT = "Print"


@unique
class YmlOsk(Enum):
    """
    OPTION kinds supported in yaml files
    """
    INT = "int"
    UINT = "uint"
    STR = "str"
    DOUBLE = "double"


@unique
class YmlOsvm(Enum):
    """
    OPTION value makers supported in yaml files
    """
    EQUAL = "="
    SPACE = "space"
    NON_OR_SPACE = "non_or_space"


def double_quotes(info: str):
    """
    double quotes the input
    :param info: str content which needs to be enclosed in double quotes
    :return: content in double quotes
    """
    return f'"{info}"'


def single_quotes(info: str):
    """
    single quotes the input
    :param info: str content which needs to be enclosed single quotes
    :return: content in single quotes
    """
    return f"'{info}'"


def prefix_(var: str):
    """
    add underscore prefix to the input
    :param var: str variable which needs to add underscore prefix
    :return: variable with underscore prefix
    """
    return UNDERSCORE + var


class OpGroup:
    """
    option group class
    """

    def __init__(self, name: str, description: str, separator: str, value_maker: str):
        """
        constructor function
        :param name: option name
        :param description: option description
        :param separator: separator between options
        :param value_maker: sign between option and value
        """
        self._name = name
        self._desp = description
        self._sep = separator
        self.set_value_maker(value_maker)

    def get_value_maker(self) -> str:
        """
        get option group value maker
        :return: option group value maker
        """
        return self._value_maker

    def set_value_maker(self, value):
        """
        assign given value to value maker
        :param value: value want to assign to value maker
        """
        if value == YmlOsvm.EQUAL.value:
            self._value_maker = AcOvm.EQUAL.value
        else:
            raise AttributeError("OPTION group's value maker only support '='")

    def __str__(self):
        return f"OpGroup: {self._name}, Description: {self._desp}, " \
               f"Separator: {self._sep}, Value Maker: {self.get_value_maker()}"


class OPTION:
    """
    option class
    """

    def __init__(self, name: str, abbrev_name: str, description: str,
                 op_type: str, value: str, value_maker: str):
        """
        constructor function
        :param name: option name
        :param abbrev_name: abbreviation name
        :param description: option description
        :param op_type: option type
        :param value: its value
        :param value_maker: option value maker
        """
        self._name = name.replace("-", "_")
        self._abbrev_name = abbrev_name
        self._desp = description
        self._type = op_type
        self._value = value
        self._value_maker = value_maker

    def __str__(self):
        return SPACE1.join([self._name, self._abbrev_name, self._desp,
                            str(self._type), str(self._value)])

    @property
    def name(self):
        """
        get option name
        :return: op name
        """
        return self._name

    @name.setter
    def name(self, value):
        """
        assign value to option name and replace '-' with '_' in value
        :param value: op name
        """
        self._name = value.replace("-", "_")

    def get_kind(self) -> AcOk:
        """
        option kind
        @return:
        """
        if self._type == YmlOsk.STR.value:
            return AcOk.STR
        if self._type == YmlOsk.INT.value:
            return AcOk.INT64
        if self._type == YmlOsk.UINT.value:
            return AcOk.UINT64
        if self._type == YmlOsk.DOUBLE.value:
            return AcOk.DOUBLE
        return AcOk.NONE

    def get_kind_str(self) -> str:
        """
        option kind literal
        @return:
        """
        return self.get_kind().value

    def get_value_maker(self):
        """
        option value maker
        @return:
        """
        if self._value_maker == YmlOsvm.EQUAL.value:
            return AcOvm.EQUAL.value
        if self._value_maker == YmlOsvm.SPACE.value:
            return AcOvm.SPACE.value
        if self._value_maker == YmlOsvm.NON_OR_SPACE.value:
            return AcOvm.NONE_SPACE.value
        return AcOvm.NONE.value

    def get_cpp_type(self, var_type: bool):
        """
        get C++ builtin type
        @param var_type: indicate return C++ string or string_view
        @return: CppSC enum type
        """
        if self._type == YmlOsk.STR.value:
            if var_type:
                return CppSC.STR
            return CppSC.STR_VIEW
        if self._type == YmlOsk.INT.value:
            return CppKw.INT64
        if self._type == YmlOsk.UINT.value:
            return CppKw.UINT64
        if self._type == YmlOsk.DOUBLE.value:
            return CppKw.DOUBLE
        return CppKw.BOOL

    def get_cpp_type_str(self, var_type: bool) -> str:
        """
        get C++ builtin type literal
        @param var_type: indicate return C++ string or string_view
        @return: C++ builtin type literal
        """
        return self.get_cpp_type(var_type).value

    def get_var_type(self):
        """
        get C++ builtin type
        @return:
        """
        return self.get_cpp_type(True).value

    def get_return_type(self) -> str:
        """
        get C++ builtin type
        @return:
        """
        return self.get_cpp_type_str(True)

    def get_value(self):
        """
        get option value literal
        @return: option value literal
        """
        # if self._value in ["off", "OFF"]:
        #     return "\"off\""
        if self._type in (YmlOsk.INT.value, YmlOsk.UINT.value, YmlOsk.DOUBLE.value, YmlOsk.STR.value):
            return str(self._value)
        return CppKw.FALSE.value


class MacroFty:
    """
    Access api factory to generate access api
    """

    def __init__(self, name: str, option_meta: list):
        """
        constructor function
        @param name: class name
        @param option_meta:
        """
        self._name = name
        self._acs_macro_name = UNDERSCORE.join([SelfDi.DECL.value,
                                                self._name, SelfDi.ACS.value])
        self._op_detail_macro_name = UNDERSCORE.join([SelfDi.DECL.value, self._name])
        self._op_meta = option_meta

    def get_access_macro_def(self):
        """
        get access maro defination
        """
        macro_param = SelfDi.CFG.value
        macro_var = "".join([self._acs_macro_name, LRB, macro_param, RRB])
        begin = SPACE1.join([CppKw.DEFINE.value, macro_var, BACKSLASH])

        funcs = []
        for var_info in self._op_meta:
            func_name = var_info._name.capitalize()
            func_name_param = "".join([func_name, LRB, CppKw.VOID.value, RRB])
            func_signature = SPACE1.join([var_info.get_return_type(),
                                          func_name_param, CppKw.CONST.value])
            body_content = "".join([macro_param, MEMBER_OPERATOR, func_name, RBS, SEMICOLON])
            func_body = SPACE1.join([CppKw.RETURN.value, body_content])
            func = SPACE1.join([func_signature, LCB, func_body, RCB, BACKSLASH])
            funcs.append(SPACE2 + func)

        end = "".join([SPACE2, SelfDi.DCCAA.value, LRB, macro_param, RRB])

        return begin, funcs, end

    def get_option_detail_macro_def(self):
        """
        get option detail macro defination
        """
        if self._op_meta:
            macro_param = SelfDi.CFG.value
            macro_var = "".join([self._op_detail_macro_name, LRB, macro_param, RRB])
            begin = SPACE1.join([CppKw.DEFINE.value, macro_var, BACKSLASH])
            init_values = []
            for op_item in self._op_meta:
                var_addr = "".join([AMPERSAND, macro_param, MEMBER_OPERATOR,
                                    prefix_(op_item._name)])
                body = ", ".join(
                    [double_quotes(op_item._name), double_quotes(op_item._abbrev_name),
                     double_quotes(op_item._desp), var_addr, op_item.get_kind_str(),
                     str(PRAGMA_CHANGEABLE), op_item.get_value_maker()])
                init_value = SPACE1.join([LCB, body, RCB + COMMA, BACKSLASH])
                init_values.append(SPACE2 + init_value)
            return begin, init_values
        return None, None


class ClassFty:
    """
    Class factory to generate C++ class
    """

    def __init__(self, name: str, option_meta: list):
        """
        constructor function
        @param name: class name
        @param option_meta:
        """
        self._name = name
        self._op_meta = option_meta

    def get_class_begin(self):
        """
        the prologue of C++ class definition
        @return: the prologue of C++ class definition literal
        """
        begin = SPACE1.join([CppKw.STRUCT.value, self._name, COLON, CppKw.PUBLIC.value,
                             AcDcn.AUCC.value, LCB])
        return begin

    def get_constructor(self):
        """
        C++ constructor
        @return: C++ constructor definition literal
        """
        ctor_signature = "".join([self._name, LRB, CppKw.VOID.value,
                                  RRB, COLON, "\n"])
        ctor_init_list = []
        op_len = len(self._op_meta)
        for i, var_info in enumerate(self._op_meta):
            if i != op_len - 1:
                member_init = "".join(
                    [SPACE4, prefix_(var_info._name), LRB, var_info.get_value(),
                     RRB, COMMA, "\n"])
            else:
                member_init = "".join(
                    [SPACE4, prefix_(var_info._name), LRB, var_info.get_value(),
                     RRB])
                member_init = SPACE1.join([member_init, CBS])
            ctor_init_list.append(member_init)

        para_init_str = "".join(ctor_init_list)

        constructor = "".join([ctor_signature, para_init_str])
        return SPACE2 + constructor

    def get_destructor(self):
        """
        C++ destructor
        @return: C++ destructor definition literal
        """
        destructor_signature = "".join([TILDE, self._name, LRB, CppKw.VOID.value,
                                        RRB])
        destructor = SPACE1.join([destructor_signature, CBS])
        return SPACE2 + destructor

    @classmethod
    def get_required_member_func(cls, print_stmts: list):
        """
        required member function, currently only Register_options, Update_options and Print
        @return: member function definition literal
        """
        void_sym = CppKw.VOID.value
        ro_func_param = "".join([AcDcn.AUDC.value, ASTERISK, SPACE1,
                                 SelfDi.CTX.value])
        ro_func = "".join([ODF.ROP.value, LRB, ro_func_param, RRB, SEMICOLON])
        ro_func_signature = SPACE1.join([void_sym, ro_func])

        uo_func = "".join([ODF.UOP.value, RBS, SEMICOLON])
        uo_func_signature = SPACE1.join([void_sym, uo_func])

        p_func_name = ODF.PRINT.value
        func_pname = SelfDi.OTS.value
        p_func_param = "".join([CppSC.OSTREAM.value, CppOp.ADDR.value,
                                SPACE1, func_pname])
        base_print_stmt = "".join([AcDcn.COMC.value, CppOp.SRO.value,
                                   p_func_name, LRB, func_pname, RRB, SEMICOLON])
        other_print_stmt = "\n".join(print_stmts)
        p_func_body = "\n".join([SPACE4 + base_print_stmt, other_print_stmt])

        p_func = "".join([p_func_name, LRB, p_func_param, RRB])
        p_func_sign_head = SPACE1.join([void_sym, p_func, CppKw.CONST.value, LCB])
        p_func_signature = "\n".join([p_func_sign_head, p_func_body, SPACE2 + RCB])
        return ro_func_signature, uo_func_signature, p_func_signature

    def get_member_func(self):
        """
        C++ class member function
        @return: C++ class member function definition literal
        """

        funcs = []
        print_stmts = []
        for var_info in self._op_meta:
            pvar_name = prefix_(var_info._name)
            name_param = "".join([var_info._name.capitalize(), LRB, CppKw.VOID.value, RRB])
            func_signature = SPACE1.join([var_info.get_return_type(), name_param])
            func_body = SPACE1.join([CppKw.RETURN.value, pvar_name + SEMICOLON])
            func = SPACE1.join([func_signature, CppKw.CONST.value, LCB, func_body, RCB])
            funcs.append(SPACE2 + func)

            print_head = SPACE1.join([SelfDi.OTS.value, CppOp.OTPT.value, double_quotes(
                var_info._name + COLON + SPACE1), CppOp.OTPT.value])
            print_tail = SPACE1.join([CppOp.OTPT.value, CppFunc.ENDL.value + SEMICOLON])
            if var_info.get_cpp_type(True) == CppKw.BOOL:
                cond_expr = SPACE1.join([pvar_name, CppOp.QM.value, double_quotes(
                    SelfDi.YES.value), COLON, double_quotes(SelfDi.NO.value)])
                cond_expr_with_bracket = "".join([LRB, cond_expr, RRB])
                print_stmt = SPACE1.join([print_head, cond_expr_with_bracket, print_tail])
            else:
                print_stmt = SPACE1.join([print_head, pvar_name, print_tail])
            print_stmts.append(SPACE4 + print_stmt)

        ro_func, uo_func, print_func = self.get_required_member_func(print_stmts)
        funcs.append(SPACE2 + ro_func)
        funcs.append(SPACE2 + uo_func)
        funcs.append(SPACE2 + print_func)
        return funcs

    def get_member_var(self):
        """
        C++ class member variables
        @return: C++ class member variables literal
        """
        member_var = []
        for var_info in self._op_meta:
            type_var = SPACE1.join([var_info.get_var_type(), prefix_(var_info._name)])
            var_declare = "".join([type_var, SEMICOLON])
            member_var.append(SPACE2 + var_declare)
        return member_var

    def get_class_end(self):
        """
        the epilogue of C++ class definition
        @return: the epilogue of C++ class definition literal
        """
        comment_info = SPACE1.join([CppCs.TWO_S.value, CppKw.STRUCT.value,
                                    self._name])
        return "".join([RCB, SEMICOLON, comment_info])


class StmtFty:
    """
    Statement factory to generate C++ statements
    """

    def __init__(self, class_name: str, ns: list, group_info: OpGroup, top_option_meta: list,
                 group_option_meta: list):
        """
        constructor function
        @param class_name:
        @param group_info:
        @param top_option_meta:
        @param group_option_meta:
        """
        self._has_top_option = False
        self._has_grp_option = False
        self._class_name = class_name
        self._ns = ns
        self._grp_info = group_info
        self._obj_name = self._class_name.lower()
        self._cap_obj_name = self._obj_name.capitalize()
        self._op_desc_var_name = UNDERSCORE.join([self._cap_obj_name, "desc"])
        self._op_dh_var_name = UNDERSCORE.join([self._cap_obj_name, "desc_handle"])
        self._top_option_meta = top_option_meta
        self._grp_option_meta = group_option_meta
        if self._top_option_meta:
            self._has_top_option = True
        if self._grp_info:
            self._grp_var_name = UNDERSCORE.join([self._grp_info._name.capitalize(), "option_grp"])
            self._has_grp_option = True

    def get_class_object_def(self):
        """
        C++ class object definition statement
        @return:
        """
        return SPACE1.join([CppKw.STATIC.value, self._class_name,
                            self._cap_obj_name + SEMICOLON])

    @staticmethod
    def get_array_def(option_var_name: str, class_name: str, object_name: str):
        """
        define option desc array
        @param option_var_name:
        @param object_name:
        @return:
        """
        begin = SPACE1.join([CppKw.STATIC.value, AcDcn.OPD.value,
                             option_var_name, SBS, EQUAL, LCB])

        param_list = "".join([LRB, object_name, RRB])
        common_macro = "".join([SPACE2 + AcDmn.DCC.value, param_list, COMMA])
        custom_macro_name = UNDERSCORE.join([SelfDi.DECL.value, class_name])
        custom_macro = "".join([SPACE2 + custom_macro_name, param_list])

        init_values = []
        init_values.append(common_macro)
        init_values.append(custom_macro)

        end = "".join([RCB, SEMICOLON])
        return begin, init_values, end

    @staticmethod
    def get_handle_def(option_var_name: str, oph_var_name: str, options_meta: list):
        """
        define option handle
        @param option_var_name:
        @param oph_var_name:
        @param options_meta:
        @return:
        """
        if options_meta:
            left_value = SPACE1.join([CppKw.STATIC.value, AcDcn.ODH.value,
                                      oph_var_name, EQUAL])
            right_value = SPACE1.join([LCB, ARRAY_SIZE_TEMPLATE %
                                       (option_var_name, option_var_name),
                                       option_var_name, RCB + SEMICOLON])
            return "\n".join([left_value, right_value])
        return None

    def grp_option_desc_array_def(self):
        """
        define group option array
        @return:
        """
        return StmtFty.get_array_def(self._op_desc_var_name, self._class_name,
                                     self._cap_obj_name)

    def grp_option_desc_handle_def(self):
        """
        define group option handle
        @return:
        """
        return StmtFty.get_handle_def(self._op_desc_var_name,
                                      self._op_dh_var_name,
                                      self._grp_option_meta)

    def grp_mgr_def(self):
        """
        define group manager
        @return:
        """
        if self._has_grp_option:
            var_addr = "".join([AMPERSAND, self._op_dh_var_name])
            left_value = SPACE1.join(
                [CppKw.STATIC.value, AcDcn.OPG.value, self._grp_var_name, EQUAL])
            right_value_body = ", ".join([double_quotes(self._grp_info._name),
                                          double_quotes(self._grp_info._desp),
                                          single_quotes(self._grp_info._sep),
                                          self._grp_info.get_value_maker(),
                                          var_addr])
            right_value = SPACE1.join([LCB, right_value_body,
                                       RCB + SEMICOLON])
            return "\n".join([left_value, right_value])
        return None

    def register_func_def(self):
        """
        define register option function
        @return:
        """
        local_var = SelfDi.CTX.value
        cn_func = "".join([self._class_name, CppOp.SRO.value, ODF.ROP.value])
        p2dc_type = AcDcn.AUDC.value + ASTERISK
        func_signature = SPACE1.join([CppKw.VOID.value, cn_func,
                                      LRB, p2dc_type, local_var, RRB, LCB])
        register_grp = "".join([local_var, P_TO, AcOmf.RGO.value, LRB, AMPERSAND])

        register_grp_var = "".join([register_grp, self._grp_var_name, RRB, SEMICOLON])
        return "\n".join([func_signature, SPACE2 + register_grp_var, RCB])

    def update_func_def(self):
        """
        define update options function
        @return:
        """
        var_name = ASTERISK + CppKw.THIS.value
        cn_func = "".join([self._class_name, CppOp.SRO.value, ODF.UOP.value])
        func_signature = SPACE1.join([CppKw.VOID.value, cn_func, RBS, LCB])
        assign_stmt = SPACE1.join([var_name, EQUAL, self._cap_obj_name + SEMICOLON])

        return "\n".join([func_signature, SPACE2 + assign_stmt, RCB])


class MISC:
    """
    Miscellaneous content of C++ file, consists of comment, include files and namespace .etc
    """

    def __init__(self, class_name: str, comment: str, include_file: list, namespace: list):
        """
        constructor function
        @param class_name: class name
        @param comment: comment information
        @param include_file: include file list
        @param ns: namespace list
        """
        self._class_name = class_name
        self._comment = comment
        self._include_file = include_file
        self._namespace = namespace
        self._up_namespace = [ns.upper() for ns in namespace]

    def get_comment_info(self):
        """
        get comment information
        """
        return self._comment

    def get_gen_header_file(self):
        """
        get generated formatted C++ include file information
        @return:
        """
        ns_file_path = SLASH.join(self._namespace)
        slash_file_path = SLASH.join([ns_file_path, SelfDi.OHF.value])
        incld_f_stmt = MISC.format_include_file(slash_file_path)
        return incld_f_stmt

    @staticmethod
    def get_use_ns() -> str:
        """
        get use of namespace
        @return:
        """
        using_ns = SPACE1.join([CppKw.USING.value, CppKw.NMS.value,
                                AcDns.AIR_UTIL.value])
        return using_ns + SEMICOLON

    def get_format_yaml_header_file(self):
        """
        get formatted C++ include file information
        @return:
        """
        incld_f_stmt = [MISC.format_include_file(f) for f in self._include_file]
        result = '\n'.join(incld_f_stmt)
        return result

    @staticmethod
    def format_include_file(file_path) -> str:
        """
        format include file
        """
        def incld_directive(f): return SPACE1.join([CppKw.INCLD.value, double_quotes(f)])
        return incld_directive(file_path)

    def get_namespace_prologue(self):
        """
        get formatted C++ namespace prologue
        @return:
        """
        def ns_prologue(n): return SPACE1.join([CppKw.NMS.value, n, LCB])
        nsp_stmt = [ns_prologue(n) for n in self._namespace]
        result = '\n'.join(nsp_stmt)
        return result

    def get_namespace_epilogue(self):
        """
        get formatted C++ namespace epilogue
        @return:
        """
        def ns_comment_info(n): return SPACE1.join([CppCs.TWO_S.value,
                                                    CppKw.NMS.value, n])
        nse_stmt = [RCB + SPACE2 + ns_comment_info(n) for n in reversed(self._namespace)]
        result = '\n'.join(nse_stmt)
        return result

    def get_guard_include_macro_prologue(self):
        """
        get guard include macro
        @return:
        """
        ns_str = UNDERSCORE.join(self._up_namespace)
        macro_value = "".join([ns_str, UNDERSCORE, self._class_name, SelfDi.HSUFFIX.value])
        ifndef_macro_directive = SPACE1.join([CppKw.IFNDEF.value, macro_value])
        def_macro_directive = SPACE1.join([CppKw.DEFINE.value, macro_value])
        macro_directives = [ifndef_macro_directive, def_macro_directive]
        result = '\n'.join(macro_directives)
        return result

    def get_guard_include_macro_epilogue(self):
        """
        get guard include macro
        @return:
        """
        ns_str = UNDERSCORE.join(self._up_namespace)
        macro_value = "".join([ns_str, UNDERSCORE, self._class_name, SelfDi.HSUFFIX.value])
        comment_macro = SPACE1.join([CppCs.TWO_S.value, macro_value])
        endif_macro_directive = SPACE2.join([CppKw.ENDIF.value, comment_macro])
        return endif_macro_directive


class FileGen:
    """
    Generate C++ .h file
    """

    def __init__(self, misc: MISC, class_factory: ClassFty,
                 macro_factory: MacroFty,
                 statement_factory: StmtFty):
        """
        constructor function
        @param misc:
        @param class_factory:
        @param macro_factory:
        @param statement_factory:
        """
        self._misc = misc
        self._class_fty = class_factory
        self._macro_fty = macro_factory
        self._stmt_fty = statement_factory

    def write_file_header(self, f):
        """
        writing C++ .h file header
        """
        f.write(self._misc.get_comment_info())
        f.write("\n\n")
        f.write(self._misc.get_guard_include_macro_prologue())
        f.write("\n\n")
        f.write(self._misc.get_format_yaml_header_file())
        f.write("\n\n")
        f.write(self._misc.get_namespace_prologue())
        f.write("\n\n")

    def write_file_tail(self, f):
        """
        writing file tail
        """
        f.write("\n\n")
        f.write(self._misc.get_namespace_epilogue())
        f.write("\n\n")
        f.write(self._misc.get_guard_include_macro_epilogue())

    def write_class_def(self, f):
        """
        writing C++ class definition
        """
        f.write(self._class_fty.get_class_begin())
        f.write("\n")
        f.write(self._class_fty.get_constructor())
        f.write("\n")
        f.write(self._class_fty.get_destructor())
        f.write("\n\n")

        for func in self._class_fty.get_member_func():
            f.write(func)
            f.write("\n")

        f.write("\n")
        for var in self._class_fty.get_member_var():
            f.write(var)
            f.write("\n")
        f.write(self._class_fty.get_class_end())

    def write_option_access_api_macro(self, f):
        """
        writing option access api macro
        """
        f.write("\n\n")
        begin, init_values, end = self._macro_fty.get_access_macro_def()
        f.write(begin)
        f.write("\n")
        for init_value in init_values:
            f.write(init_value)
            f.write("\n")
        f.write(end)
        f.write("\n")

    def write_option_detail_macro(self, f):
        """
        writing C++ deltail macro
        """
        f.write("\n\n")
        begin, init_values = self._macro_fty.get_option_detail_macro_def()
        f.write(begin)
        f.write("\n")
        for init_value in init_values:
            f.write(init_value)
            f.write("\n")

    def write_stmt(self, f):
        """
        writing C++ statements
        """
        if self._stmt_fty._has_grp_option:
            f.write("\n")
            f.write(self._stmt_fty.get_class_object_def())
            f.write("\n\n")
            begin, init_values, end = self._stmt_fty.grp_option_desc_array_def()
            f.write(begin)
            f.write("\n")
            for init_value in init_values:
                f.write(init_value)
                f.write("\n")
            f.write(end)
            f.write("\n\n")

            f.write(self._stmt_fty.grp_option_desc_handle_def())

            f.write("\n\n")
            f.write(self._stmt_fty.grp_mgr_def())

        f.write("\n\n")
        f.write(self._stmt_fty.register_func_def())

        f.write("\n\n")
        f.write(self._stmt_fty.update_func_def())

    def write_dot_h_file(self):
        """
        main entry to write C++ .h file
        """
        with open(SelfDi.OHF.value, 'w', encoding="utf-8") as f:
            self.write_file_header(f)
            self.write_class_def(f)
            self.write_option_access_api_macro(f)
            self.write_option_detail_macro(f)
            self.write_file_tail(f)

    def write_cxx_file(self):
        """
        main entry to write C++ .h file
        """

        with open(SelfDi.OCXXF.value, 'w', encoding="utf-8") as f:
            f.write(self._misc.get_comment_info())
            f.write("\n\n")
            f.write(self._misc.get_gen_header_file())
            f.write("\n\n")
            f.write(self._misc.get_use_ns())
            f.write("\n\n")
            f.write(self._misc.get_namespace_prologue())
            f.write("\n\n")
            self.write_stmt(f)
            f.write("\n\n")
            f.write(self._misc.get_namespace_epilogue())

    def write_file(self):
        """
        main entry to write C++ .h file
        """
        self.write_dot_h_file()
        self.write_cxx_file()


class InputChecker:
    """
    input checker
    """

    def __init__(self, raw_data):
        """
        construction function
        @param raw_data:
        """
        self._raw_data = raw_data

    def check_comment_info(self):
        """
        check whether the comment is legal in C++
        """
        comment_info = self._raw_data.get("comment_info")
        if comment_info is not None:
            if not (comment_info.startswith(CppCs.TWO_S.value) or
                    (comment_info.startswith(CppCs.SAB.value) and
                     comment_info.endswith(CppCs.SAE.value))):
                logging.error("comment info must use C++ comment symbol")
                sys.exit(1)

    def check_namespace(self):
        """
        check whether the namespace is legal
        """
        namespace = self._raw_data.get("namespace")
        if namespace is None:
            logging.error("namespace must be provided!")
            sys.exit(1)
        if not re.match(r'^[a-z0-9_,]+$', namespace):
            logging.error("Namespace name only use lowercase letters, numbers and underscores, \
                          seperated by comma")
            sys.exit(1)

    def check_header_file(self):
        """
        check whether the header files are legal in C++
        """
        header_files = self._raw_data.get("header_file")
        if header_files is None:
            logging.error("header files must be provided!")
            sys.exit(1)

    @staticmethod
    def check_group_info(group_info: dict):
        """
        check whether the group information is legal
        @param group_info:
        """
        assert group_info is not None
        name = group_info.get("name")
        desp = group_info.get("description")
        sep = group_info.get("separator")
        vm = group_info.get("value_maker")
        if not all(v is not None for v in [name, desp, sep, vm]):
            logging.error("group name, description, separator, value_maker all must be provided!")
            sys.exit(1)

        if not re.match(r'^[A-Z0-9_]+$', str(name)):
            logging.error("group name only use upper letters, numbers and underscores")
            sys.exit(1)

        if not str(name)[0].isalpha():
            logging.error("The first character of group name must be letters")
            sys.exit(1)

    @staticmethod
    def check_cpp_var_rule(var: str, display_info: str):
        """
        check whether the option name is legal in C++ variable naming conventions
        @param var:
        @param display_info:
        """
        if var is None:
            logging.error("%s must be provided", display_info.capitalize())
            sys.exit(1)
        if not re.match(r'^[A-Za-z0-9_]+$', var):
            print(var)
            logging.error("%s only use letters, numbers and underscores", display_info.capitalize())
            sys.exit(1)
        if not var[0].isalpha():
            logging.error("The first character of %s must be letters", display_info)
            sys.exit(1)

    @staticmethod
    def check_option(raw_option: list):
        """
        check whether the option metadata is legal
        @param raw_option:
        """
        for op_item in raw_option:
            InputChecker.check_cpp_var_rule(op_item.get("name"), "option name")
            if op_item.get("abbrev_name"):
                InputChecker.check_cpp_var_rule(op_item.get("abbrev_name"), "option abbrev_name")

            op_type = op_item.get("kind")
            if op_type not in [None, YmlOsk.INT.value,
                               YmlOsk.UINT.value, YmlOsk.STR.value, YmlOsk.DOUBLE.value]:
                logging.error("option kind is not supported")
                sys.exit(1)

            if op_item.get("description") is None:
                logging.error("option description must be provided")
                sys.exit(1)

            op_value = op_item.get("value")
            if op_type is None and op_value is not None:
                logging.error("when option kind is not provided,"
                              " option value should also not be provided")
                sys.exit(1)

            if op_type is not None and op_value is None:
                logging.error("when option kind is provided, option value must be provided")
                sys.exit(1)

            if op_type == YmlOsk.STR.value:
                # if op_value not in ["off", "OFF"]:
                if len(op_value) >= 100:
                    logging.error(
                        "when option kind is str, the length of option value must be less than 100")
                    sys.exit(1)
            elif op_type == YmlOsk.INT.value:
                if not isinstance(op_value, int):
                    logging.error("when option kind is int, option value must be integer")
                    sys.exit(1)
            elif op_type == YmlOsk.UINT.value:
                if not (isinstance(op_value, int) and op_value >= 0):
                    logging.error("when option kind is uint, option value must be integer >= 0")
                    sys.exit(1)

    def check(self):
        """
        check legality
        """
        self.check_comment_info()
        self.check_namespace()
        self.check_header_file()

        raw_tl_option = self._raw_data.get("option")
        if raw_tl_option:
            InputChecker.check_option(raw_tl_option)

        group_info = self._raw_data.get("group")
        if group_info is None and raw_tl_option is None:
            logging.error("common option or group option must be provided")
            sys.exit(1)

        if group_info:
            InputChecker.check_group_info(group_info)
            raw_grp_option = group_info.get("option")
            if raw_grp_option:
                InputChecker.check_option(raw_grp_option)

            if raw_tl_option is None and raw_grp_option is None:
                logging.error("at least one option or group option must be provided")
                sys.exit(1)


class FileProcessor:
    """
    yaml file processor
    """

    def __init__(self, file_path: str) -> None:
        """
        constructor function
        @param file_path: file path
        """
        self._file_path = file_path

    def read_file(self):
        """
        read yaml file
        @return:
        """
        try:
            with open(self._file_path, 'r', encoding="utf-8") as config_file:
                content = yaml.load(config_file, Loader=yaml.SafeLoader)
                return content
        except FileNotFoundError:
            logging.error("File '{self._file_path}' not found.")
            sys.exit(1)
        except yaml.scanner.ScannerError as error:
            logging.error("please check yaml file format:")
            print(error)
            sys.exit(1)

    @staticmethod
    def process_content(raw_data):
        """
        process yaml file content which contains option metadata
        @param raw_data:
        @return:
        """
        InputChecker(raw_data).check()

        option_all = []
        grp_option = []
        group_object = None
        class_name = SelfDi.CNSUFFIX.value
        group_info = raw_data.get("group")
        if group_info:
            class_name = group_info.get("name") + class_name
            group_object = OpGroup(group_info.get("name"), group_info.get("description"),
                                   group_info.get("separator"), group_info.get("value_maker"))
            raw_grp_option = group_info.get("option")
            if raw_grp_option:
                grp_option = FileProcessor.construct_option(raw_grp_option)
                option_all += grp_option

        tl_option = []
        raw_tl_option = raw_data.get("option")
        if raw_tl_option:
            tl_option = FileProcessor.construct_option(raw_tl_option)
            option_all += tl_option

        incld_fs = raw_data.get("header_file").split(',')
        ns = raw_data.get("namespace").split(',')
        file_header = MISC(class_name, raw_data.get("comment_info"), incld_fs, ns)
        class_factory = ClassFty(class_name, option_all)
        access_api_factory = MacroFty(class_name, option_all)
        stmt_factory = StmtFty(class_name, ns, group_object,
                               tl_option, grp_option)
        file_generator = FileGen(file_header, class_factory, access_api_factory, stmt_factory)
        return file_generator

    @staticmethod
    def construct_option(raw_option: list):
        """
        construct option object list
        @param raw_option:
        @return:
        """
        assert len(raw_option) != 0
        option = []
        for op_item in raw_option:
            op_name = op_item.get("name")
            op_type = op_item.get("kind")
            op_abbrev_name = op_item.get("abbrev_name", "")
            op_desp = op_item.get("description")
            op_value = op_item.get("value", False)
            op_value_maker = op_item.get("value_maker")
            op_obj = OPTION(op_name, op_abbrev_name, op_desp, op_type, op_value, op_value_maker)
            option.append(op_obj)

        return option


def get_parser():
    """
    get command line parser object
    @return:
    """
    parser = argparse.ArgumentParser(description='parse yml file and generate C++ files')
    parser.add_argument('--input-path', '-ip', type=str, dest='input_path', required=False,
                        help='Path of input yml file')
    return parser


def main():
    """
    option generator main entry
    """
    parser = get_parser()
    args = parser.parse_args()
    if not os.path.exists(args.input_path):
        logging.error("input yml file path does not exist, please check!")
        sys.exit(1)
    if not args.input_path.endswith(".yml") and not args.input_path.endswith(".yaml"):
        logging.error("no yml file, please check!")
        sys.exit(1)

    processor = FileProcessor(args.input_path)
    content = processor.read_file()
    file_generator = FileProcessor.process_content(content)
    file_generator.write_file()


if __name__ == '__main__':
    main()
