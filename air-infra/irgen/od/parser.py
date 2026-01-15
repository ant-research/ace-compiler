#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Parser for OD file
'''

import yaml
from domain import DomainDescEntry  # noqa: T484
from op import OpDescEntry          # noqa: T484
from property import OprProp        # noqa: T484


class ODParser:
    ''' Parser for OD file. '''

    def __init__(self, i) -> None:
        self._domain = None
        self._opcodes = None
        self._ns = []
        with open(i, 'r', encoding='utf-8') as f:
            text = yaml.safe_load(f)
            dom = text['domain']
            assert 'id' in dom and 'name' in dom
            if DomainDescEntry.is_created():
                raise RuntimeError("Domain object has been created")

            DomainDescEntry(dom['name'], dom['id'])
            self._domain = DomainDescEntry.get_instance()

            assert 'opcodes' in text
            self._opcodes = text['opcodes']

            assert 'namespace' in text
            ns = text['namespace']
            ns_keys = ['decl', 'use']
            for key in ns_keys:
                self._ns.append(ns.get(key, "").split(",")
                                if key in ns else [])

            assert 'guard' in text
            self.guard = text['guard']

    def parse(self):
        ''' Parse OD file. '''
        for op in self._opcodes:
            assert 'name' in op
            op_desc = OpDescEntry(op['name'])
            self._domain.add_op(op_desc)

            # handle arguments
            if 'arg_num' in op:
                op_desc.set_arg_num(op['arg_num'])
            if 'args' in op:
                for arg in op['args']:
                    assert 'name' in arg and 'type' in arg
                    desc = '' if 'desc' not in arg else arg['desc']
                    op_desc.add_arg(arg['name'], arg['type'], desc)
                if op_desc.arg_num == 0:
                    op_desc.set_arg_num(len(op_desc.args))
                else:
                    assert len(op_desc.args) <= op_desc.arg_num

            # handle properties and description
            if 'prop' in op:
                for p in op['prop']:
                    op_desc.add_prop(OprProp[p])
            if 'desc' in op:
                op_desc.description(op['desc'])

    @property
    def ns(self):
        ''' Getter for namespace. '''
        return self._ns

    @property
    def ns_decl(self):
        ''' Getter for namespace declaration. '''
        return self._ns[0]

    @property
    def ns_use(self):
        ''' Getter for namespace usage. '''
        return self._ns[1]

    @property
    def domain(self):
        ''' Getter for domain. '''
        return self._domain
