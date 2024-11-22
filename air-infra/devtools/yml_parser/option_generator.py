#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception


import os
import argparse
import yaml
import logging
import re

from enum import Enum, unique

# ASCII character and combination
TILDE = "~"
LCURLY_BRACKET = "{"
RCURLY_BRACKET = "}"
CURLY_BRACKETS = "{}"
LROUND_BRACKET = "("
RROUND_BRACKET = ")"
LSQUARE_BRACKET = "["
RSQUARE_BRACKET = "]"
SQUARE_BRACKETS = "[]"
SEMICOLON = ";"
COLON = ":"
COMMA = ","
AMPERSAND = "&"
UNDERSCORE = "_"
EQUAL = "="
MEMBER_OPERATOR = "."

ONE_SPACE = " "
TWO_SPACE = "  "
FOUR_SPACE = "    "

PRAGMA_CHANGEABLE = 0

array_size_template = "sizeof(%s)/sizeof(%s[0]),"


@unique
class CPPCommentSymbol(Enum):
    TS = "//"
    SAB = "/*"
    SAE = "*/"


@unique
class CPPKeyword(Enum):
    STRUCT = "struct"
    PUBLIC = "public"
    STATIC = "static"
    VOID = "void"
    RETURN = "return"
    BOOL = "bool"
    SIZEOF = "sizeof"
    TRUE = "true"
    FALSE = "false"
    IF = "if"
    ELSE = "else"
    INT64 = "int64_t"
    UINT64 = "uint64_t"


@unique
class CPPStandardClass(Enum):
    STR = "std::string"
    STR_VIEW = "std::string_view"


# should be consistent with the symbols in air-infra/include/air/util/option.h file
@unique
class ACDefinedClassName(Enum):
    OD = "air::util::OPTION_DESC"
    ODH = "air::util::OPTION_DESC_HANDLE"
    OG = "air::util::OPTION_GRP"
    OM = "air::util::OPTION_MGR"
    CC = "air::util::COMMON_CONFIG"


# should be consistent with the symbols in air-infra/include/air/util/option.h file
@unique
class ACOptionKind(Enum):
    INVALID = "air::util::K_INVALID"
    NONE = "air::util::K_NONE"
    BOOL = "air::util::K_BOOL"
    INT64 = "air::util::K_INT64"
    UINT64 = "air::util::K_UINT64"
    STR = "air::util::K_STR"


@unique
class ACOptionValueMaker(Enum):
    NONE = "air::util::V_NONE"
    SPACE = "air::util::K_SPACE"
    EQUAL = "air::util::V_EQUAL"
    NONE_SPACE = "air::util::V_NONE_SPACE"


# should be consistent with the symbols in air-infra/include/air/util/option.h file
@unique
class ACOptionManagerFunc(Enum):
    RTO = "Register_top_level_option"
    RGO = "Register_option_group"


@unique
class YamlOptionSupportKind(Enum):
    INT = "int"
    UINT = "uint"
    STR = "str"


@unique
class YamlOptionSupportValueMaker(Enum):
    EQUAL = "="
    SPACE = "space"
    NON_OR_SPACE = "non_or_space"


def double_quotes(info: str):
    return f'"{info}"'


def single_quotes(info: str):
    return f"'{info}'"


def underscore_prefix(var: str):
    return UNDERSCORE + var


class OptionGroup:
    def __init__(self, name: str, description: str, separator: str, value_maker: str):
        self.name = name
        self.desp = description
        self.sep = separator
        self.set_value_maker(value_maker)

    def get_value_maker(self) -> str:
        return self._value_maker

    def set_value_maker(self, value):
        if value == YamlOptionSupportValueMaker.EQUAL.value:
            self._value_maker = ACOptionValueMaker.EQUAL.value
        else:
            raise AttributeError("Option group's value maker only support '='")

    def __str__(self):
        return f"OptionGroup: {self.name}, Description: {self.desp}, " \
               f"Separator: {self.sep}, Value Maker: {self.get_value_maker()}"


class Option:
    def __init__(self, name: str, abbrev_name: str, description: str, op_type: str, value: str, value_maker: str):
        self.name = name
        self.abbrev_name = abbrev_name
        self.desp = description
        self.type = op_type
        self.value = value
        self.value_maker = value_maker

    def __str__(self):
        return ONE_SPACE.join([self.name, self.abbrev_name, self.desp, str(self.type), str(self.value)])

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, value):
        self._name = value.replace("-", "_")

    def get_kind(self):
        if self.type is None:
            return ACOptionKind.NONE.value
        elif self.type == YamlOptionSupportKind.STR.value:
            return ACOptionKind.STR.value
        elif self.type == YamlOptionSupportKind.INT.value:
            return ACOptionKind.INT64.value
        elif self.type == YamlOptionSupportKind.UINT.value:
            return ACOptionKind.UINT64.value

    def get_value_maker(self):
        if self.value_maker is None:
            return ACOptionValueMaker.NONE.value
        elif self.value_maker == YamlOptionSupportValueMaker.EQUAL.value:
            return ACOptionValueMaker.EQUAL.value
        elif self.value_maker == YamlOptionSupportValueMaker.SPACE.value:
            return ACOptionValueMaker.SPACE.value
        elif self.value_maker == YamlOptionSupportValueMaker.NON_OR_SPACE.value:
            return ACOptionValueMaker.NONE_SPACE.value

    def _get_cpp_type(self, var_type: bool):
        if self.type is None:
            return CPPKeyword.BOOL.value
        elif self.type == YamlOptionSupportKind.STR.value:
            if var_type:
                return CPPStandardClass.STR.value
            else:
                return CPPStandardClass.STR_VIEW.value
        elif self.type == YamlOptionSupportKind.INT.value:
            return CPPKeyword.INT64.value
        elif self.type == YamlOptionSupportKind.UINT.value:
            return CPPKeyword.UINT64.value

    def get_var_type(self):
        return self._get_cpp_type(True)

    def get_return_type(self):
        return self._get_cpp_type(False)

    def get_value(self):
        if self.value is False:
            return CPPKeyword.FALSE.value
        elif self.value in ["off", "OFF"]:
            return "\"off\""
        elif self.type == YamlOptionSupportKind.INT.value or \
                self.type == YamlOptionSupportKind.UINT.value:
            return str(self.value)


class ClassFactory:
    def __init__(self, name: str, option_meta: list):
        self.name = name
        self.option_meta = option_meta

    def get_class_begin(self):
        begin = ONE_SPACE.join([CPPKeyword.STRUCT.value, self.name, COLON, CPPKeyword.PUBLIC.value,
                                ACDefinedClassName.CC.value, LCURLY_BRACKET])
        return begin

    def get_constructor(self):
        constructor_signature = "".join([self.name, LROUND_BRACKET, CPPKeyword.VOID.value, RROUND_BRACKET, COLON])

        member_var_val_list = []
        for var_info in self.option_meta:
            member_var_val = "".join(
                [underscore_prefix(var_info.name), LROUND_BRACKET, var_info.get_value(), RROUND_BRACKET])
            member_var_val_list.append(member_var_val)

        para_init_list = COMMA.join(member_var_val_list)

        constructor = ONE_SPACE.join([constructor_signature, para_init_list, CURLY_BRACKETS])
        return TWO_SPACE + constructor

    def get_destructor(self):
        destructor_signature = "".join([TILDE, self.name, LROUND_BRACKET, CPPKeyword.VOID.value, RROUND_BRACKET])
        destructor = ONE_SPACE.join([destructor_signature, CURLY_BRACKETS])
        return TWO_SPACE + destructor

    def get_member_func(self):
        funcs = []
        for var_info in self.option_meta:
            name_param = "".join([var_info.name.capitalize(), LROUND_BRACKET, CPPKeyword.VOID.value, RROUND_BRACKET])
            func_signature = ONE_SPACE.join([var_info.get_return_type(), name_param])
            func_body = ONE_SPACE.join([CPPKeyword.RETURN.value, underscore_prefix(var_info.name), SEMICOLON])
            func = ONE_SPACE.join([func_signature, LCURLY_BRACKET, func_body, RCURLY_BRACKET])
            funcs.append(TWO_SPACE + func)
        return funcs

    def get_member_var(self):
        member_var = []
        for var_info in self.option_meta:
            type_var = ONE_SPACE.join([var_info.get_var_type(), underscore_prefix(var_info.name)])
            var_declare = "".join([type_var, SEMICOLON])
            member_var.append(TWO_SPACE + var_declare)
        return member_var

    def get_class_end(self):
        comment_info = ONE_SPACE.join([CPPCommentSymbol.TS.value, CPPKeyword.STRUCT.value, self.name])
        return "".join([RCURLY_BRACKET, SEMICOLON, comment_info])


class StatementFactory:
    def __init__(self, class_name: str, group_info: OptionGroup, top_option_meta: list, group_option_meta: list):
        self.has_top_option = False
        self.has_grp_option = False
        self.class_name = class_name
        self.grp_info = group_info
        self.object_name = self.class_name.lower()
        self.option_var_name = UNDERSCORE.join([self.object_name, "option"])
        self.grp_option_var_name = UNDERSCORE.join(["grp", self.option_var_name])
        self.option_handle_var_name = UNDERSCORE.join([self.object_name, "option_handle"])
        self.grp_option_handle_var_name = UNDERSCORE.join(["grp", self.option_handle_var_name])
        self.top_option_meta = top_option_meta
        self.grp_option_meta = group_option_meta
        if self.top_option_meta:
            self.has_top_option = True
        if self.grp_info:
            self.grp_var_name = UNDERSCORE.join(["option_grp", self.grp_info.name.lower()])
            self.has_grp_option = True

    def get_class_object_def(self):
        return ONE_SPACE.join([CPPKeyword.STATIC.value, self.class_name, self.object_name + SEMICOLON])

    @staticmethod
    def get_array_def(option_var_name: str, object_name: str, option_meta: list):
        if option_meta is not None:
            begin = ONE_SPACE.join([CPPKeyword.STATIC.value, ACDefinedClassName.OD.value, option_var_name,
                                    SQUARE_BRACKETS, EQUAL, LCURLY_BRACKET])
            init_values = []
            for op_item in option_meta:
                var_addr = "".join([AMPERSAND, object_name, MEMBER_OPERATOR, underscore_prefix(op_item.name)])
                body = ", ".join(
                    [double_quotes(op_item.name), double_quotes(op_item.abbrev_name), double_quotes(op_item.desp),
                     var_addr, op_item.get_kind(), str(PRAGMA_CHANGEABLE), op_item.get_value_maker()])
                init_value = ONE_SPACE.join([LCURLY_BRACKET, body, RCURLY_BRACKET + COMMA])
                init_values.append(TWO_SPACE + init_value)
            end = "".join([RCURLY_BRACKET, SEMICOLON])
            return begin, init_values, end
        else:
            return None, None, None

    def get_option_array_def(self):
        return StatementFactory.get_array_def(self.option_var_name, self.object_name, self.top_option_meta)

    @staticmethod
    def get_handle_def(option_var_name: str, option_handle_var_name: str, options_meta: list):
        if options_meta:
            left_value = ONE_SPACE.join(
                [CPPKeyword.STATIC.value, ACDefinedClassName.ODH.value, option_handle_var_name, EQUAL])
            right_value = ONE_SPACE.join([LCURLY_BRACKET, array_size_template % (option_var_name, option_var_name),
                                          option_var_name, RCURLY_BRACKET + SEMICOLON])
            return "\n".join([left_value, right_value])

    def option_handle_def(self):
        return StatementFactory.get_handle_def(self.option_var_name, self.option_handle_var_name, self.top_option_meta)

    def grp_option_array_def(self):
        return StatementFactory.get_array_def(self.grp_option_var_name, self.object_name, self.grp_option_meta)

    def grp_option_handle_def(self):
        return StatementFactory.get_handle_def(self.grp_option_var_name, self.grp_option_handle_var_name,
                                               self.grp_option_meta)

    def grp_mgr_def(self):
        if self.has_grp_option:
            var_addr = "".join([AMPERSAND, self.grp_option_handle_var_name])
            left_value = ONE_SPACE.join(
                [CPPKeyword.STATIC.value, ACDefinedClassName.OG.value, self.grp_var_name, EQUAL])
            right_value_body = ", ".join([double_quotes(self.grp_info.name), double_quotes(self.grp_info.desp),
                                          single_quotes(self.grp_info.sep), self.grp_info.get_value_maker(), var_addr])
            right_value = ONE_SPACE.join([LCURLY_BRACKET, right_value_body, RCURLY_BRACKET + SEMICOLON])
            return "\n".join([left_value, right_value])

    def register_func_def(self):
        _OP_MRG_VAR = "option_mgr"
        _STANDALONE_VAR = "standalone"
        register_func_name = "Register_options_" + self.object_name

        func_signature = ONE_SPACE.join(
            [CPPKeyword.STATIC.value, CPPKeyword.VOID.value, register_func_name, LROUND_BRACKET,
             ACDefinedClassName.OM.value + AMPERSAND, _OP_MRG_VAR + ",",
             CPPKeyword.BOOL.value, _STANDALONE_VAR, RROUND_BRACKET, LCURLY_BRACKET])
        register_top = "".join([_OP_MRG_VAR, MEMBER_OPERATOR, ACOptionManagerFunc.RTO.value, LROUND_BRACKET, AMPERSAND])
        register_grp = "".join([_OP_MRG_VAR, MEMBER_OPERATOR, ACOptionManagerFunc.RGO.value, LROUND_BRACKET, AMPERSAND])
        register_option_handle = "".join([register_top, self.option_handle_var_name, RROUND_BRACKET, SEMICOLON])
        if_statement = "".join([CPPKeyword.IF.value, LROUND_BRACKET, _STANDALONE_VAR, RROUND_BRACKET])

        if self.has_grp_option:
            register_grp_option_handle = "".join(
                [register_top, self.grp_option_handle_var_name, RROUND_BRACKET, SEMICOLON])
            register_grp_var = "".join([register_grp, self.grp_var_name, RROUND_BRACKET, SEMICOLON])
            if self.has_top_option:
                return "\n".join([func_signature, TWO_SPACE + register_option_handle, TWO_SPACE + if_statement,
                                  FOUR_SPACE + register_grp_option_handle, TWO_SPACE + CPPKeyword.ELSE.value,
                                  FOUR_SPACE + register_grp_var, RCURLY_BRACKET])
            else:
                return "\n".join([func_signature, TWO_SPACE + if_statement, FOUR_SPACE + register_grp_option_handle,
                                  TWO_SPACE + CPPKeyword.ELSE.value, FOUR_SPACE + register_grp_var, RCURLY_BRACKET])
        else:
            return "\n".join([func_signature, TWO_SPACE + register_option_handle, RCURLY_BRACKET])

    def update_func_def(self):
        _UPDATE_FUNC_NAME = "Update_options"
        func_param_var = self.object_name + "_option"
        func_signature = ONE_SPACE.join(
            [CPPKeyword.STATIC.value, CPPKeyword.VOID.value, _UPDATE_FUNC_NAME, LROUND_BRACKET,
             self.class_name + AMPERSAND, func_param_var, RROUND_BRACKET, LCURLY_BRACKET])
        assign_statement = ONE_SPACE.join([func_param_var, EQUAL, self.object_name + SEMICOLON])

        return "\n".join([func_signature, TWO_SPACE + assign_statement, RCURLY_BRACKET])


class FileHeader:
    def __init__(self, comment: str, include_info: list):
        self.comment = comment
        self.include_info = include_info

    @property
    def include_info(self):
        return self._include_info

    @include_info.setter
    def include_info(self, value):
        self._include_info = "#include " + double_quotes(value)


class FileGenerator:
    def __init__(self, file_header: FileHeader, class_factory: ClassFactory, statement_factory: StatementFactory):
        self.file_header = file_header
        self.class_factory = class_factory
        self.statement_factory = statement_factory

    def write_file_header(self):
        print(self.file_header.comment)
        print("\n")
        print(self.file_header.include_info)
        print("\n")

    def write_class_def(self):
        print(self.class_factory.get_class_begin())
        print(self.class_factory.get_constructor())
        print(self.class_factory.get_destructor())

        print("\n")
        for func in self.class_factory.get_member_func():
            print(func)
        print("\n")
        for var in self.class_factory.get_member_var():
            print(var)
        print(self.class_factory.get_class_end())

    def write_statement(self):
        if self.statement_factory.has_top_option:
            print("\n")
            print(self.statement_factory.get_class_object_def())
            print("\n")
            begin, init_values, end = self.statement_factory.get_option_array_def()
            print(begin)
            for init_value in init_values:
                print(init_value)
            print(end)
            print(self.statement_factory.option_handle_def())

        if self.statement_factory.has_grp_option:
            print("\n")
            begin, init_values, end = self.statement_factory.grp_option_array_def()
            print(begin)
            for init_value in init_values:
                print(init_value)
            print(end)
            print(self.statement_factory.grp_option_handle_def())

            print("\n")
            print(self.statement_factory.grp_mgr_def())

        print("\n")
        print(self.statement_factory.register_func_def())

        print("\n")
        print(self.statement_factory.update_func_def())

    def write_file(self):
        self.write_file_header()
        self.write_class_def()
        self.write_statement()


class InputChecker:
    def __init__(self, raw_data):
        self.raw_data = raw_data

    def check_class_name(self):
        class_name = self.raw_data.get("class_name")
        if class_name is None:
            logging.error("Class name must be provided!")
            exit(1)

        if not re.match(r'^[A-Z0-9_]+$', class_name):
            logging.error("Class name only use upper letters, numbers and underscores")
            exit(1)

        if not class_name[0].isalpha():
            logging.error("The first character of class name must be letters")
            exit(1)

    def check_comment_info(self):
        comment_info = self.raw_data.get("comment_info")
        if comment_info is not None:
            if not (comment_info.startswith(CPPCommentSymbol.TS.value) or
                    (comment_info.startswith(CPPCommentSymbol.SAB.value) and
                     comment_info.endswith(CPPCommentSymbol.SAE.value))):
                logging.error("comment info must use C++ comment symbol")
                exit(1)

    @staticmethod
    def check_group_info(group_info: dict):
        assert (group_info is not None)
        name = group_info.get("name")
        desp = group_info.get("description")
        sep = group_info.get("separator")
        vm = group_info.get("value_maker")
        if not all(v is not None for v in [name, desp, sep, vm]):
            logging.error("group name, description, separator, value_maker all must be provided!")
            exit(1)

    @staticmethod
    def check_cpp_variable_rule(var: str, display_info: str):
        if var is None:
            logging.error("%s must be provided" % display_info.capitalize())
            exit(1)
        if not re.match(r'^[A-Za-z0-9_]+$', var):
            print(var)
            logging.error("%s only use letters, numbers and underscores" % display_info.capitalize())
            exit(1)
        if not var[0].isalpha():
            logging.error("The first character of %s must be letters" % display_info)
            exit(1)

    @staticmethod
    def check_option(raw_option: list):
        for op_item in raw_option:
            InputChecker.check_cpp_variable_rule(op_item.get("name"), "option name")
            if op_item.get("abbrev_name"):
                InputChecker.check_cpp_variable_rule(op_item.get("abbrev_name"), "option abbrev_name")

            op_type = op_item.get("kind")
            if op_type not in [None, YamlOptionSupportKind.INT.value,
                               YamlOptionSupportKind.UINT.value, YamlOptionSupportKind.STR.value]:
                logging.error("option kind is not supported")
                exit(1)

            if op_item.get("description") is None:
                logging.error("option description must be provided")
                exit(1)

            op_value = op_item.get("value")
            if op_type is None and op_value is not None:
                logging.error("when option kind is not provided, option value should also not be provided")
                exit(1)

            if op_type is not None and op_value is None:
                logging.error("when option kind is provided, option value must be provided")
                exit(1)

            if op_type == YamlOptionSupportKind.STR.value:
                if op_value not in ["off", "OFF"]:
                    logging.error("when option kind is str, option value must be off/OFF")
                    exit(1)
            elif op_type == YamlOptionSupportKind.INT.value:
                if not isinstance(op_value, int):
                    logging.error("when option kind is int, option value must be integer")
                    exit(1)
            elif op_type == YamlOptionSupportKind.UINT.value:
                if not (isinstance(op_value, int) and op_value > 0):
                    logging.error("when option kind is uint, option value must be integer > 0")
                    exit(1)

    def check(self):
        self.check_class_name()
        self.check_comment_info()

        raw_tl_option = self.raw_data.get("option")
        if raw_tl_option:
            InputChecker.check_option(raw_tl_option)

        group_info = self.raw_data.get("group")
        if group_info is None and raw_tl_options is None:
            logging.error("common option or group option must be provided")
            exit(1)

        if group_info:
            InputChecker.check_group_info(group_info)
            raw_grp_option = group_info.get("option")
            if raw_grp_option:
                InputChecker.check_option(raw_grp_option)

            if raw_tl_option is None and raw_grp_option is None:
                logging.error("at least one option or group option must be provided")
                exit(1)


class FileProcessor:
    def __init__(self, file_path: str) -> None:
        self.file_path = file_path

    def read_file(self):
        try:
            with open(self.file_path, 'r') as option_file:
                content = yaml.load(option_file, Loader=yaml.SafeLoader)
                return content
        except FileNotFoundError:
            logging.error("File '{self.file_path}' not found.")
            exit(1)
        except yaml.scanner.ScannerError as error:
            logging.error("please check yaml file format:")
            print(error)
            exit(1)

    @staticmethod
    def process_content(raw_data):
        InputChecker(raw_data).check()

        option_all = []
        grp_option = []
        group_object = None
        group_info = raw_data.get("group")
        if group_info:
            group_object = OptionGroup(group_info.get("name"), group_info.get("description"),
                                       group_info.get("separator"), group_info.get("value_maker"))
            raw_grp_option = group_info.get("option")
            if raw_grp_option:
                grp_option = FileProcessor._construct_option(raw_grp_option)
                option_all += grp_option

        tl_option = []
        raw_tl_option = raw_data.get("option")
        if raw_tl_option:
            tl_option = FileProcessor._construct_option(raw_tl_option)
            option_all += tl_option
        file_header = FileHeader(raw_data.get("comment_info"), raw_data.get("header_file"))
        class_factory = ClassFactory(raw_data.get("class_name"), option_all)
        statement_factory = StatementFactory(raw_data.get("class_name"), group_object, tl_option, grp_option)
        file_generator = FileGenerator(file_header, class_factory, statement_factory)
        return file_generator

    @staticmethod
    def _construct_option(raw_option: list):
        assert (len(raw_option) != 0)
        option = []
        for op_item in raw_option:
            op_name = op_item.get("name")
            op_type = op_item.get("kind")
            op_abbrev_name = op_item.get("abbrev_name", "")
            op_desp = op_item.get("description")
            op_value = op_item.get("value", False)
            op_value_maker = op_item.get("value_maker")
            op_obj = Option(op_name, op_abbrev_name, op_desp, op_type, op_value, op_value_maker)
            option.append(op_obj)

        return option


def get_parser():
    parser = argparse.ArgumentParser(description='parse yml file and generate option .inc file')
    parser.add_argument('--input-path', '-ip', type=str, dest='input_path', required=False,
                        help='Path of input yml file')
    return parser


def main():
    parser = get_parser()
    args = parser.parse_args()
    if not os.path.exists(args.input_path):
        logging.error("input yml file path does not exist, please check!")
        exit(1)
    if not args.input_path.endswith(".yml") and not args.input_path.endswith(".yaml"):
        logging.error("no yml file, please check!")
        exit(1)

    processor = FileProcessor(args.input_path)
    content = processor.read_file()
    file_generator = FileProcessor.process_content(content)
    file_generator.write_file()


if __name__ == '__main__':
    main()
