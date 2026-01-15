#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
Unit test for code generator of opcode
'''

import unittest
from od import CREATE_DOMAIN, REGISTER_OP
from od.domain import DomainDescEntry
from od.property import OprProp
from od.cg.opcode import OpcodeHeaderConfig, OpcodeHeaderFileCG, OpcodeCXXConfig, OpcodeCXXFileCG


class TestOpcodeCG(unittest.TestCase):
    ''' Test case for opcode code generator '''

    @classmethod
    def setUpClass(cls):
        cls.domain = CREATE_DOMAIN('NN', 1)
        cls().assertEqual(cls.domain, DomainDescEntry.get_instance())

    def test_generate(self):
        '''Test the generate method of the OpcodeHeaderCG class.'''
        add = (REGISTER_OP('add')
              .description('Performs element-wise addition')
              .set_arg_num(2)
              .add_arg('a', int, 'lhs operand')
              .add_arg('b', int, 'rhs operand')
              .add_prop(OprProp.EXPR)
              .add_prop(OprProp.ATTR))
        self.assertIn(add, self.domain.ops)

        average_pool = (REGISTER_OP('average_pool')
                   .description('aver operator')
                   .set_arg_num(1)
                   .add_arg('X', 'Tensor', 'input tensor')
                   .add_prop(OprProp.EXPR)
                   .add_prop(OprProp.ATTR)
                   )
        self.assertIn(average_pool, self.domain.ops)

        header_config = (OpcodeHeaderConfig()
              .add_namespace('nn')
              .add_namespace('core')
              .set_macro_guard('NN_CORE_OPCODE_H'))
        header_cg = OpcodeHeaderFileCG(header_config, self.domain)
        header_cg.generate()

        cxx_config = (OpcodeCXXConfig()
              .add_namespace('nn')
              .add_namespace('core')
              .set_opcode_header('nn/core/opcode.h'))
        cxx_cg = OpcodeCXXFileCG(cxx_config, self.domain)
        cxx_cg.generate()


if __name__ == '__main__':
    unittest.main()
