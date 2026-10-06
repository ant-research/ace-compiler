#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Parser for target info.
'''

from pathlib import Path
from typing import List, Tuple
from yaml import safe_load
from .reg import (RegCls, RegMeta, RegConv,          # noqa: T484
                  ABIMeta, OPFlags, to_flags)        # noqa: T484
from .target_info import TargetInfo                  # noqa: T484
from .inst import InstMeta, OpndMeta                 # noqa: T484


class TDParser:
    ''' Parser for target info. '''

    def __init__(self, inputs, rpath=None) -> None:
        self.contents: dict = {}
        for i in inputs:
            if rpath:
                i = Path(rpath) / i
            else:
                i = Path(i)
            if not i.exists():
                raise FileNotFoundError(f"{i} not found")
            with open(i, 'r', encoding='utf-8') as f:
                text = safe_load(f)
                label = text['label']
                assert label not in self.contents
                self.contents[label] = {'rpath': i, 'text': text}

    def parse_in(self, include: list) -> List[str]:
        ''' Parse include. '''
        res = []
        for v in include:
            res.append(v)
        return res

    def parse_guard(self, guard: str) -> str:
        ''' Parse guard. '''
        return guard

    def parse_headers(self, headers: dict) -> str:
        ''' Parse header files. '''
        res = ""
        header_types = {
            'external': lambda i: f"#include <{i}>\n",
            'internal': lambda i: f"#include \"{i}\"\n"
        }
        for header_type, formatter in header_types.items():
            if header_type in headers:
                res += ''.join(formatter(i)
                               for i in headers[header_type].split(","))
        return res

    def parse_namespace(self, ns: dict) -> Tuple[List[str], List[str]]:
        ''' Parse namespace. '''
        decl = []
        use = []
        if 'decl' in ns:
            decl = ns['decl'].split(",")
        if 'use' in ns:
            use = ns['use'].split(",")
        return decl, use

    def parse_global_vars(self, glb_vars: dict) -> List[str]:
        ''' Parse global variables. '''
        # v is a dict in form of {'prop': ['static', 'const], 'type': 'int', 'val': '0'}
        # generate corresponding c++ code
        res = []
        for k, v in glb_vars.items():
            tmp = ""
            if 'prop' in v:
                tmp += " ".join(v['prop'])
            assert 'type' in v
            tmp += " " + v['type'] + " " + k
            if 'val' in v:
                tmp += " = " + str(v['val'])
            res.append(tmp + ";\n")
        return res

    def parse_reg_meta(self, reg_meta: dict, code: str) -> RegMeta:
        ''' Parse register meta. '''
        assert 'name' in reg_meta
        assert 'size' in reg_meta
        assert 'flag' in reg_meta
        name = reg_meta['name']
        size = reg_meta['size']
        flag = to_flags(reg_meta['flag'])
        reg = RegMeta(name, code, size).set_flags(flag)
        return reg

    def parse_reg_cls(self, reg_cls: dict) -> List[RegCls]:
        ''' Parse register class. '''
        res = []
        for k, v in reg_cls.items():
            assert 'id' in v
            cls = RegCls(k, v['id'])
            if 'regs' in v:
                assert 'size' in v
                cls.set_size(v['size'])
                regs = v['regs']
                for i, reg in enumerate(regs):
                    cls.add_reg(self.parse_reg_meta(reg, str(i)))
            res.append(cls)
        return res

    def parse_reg_conv(self, reg_conv: dict) -> RegConv:
        ''' Parse register convention. '''
        assert 'name' in reg_conv and 'cls' in reg_conv
        conv = RegConv(reg_conv['name'], reg_conv['cls'])

        assert 'param_retv' in reg_conv
        param_retv = reg_conv['param_retv']

        # set up calling convention
        (conv.set_zero(reg_conv['reg_zero'])
             .set_param(param_retv['param'])
             .set_ret(param_retv['retv']))
        return conv

    def parse_abi(self, abi: dict) -> ABIMeta:
        ''' Parse ABI. '''
        assert 'name' in abi and 'id' in abi
        res = ABIMeta(abi['name'], abi['id'])
        (res.set_gpr_cls(abi['gpr_cls'])
            .set_fp(abi['fp'])
            .set_sp(abi['sp'])
            .set_gp(abi['gp'])
            .set_tp(abi['tp'])
            .set_ra(abi['ra']))

        for conv in abi['convs']:
            res.add_conv(self.parse_reg_conv(conv))

        return res

    def parse_op_flags(self, op_flags: list) -> List[OPFlags]:
        ''' Parse op flags. '''
        res = []
        for i in op_flags:
            res.append(OPFlags(i['name'], i['id'], i['desc']))
        return res

    def parse_inst(self, insts: list) -> List[InstMeta]:
        ''' Parse instruction meta. '''
        res = []
        for i in insts:
            meta = InstMeta(i['name'], i['code'], i['res_cnt'], i['opnd_cnt'], i['flag'])
            for k in i['res_opnd']:
                opnd = OpndMeta(k['op_kind'], k['reg_cls'],
                                k['reg_num'], k['size_mask'])
                meta.add_opnd(opnd)
            res.append(meta)
        return res

    def parse_target(self, target: dict) -> Tuple[str, str]:
        ''' Parse target. '''
        assert 'name' in target and 'isa_id' in target
        return target['name'], target['isa_id']

    def parse(self) -> List[TargetInfo]:
        ''' Parse target info. '''
        res = []

        for _, content in self.contents.items():
            rpath = content['rpath']
            print(rpath)
            v = content['text']

            # capture all macros in a list
            macros = set(traverse(v, lambda x: x[1:-1] if isinstance(x, str) and
                                  x.startswith('<') and x.endswith('>') else None))

            # process macros
            update_rec(v, lambda x: x[1:-1] if isinstance(x, str)
                       and x.startswith('<') and x.endswith('>') else x)

            target = TargetInfo("None", "None")
            if 'target' in v:
                name, isa_id = self.parse_target(v['target'])
                target = TargetInfo(name, isa_id)
            target.set_used_macros(list(macros))

            if 'in' in v:
                inc = self.parse_in(v['in'])
                includes = TDParser(inc, rpath.parent).parse()
                target.set_includes(includes)

            if 'guard' in v:
                guard = self.parse_guard(v['guard'])
                target.set_guard(guard)

            if 'header_files' in v:
                headers = self.parse_headers(v['header_files'])
                target.set_headers(headers)

            if 'namespace' in v:
                d, u = self.parse_namespace(v['namespace'])
                target.set_namespace(d, u)

            if 'global_vars' in v:
                var = self.parse_global_vars(v['global_vars'])
                target.set_global_vars(var)

            if 'reg_cls' in v:
                cls = self.parse_reg_cls(v['reg_cls'])
                for c in cls:
                    target.add_reg_cls(c)

            if 'abi' in v:
                conv = self.parse_abi(v['abi'])
                target.set_abi(conv)

            if 'op_flags' in v:
                op_flags = self.parse_op_flags(v['op_flags'])
                target.set_op_flags(op_flags)

            if 'inst_meta' in v:
                inst_meta = self.parse_inst(v['inst_meta'])
                target.set_inst_meta(inst_meta)

            res.append(target)
        return res


def update_rec(data, func):
    ''' Update container recursively. '''
    if isinstance(data, dict):
        for key, value in data.items():
            data[key] = update_rec(value, func)
    elif isinstance(data, list):
        data = [update_rec(item, func) for item in data]
    else:
        data = func(data)
    return data


def traverse(data, func):
    ''' Traverse container recursively. '''
    if not isinstance(data, (dict, list)):
        # If data is not a collection, apply the func directly
        result = func(data)
        return [result] if result is not None else []

    results = []

    if isinstance(data, dict):
        for value in data.values():
            results.extend(traverse(value, func))
    elif isinstance(data, list):
        for item in data:
            results.extend(traverse(item, func))

    return results
