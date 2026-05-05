#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
Unit tests for the DomainDescEntry class.
'''

import unittest
from od import CREATE_DOMAIN, REGISTER_OP
from od.domain import DomainDescEntry
from od.property import OprProp


class TestDomainDescEntry(unittest.TestCase):
    '''Unit tests for the DomainDescEntry class.'''

    @classmethod
    def setUpClass(cls) -> None:
        cls.domain = CREATE_DOMAIN('NN', 1)
        cls().assertEqual(cls.domain, DomainDescEntry.get_instance())

    def test_initialization(self):
        '''Test the initialization of the DomainDescEntry class.'''
        self.assertEqual(self.domain.name, 'NN')
        self.assertEqual(self.domain.domain_id, 1)

    def test_add_op(self):
        '''Test the add_op method of the DomainDescEntry class.'''
        invalid = (REGISTER_OP('invalid')
                   .description('Invalid operator')
                   .set_arg_num(0)
                   .add_prop(OprProp.NONE)
                   )
        self.assertIn(invalid, self.domain.ops)

        add = (REGISTER_OP('add')
              .description('Performs element-wise addition')
              .set_arg_num(2)
              .add_arg('a', int, 'lhs operand')
              .add_arg('b', int, 'rhs operand')
              .add_prop(OprProp.EXPR)
              .add_prop(OprProp.ATTR))
        self.assertIn(add, self.domain.ops)

    def test_verify(self):
        '''Test the verify method of the DomainDescEntry class.'''
        self.domain.verify()

    @classmethod
    def tearDownClass(cls):
        cls.domain = None
        DomainDescEntry.get_instance().clear_instance()


if __name__ == '__main__':
    unittest.main()
