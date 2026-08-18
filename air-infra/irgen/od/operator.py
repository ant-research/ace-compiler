#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
Defines classes for operator descriptions and arguments
Classes:
    ArgEntry: Argument of the operator.
    OpDescEntry: Operator for DSL intermediate representation.
'''

import inspect
from typing import TypeVar, Generic
from .type import TypeDescEntry
from .property import OprProp

T = TypeVar('T', bound='OpDescEntry')
V = TypeVar('V')


class ArgEntry:
    '''
    Argument of the operator
    '''

    def __init__(self, name: str, ty: TypeDescEntry, description: str):
        self._name = name
        self._type = ty
        self._description = description

    def __str__(self):
        return f"ArgEntry(name={self.name}, type={self.type}, description={self.description})"

    @property
    def name(self) -> str:
        ''' Getter for name of the argument. '''
        return self._name

    @property
    def type(self) -> TypeDescEntry:
        ''' Getter for type of the argument. '''
        return self._type

    @property
    def description(self) -> str:
        ''' Getter for description of the argument. '''
        return self._description


class Attr(Generic[V]):
    '''
    Attribute of an operator.
    '''

    def __init__(self, name: str, value: V, description: str):
        self._name = name
        self._value = value
        self._description = description

    def __str__(self):
        return (f"Attr(name={self._name}, "
                f"value={self._value}, "
                f"description={self._description})")

    @property
    def name(self) -> str:
        ''' Getter for name of the attribute. '''
        return self._name

    @property
    def value(self) -> V:
        ''' Getter for value of the attribute. '''
        return self._value

    @property
    def description(self) -> str:
        ''' Getter for description of the attribute. '''
        return self._description


class OpDescEntry(Generic[T]):
    '''
    Operator for DSL intermediate representation.

    Attributes:
        name: name of the operator.
        description: description of the operator
        arg_num: number of arguments
        args: argument description
        attrs: attributes of the operator
        properties: properties of the operator

    Functions:
        add_arg: add an argument to the operator

    '''

    def __init__(self, name: str):
        self._name = name
        self._description = ""
        self._arg_num = 0
        self._args = []
        self._attrs = []
        self._properties = []

        frame = inspect.currentframe()
        try:
            self.creation_location = f"{inspect.getfile(frame.f_back)}:{frame.f_back.f_lineno}"
        finally:
            del frame  # Break reference cycle

    def __str__(self):
        return (f"{self.creation_location}:"
                f"OpDescEntry(name={self.name}, "
                f"description={self._description}, "
                f"arg_num={self.arg_num}, "
                f"args={self.args}, "
                f"attrs={self.attrs}, "
                f"properties={self.properties},)")

    def str_brief(self):
        ''' Brief string of the operator.'''
        return (f"{self.creation_location}:"
                f"OpDescEntry(name={self.name})")

    @property
    def name(self) -> str:
        ''' Getter for name of the operator. '''
        return self._name

    @property
    def arg_num(self) -> int:
        ''' Getter for number of arguments of the operator. '''
        return self._arg_num

    @property
    def args(self) -> list:
        ''' Getter for arguments of the operator. '''
        return self._args

    @property
    def attrs(self) -> list:
        ''' Getter for attributes of the operator. '''
        return self._attrs

    @property
    def properties(self) -> list:
        ''' Getter for properties of the operator. '''
        return self._properties

    @arg_num.setter
    def arg_num(self, arg_num: int) -> T:
        ''' Setter for number of arguments of the operator. '''
        self._arg_num = arg_num
        return self

    def get_description(self) -> str:
        ''' Getter for description of the operator. '''
        return self._description

    def description(self, desc: str) -> T:
        ''' Setter for description of the operator. '''
        self._description = desc
        return self

    def set_arg_num(self, arg_num: int) -> T:
        ''' Setter for number of arguments of the operator. '''
        self._arg_num = arg_num
        return self

    def add_arg(self, name: str, ty: TypeDescEntry, description: str) -> T:
        ''' Add an argument to the operator. '''
        self._args.append(ArgEntry(name, ty, description))
        return self

    def add_prop(self, prop: OprProp) -> T:
        ''' Add a property to the operator. '''
        self._properties.append(prop)
        return self

    def add_attr(self, name: str, value: V, description: str) -> T:
        ''' Add an attribute to the operator. '''
        self._attrs.append(Attr(name, value, description))
        return self

    def verify(self):
        '''
        Verify the operator.
        '''
        errors = []
        if self._description == "":
            errors.append("description is empty")

        if self.arg_num != len(self.args):
            errors.append(
                f"argument number mismatch: expected {self.arg_num}, got {len(self.args)}"
            )

        if len(self.properties) == 0:
            errors.append("properties are empty")

        if errors:
            error_message = "; ".join(errors)
            raise ValueError(
                f"Operator verification failed for {self.str_brief()}: {error_message}"
            )
