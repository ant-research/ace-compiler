#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
Base class for code generator
'''

from cg.config import Config  # noqa: T484


class BaseCG:
    '''
    Base class for code generator
    '''

    def __init__(self, config: Config):
        self.config = config

    def get_config(self) -> Config:
        ''' Get the config '''
        return self.config

    def generate_operator(self) -> str:
        ''' Generate operator code '''
        return ""

    def generate_domain(self) -> str:
        ''' Generate domain code '''
        return ""

    def generate(self) -> str:
        ''' Generate the whole file '''
        return ""

    def generate_includes(self) -> str:
        ''' Generate include files '''
        content = ""
        if self.config.has_includes():
            for include in self.config.get_lib_includes():
                content += f"#include <{include}>\n"
            for include in self.config.cpp_user_include:
                content += f"#include \"{include}\"\n"
            content += "\n"
        return content

    def generate_namespace_prefix(self) -> str:
        ''' Generate namespace prefix '''
        content = ""
        if self.config.has_namespaces():
            for namespace in self.config.cpp_namespace:
                content += f"namespace {namespace} {{\n"
            content += "\n"
        return content

    def generate_namespace_suffix(self) -> str:
        ''' Generate namespace suffix '''
        content = ""
        if self.config.has_namespaces():
            for namespace in self.config.cpp_namespace:
                content += f"}} // namespace {namespace}\n"
            content += "\n"
        return content

    def generate_using_namespace(self) -> str:
        ''' Generate using namespace '''
        content = ""
        if self.config.using_namespace:
            for namespace in self.config.using_namespace:
                content += f"using namespace {namespace};\n"
            content += "\n"
        return content
