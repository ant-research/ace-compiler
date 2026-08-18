#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
Provides functionality to register operators/domains in a DSL's intermediate representation.

Functions:
    CREATE_DOMAIN: Create a Domain instance.
    REGISTER_OP: Register an operator.
'''

from .operator import OpDescEntry
from .domain import DomainDescEntry
from .type import TypeDescEntry, TypeTrait


def CREATE_DOMAIN(name: str, domain_id: int) -> DomainDescEntry:
    '''
    Create a Domain object.

    Args:
        name: name of the domain.
        domain_id: id of the domain.

    Returns:
        Domain object.
    '''
    if DomainDescEntry.is_created():
        raise RuntimeError("Domain object has been created")

    DomainDescEntry(name, domain_id)
    return DomainDescEntry.get_instance()


def REGISTER_OP(name: str) -> OpDescEntry:
    '''
    Register an operator.

    Args:
        name: name of the operator.

    Returns:
        Operator object.
    '''
    op_desc = OpDescEntry(name)
    DomainDescEntry.get_instance().add_op(op_desc)
    return op_desc


def REGISTER_TY(type_name: str, trait: TypeTrait) -> TypeDescEntry:
    '''
    Register a type.

    Args:
        type_name: name of the type.
        trait: trait of the type.

    Returns:
        Type object.
    '''
    ty_desc = TypeDescEntry(type_name, trait)
    DomainDescEntry.get_instance().add_ty(ty_desc)
    return ty_desc
