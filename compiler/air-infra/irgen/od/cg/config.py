#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
Config class for the code generator
'''

from typing import TypeVar

T = TypeVar('T', bound='Config')

CPP_LICENSE = '''
//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================\n
'''


class Config:
    '''
    Config class for the code generator
    '''

    def __init__(self):
        self.filename = ""
        self.cpp_macro_guard = ""
        self.cpp_lib_include = []
        self.cpp_user_include = []
        self.cpp_namespace = []
        self.using_namespace = []

    def __str__(self):
        return (f"Config: {self.filename}, "
                f"{self.cpp_macro_guard}, "
                f"{self.cpp_lib_include}, "
                f"{self.cpp_user_include}, "
                f"{self.cpp_namespace}")

    def add_include(self, include: str, is_lib: bool) -> T:
        ''' Add include file '''
        if is_lib:
            self.cpp_lib_include.append(include)
        else:
            self.cpp_user_include.append(include)
        return self

    def add_namespace(self, namespace: str) -> T:
        ''' Add namespace '''
        self.cpp_namespace.append(namespace)
        return self

    def set_macro_guard(self, guard: str) -> T:
        ''' Set macro guard '''
        self.cpp_macro_guard = guard
        return self

    def get_macro_guard(self) -> str:
        ''' Get macro guard '''
        return self.cpp_macro_guard

    def has_includes(self) -> bool:
        ''' Check if there are includes '''
        return (len(self.cpp_user_include) > 0) or (len(self.cpp_lib_include)
                                                    > 0)

    def has_namespaces(self) -> bool:
        ''' Check if there are namespaces '''
        return len(self.cpp_namespace) > 0

    def get_lib_includes(self) -> list:
        ''' Get lib includes '''
        return self.cpp_lib_include

    def get_user_includes(self) -> list:
        ''' Get user includes '''
        return self.cpp_user_include

    def get_namespaces(self) -> list:
        ''' Get namespaces '''
        return self.cpp_namespace

    def get_cpp_license(self) -> str:
        ''' Get license '''
        return CPP_LICENSE

    def has_macro_guard(self) -> bool:
        ''' Check if there is a macro guard '''
        return self.cpp_macro_guard != ""

    def add_using_namespace(self, namespace: str) -> T:
        ''' Add using namespace '''
        self.using_namespace.append(namespace)

    def get_using_namespace(self) -> list:
        ''' Get using namespace '''
        return self.using_namespace

    def has_using_namespace(self) -> bool:
        ''' Check if there are using namespaces '''
        return len(self.using_namespace) > 0
