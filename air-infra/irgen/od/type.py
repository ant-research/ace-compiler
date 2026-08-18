#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
This module defines the TypeDescEntry class.

Classes:
    TypeDescEntry
    TypeTrait
'''


class TypeTrait:
    '''
    Type trait for DSL intermediate representation.
    
    TODO: to be completed.
    '''

    def __init__(self):
        pass

    def __str__(self):
        return "TypeTrait()"


class TypeDescEntry:
    '''
    Type for DSL intermediate representation.
    '''

    def __init__(self, name: str, trait: TypeTrait):
        self._name = name
        self._trait = trait

    def __str__(self):
        return f"TypeDescEntry(name={self.name}, trait={self.trait})"

    @property
    def name(self) -> str:
        ''' Getter for name of the type. '''
        return self._name

    @property
    def trait(self) -> TypeTrait:
        ''' Getter for trait of the type. '''
        return self._trait
