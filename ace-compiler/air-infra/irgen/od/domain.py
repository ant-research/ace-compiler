#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
This module defines the DomainDescEntry class.

Class:
    SingletonMeta
    DomainDescEntry
'''

from od.op import OpDescEntry  # noqa: T484


class SingletonMeta(type):
    '''
    A metaclass that creates a Singleton instance.
    '''
    _instances: dict = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            instance = super(SingletonMeta, cls).__call__(*args, **kwargs)
            cls._instances[cls] = instance
        return cls._instances[cls]

    def get_instance(cls, *args, **kwargs):
        '''
        Get the singleton instance of the class.

        Args:
            *args: Positional arguments for the class constructor.
            **kwargs: Keyword arguments for the class constructor.

        Returns:
            The singleton instance of the class.
        '''
        return cls(*args, **kwargs)

    def clear_instance(cls):
        '''
        Clear the singleton instance of the class.

        Args:
            cls: The class to clear the singleton instance.
        '''
        if cls in cls._instances:
            del cls._instances[cls]

    def is_created(cls):
        '''
        Check if the singleton instance has been created.

        Returns:
            bool: True if the instance has been created, False otherwise.
        '''
        return cls in cls._instances


class DomainDescEntry(metaclass=SingletonMeta):
    '''
    Domain for DSL intermediate representation.

    Attributes:
        name: name of the domain.
        id: id of the domain.
    '''

    def __init__(self, name: str, domain_id: int):
        self._name = name
        self._domain_id = domain_id
        self._ops: list = []
        self._types: list = []

    def __str__(self):
        ops_str = ", ".join(op.str_brief() for op in self._ops)
        return (
            f"DomainDescEntry(name={self._name}, domain_id={self._domain_id}, "
            f"ops=[{ops_str}], types={self._types})")

    @property
    def name(self) -> str:
        ''' Getter for name of the domain. '''
        return self._name

    @property
    def domain_id(self) -> int:
        ''' Getter for id of the domain. '''
        return self._domain_id

    @property
    def ops(self) -> list:
        ''' Getter for operators of the domain. '''
        return self._ops

    @property
    def types(self) -> list:
        ''' Getter for types of the domain. '''
        return self._types

    def add_op(self, op: OpDescEntry):
        '''
        Add an operator to the domain.
        '''
        self._ops.append(op)

    def add_ty(self, ty):
        '''
        Add a type to the domain.
        '''
        self._types.append(ty)

    def verify(self):
        '''
        Verify the domain. Accumulate and raise all exceptions from op.verify().
        '''
        exceptions = []
        for op in self._ops:
            try:
                op.verify()
            except ValueError as e:
                exceptions.append(f"Error in {op}: {e}")
            except TypeError as e:
                exceptions.append(f"Error in {op}: {e}")

        if exceptions:
            error_message = "\n".join(exceptions)
            raise ValueError(
                f"Domain verification failed with the following errors:\n{error_message}"
            )

    def __del__(self):
        ''' 
        Clear the singleton instance.
        '''
        SingletonMeta.clear_instance(self.__class__)

    def clear_instance(self):
        '''
        Clear the singleton instance.
        '''
        SingletonMeta.clear_instance(self.__class__)
