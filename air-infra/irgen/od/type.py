#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
This module defines the TYPE_DESC_ENTRY class.

Classes:
    TYPE_DESC_ENTRY
    TYPE_TRAIT
'''

import ast
import inspect
import pathlib
from enum import Enum
from typing import Callable, List


class TYPE_TRAIT_KIND(Enum):
    '''
    Type trait for DSL intermediate representation.
    '''
    UNKNOWN = 0b0000
    PRIMITIVE = 0b0001
    VA_LIST = 0b0010
    POINTER = 0b0011
    ARRAY = 0b0100
    RECORD = 0b0101
    SIGNATURE = 0b0110
    SUBTYPE = 0b0111
    END = 0b1000

    def __str__(self):
        return self.name


class PRIMITIVE_TYPE(Enum):
    '''
    Primitive type in AIR
    '''
    INT_S8 = 0x0
    INT_S16 = 0x1
    INT_S32 = 0x2
    INT_S64 = 0x3
    INT_U8 = 0x4
    INT_U16 = 0x5
    INT_U32 = 0x6
    INT_U64 = 0x7
    FLOAT_32 = 0x8
    FLOAT_64 = 0x9
    FLOAT_80 = 0xa
    FLOAT_128 = 0xb
    COMPLEX_32 = 0xc
    COMPLEX_64 = 0xd
    COMPLEX_80 = 0xe
    COMPLEX_128 = 0xf
    VOID = 0x10
    BOOL = 0x11
    END = 0x12

    def __str__(self):
        return self.name


class TypeTrait:
    # pylint: disable=C0103
    '''
    Type trait for DSL intermediate representation.
    '''

    def __init__(self, kind: TYPE_TRAIT_KIND):
        self._kind = kind
        self._prim_type = None

    def __str__(self):
        return (f"TYPE_TRAIT(kind={self.kind}, "
                f"prim_type={self.prim_type})")

    @property
    def kind(self) -> TYPE_TRAIT_KIND: # pylint: disable=C0103
        ''' Getter for kind of the type. '''
        return self._kind

    @property
    def prim_type(self) -> PRIMITIVE_TYPE: # pylint: disable=C0103
        ''' Getter for primitive type. '''
        return self._prim_type

    def Set_prim_type(self, primitive: PRIMITIVE_TYPE) -> 'TYPE_TRAIT':
        ''' Set the primitive type. '''
        self._prim_type = primitive
        return self

    def Is_prim(self) -> bool:
        ''' Check if the type is primitive. '''
        return self.kind == TYPE_TRAIT_KIND.PRIMITIVE

    def Is_va_list(self) -> bool:
        ''' Check if the type is va_list. '''
        return self.kind == TYPE_TRAIT_KIND.VA_LIST

    def Is_pointer(self) -> bool:
        ''' Check if the type is pointer. '''
        return self.kind == TYPE_TRAIT_KIND.POINTER

    def Is_array(self) -> bool:
        ''' Check if the type is array. '''
        return self.kind == TYPE_TRAIT_KIND.ARRAY

    def Is_record(self) -> bool:
        ''' Check if the type is record. '''
        return self.kind == TYPE_TRAIT_KIND.RECORD

    def Is_signature(self) -> bool:
        ''' Check if the type is signature. '''
        return self.kind == TYPE_TRAIT_KIND.SIGNATURE

    def Is_subtype(self) -> bool:
        ''' Check if the type is subtype. '''
        return self.kind == TYPE_TRAIT_KIND.SUBTYPE

    def Is_unsigned_int(self) -> bool:
        ''' Check if the type is unsigned int. '''
        if not self.Is_prim():
            return False
        return self.prim_type in (PRIMITIVE_TYPE.INT_U8, PRIMITIVE_TYPE.INT_U16,
                                  PRIMITIVE_TYPE.INT_U32, PRIMITIVE_TYPE.INT_U64)

    def Is_signed_int(self) -> bool:
        ''' Check if the type is signed int. '''
        if not self.Is_prim():
            return False
        return self.prim_type in (PRIMITIVE_TYPE.INT_S8, PRIMITIVE_TYPE.INT_S16,
                                  PRIMITIVE_TYPE.INT_S32, PRIMITIVE_TYPE.INT_S64)

    def Is_int(self) -> bool:
        ''' Check if the type is int. '''
        return self.Is_signed_int() or self.Is_unsigned_int()

    def Is_float(self) -> bool:
        ''' Check if the type is float. '''
        return self.Is_real_float() or self.Is_complex_float()

    def Is_real_float(self) -> bool:
        ''' Check if the type is real float. '''
        if not self.Is_prim():
            return False
        return self.prim_type in (PRIMITIVE_TYPE.FLOAT_32,
                                  PRIMITIVE_TYPE.FLOAT_64,
                                  PRIMITIVE_TYPE.FLOAT_80,
                                  PRIMITIVE_TYPE.FLOAT_128)

    def Is_complex_float(self) -> bool:
        ''' Check if the type is complex float. '''
        if not self.Is_prim():
            return False
        return self.prim_type in (PRIMITIVE_TYPE.COMPLEX_32,
                                  PRIMITIVE_TYPE.COMPLEX_64,
                                  PRIMITIVE_TYPE.COMPLEX_80,
                                  PRIMITIVE_TYPE.COMPLEX_128)

    def Is_void(self) -> bool:
        ''' Check if the type is void. '''
        if not self.Is_prim():
            return False
        return self.prim_type == PRIMITIVE_TYPE.VOID

    def Is_scalar(self) -> bool:
        ''' Check if the type is scalar. '''
        return (self.Is_prim() or self.Is_pointer()
                or self.Is_va_list()) and (not self.Is_complex_float())

    def Is_aggregate(self) -> bool:
        ''' Check if the type is aggregate. '''
        return self.Is_array() or self.Is_record()


class TYPE_TRAIT(TypeTrait):
    '''
    TypeTrait for DSL intermediate representation.
    '''

    def __init__(self, kind: TYPE_TRAIT_KIND): # pylint: disable=W0235
        super().__init__(kind)


class ARRAY_TYPE_TRAIT(TYPE_TRAIT):
    '''
    Array type trait for DSL intermediate representation.
    '''

    def __init__(self, etype: TYPE_TRAIT, dims: list):
        super().__init__(TYPE_TRAIT_KIND.ARRAY)
        self._elem_type = etype
        self._dims = dims

    def Is_array(self) -> bool:
        return True

    def Set_dims(self, dims) -> 'TYPE_TRAIT':
        '''
        Set the dimensions of the type.
        dims can be list or tuple. [1, 2, 3] or (1, 2, 3)
        '''
        self._dims = list(dims)
        return self

    def Shape(self) -> list:
        '''
        Getter for dimensions of the type.
        Igonre ARB in AIR for now
        '''
        return self._dims

    def Elem_type(self) -> 'TYPE_TRAIT':
        ''' Getter for element type of the type. '''
        return self._elem_type

    def Set_elem_type(self, elem_type: 'TYPE_TRAIT') -> 'TYPE_TRAIT':
        ''' Set the element type of the type. '''
        self._elem_type = elem_type
        return self


class RECORD_TYPE_TRAIT(TYPE_TRAIT):
    '''
    Record type trait for DSL intermediate representation.
    '''

    def __init__(self, name: str):
        super().__init__(TYPE_TRAIT_KIND.RECORD)
        self._name = name

    @property
    def name(self) -> str: # pylint: disable=C0103
        ''' Getter for name of the type. '''
        return self._name


class POINTER_TYPE_TRAIT(TYPE_TRAIT):
    '''
    Pointer type trait for DSL intermediate representation.
    '''

    def __init__(self, etype: TYPE_TRAIT):
        super().__init__(TYPE_TRAIT_KIND.POINTER)
        self._elem_type = etype

    def Elem_type(self) -> 'TYPE_TRAIT':
        ''' Getter for element type of the type. '''
        return self._elem_type

    def Set_elem_type(self, elem_type: 'TYPE_TRAIT') -> 'TYPE_TRAIT':
        ''' Set the element type of the type. '''
        self._elem_type = elem_type
        return self


class SIGNATURE_TYPE_TRAIT(TYPE_TRAIT):
    '''
    Signature type trait for DSL intermediate representation.
    '''

    def __init__(self, ret_type: TYPE_TRAIT, arg_types: list):
        super().__init__(TYPE_TRAIT_KIND.SIGNATURE)
        self._ret_type = ret_type
        self._arg_types = arg_types

    def Ret_type(self) -> 'TYPE_TRAIT':
        ''' Getter for return type of the type. '''
        return self._ret_type

    def Arg_types(self) -> list:
        ''' Getter for argument types of the type. '''
        return self._arg_types

    def Set_arg_types(self, arg_types: list) -> 'TYPE_TRAIT':
        ''' Set the argument types of the type. '''
        self._arg_types = arg_types
        return self


class Attrs:
    # pylint: disable=C0103
    '''
    Attributes for DSL intermediate representation.
    '''

    def __init__(self, attrs: dict):
        self._attrs = attrs

    def __getitem__(self, key): # pylint: disable=C0103
        return self._attrs[key], len(self._attrs[key])

    def Get(self, key):
        '''
        Get the value of the key.
        '''
        return self._attrs.get(key, None), len(self._attrs.get(key, None))


class ATTRS(Attrs):
    # pylint: disable=R0903
    '''
    Attributes for DSL intermediate representation.
    '''

    def __init__(self, attrs: dict): # pylint: disable=W0235
        super().__init__(attrs)

class VERIFIER:
    '''
    Type verifier for DSL intermediate representation.
    '''

    def __init__(self,
                 verifier: Callable[[List[TYPE_TRAIT], ATTRS], TYPE_TRAIT],
                 name: str = ""):
        '''
        verifier takes a list of TYPE_TRAIT and a list of ATTRS as input.
        Each TYPE_TRAIT in the list is the type of the corresponding child of current NODE.
        We use assert to specify the checking rules, for example:

            def gemm_verifier(types, attrs: ATTRS) -> TYPE_TRAIT:
                t1 = types[0]
                t2 = types[1]

                assert t1.Is_array() and t2.Is_array()
                assert t1.elem_type() == t2.elem_type()

                shape1 = t1.Shape()
                shape2 = t2.Shape()
                assert len(shape1) == 2 and len(shape2) == 2
                assert shape1[1] == shape2[0]
                ...
                return ARRAY_TYPE_TRAIT(t1.elem_type(), [shape1[0], shape2[1]])
        '''
        if name == "":
            assert hasattr(
                verifier, '__name__'
            ), "VERIFIER must have a name in form of 'opname_verifier'"
        self._verifier = verifier
        self._emitter = VERIFIER_EMITTER(self)
        self._namespaces = []
        self._outdir = ""

    @property
    def verifier(self) -> Callable[[List[TYPE_TRAIT], ATTRS], TYPE_TRAIT]: # pylint: disable=C0103
        ''' Getter for verifier. '''
        return self._verifier

    @property
    def namespaces(self) -> List[str]: # pylint: disable=C0103
        ''' Getter for namespaces. '''
        return self._namespaces

    @property
    def outdir(self) -> str: # pylint: disable=C0103
        ''' Getter for output directory. '''
        return self._outdir

    def Set_outdir(self, outdir: str) -> 'VERIFIER':
        ''' Setter for output directory. '''
        self._outdir = outdir
        return self

    def Set_namespaces(self, namespaces: List[str]) -> 'VERIFIER':
        ''' Setter for namespaces. '''
        self._namespaces = namespaces
        return self

    def Gen_verifier(self, output: str):
        ''' Generate verifier code. '''
        src = inspect.getsource(self._verifier)
        tree = ast.parse(src)
        head, node = self._emitter.Emit(tree)

        # include the namespaces
        prefix = ""
        suffix = ""
        for ns in self._namespaces:
            prefix += f"namespace {ns} " + "{\n"
            suffix += "} " + f"// namespace {ns} \n"
        node = prefix + node + suffix

        # include the function definition
        node = f"#include \"{output}.h\"\n" + node

        head = head + prefix + "\n".join(self._emitter.Get_functions())
        head = head + "\n" + suffix

        # set guard for header file
        head = f"#ifndef {output.upper()}_H\n#define {output.upper()}_H\n\n" + head
        head = head + "\n#endif\n"

        # create directory if not exist
        pathlib.Path(self._outdir).mkdir(parents=True, exist_ok=True)
        cxx = pathlib.Path(self._outdir) / f"{output}.cxx"
        header = pathlib.Path(self._outdir) / f"{output}.h"
        with open(cxx, "w", encoding='utf-8') as f:
            f.write(node)
        with open(header, "w", encoding='utf-8') as f:
            f.write(head)


class VERIFIER_EMITTER(ast.NodeVisitor):
    '''
    Type verifier emitter will convert the verifier into C++ code.
    '''

    def __init__(self, verifier: VERIFIER):
        self._verifier = verifier
        self._arg_names = []
        self._depth = 0
        self._indent = "  "
        self._code_stack = []
        self._tmp_id = 0
        self._functions = []

    def Get_functions(self) -> List[str]:
        '''
        Get the functions.
        '''
        return self._functions

    def Get_tmp_id(self) -> int:
        '''
        Get the temporary id.
        '''
        self._tmp_id += 1
        return str(self._tmp_id)

    def Get_indent(self) -> str:
        '''
        Get the indent string.
        '''
        return self._indent * self._depth

    def Emit(self, node: ast.AST):
        '''
        Emit the verifier code.
        '''
        head = Set_includes() + Set_namespace()
        node = self.visit(node)
        return head, node

    def visit_Name(self, node: ast.Name) -> str: # pylint: disable=C0103,R0201
        '''
        Handle the name node.
        '''
        return node.id

    def visit_Slice(self, node: ast.Slice) -> str: # pylint: disable=C0103,R0201,W0613
        '''
        Handle the slice node.
        '''
        assert False, "Slice is not supported"
        return ""

    def visit_Tuple(self, node: ast.Tuple) -> str: # pylint: disable=C0103,R0201,W0613
        '''
        Handle the tuple node.
        '''
        assert False, "Tuple is not supported"
        return ""

    def visit_Constant(self, node: ast.Constant): # pylint: disable=C0103
        '''
        Handle the constant node.
        '''
        if isinstance(node.value, int):
            return node.value
        if isinstance(node.value, str):
            return f"\"{node.value}\""
        if isinstance(node.value, float):
            return node.value
        if node.value is None:
            return "nullptr"
        print(f"Constant: {node.value}")
        assert False, "Constant type is not supported"
        return None

    # python version < 3.9
    def visit_Index(self, node: ast.Index) -> str: # pylint: disable=C0103
        '''
        Handle the index node.
        '''
        return self.visit(node.value)

    def visit_Subscript(self, node: ast.Subscript) -> str: # pylint: disable=C0103
        '''
        Handle the subscript node.
        '''
        value = self.visit(node.value)
        s = self.visit(node.slice)
        return f"{value}[{s}]"

    def visit_List(self, node: ast.List) -> str: # pylint: disable=C0103
        '''
        Handle the list node.
        '''
        elems = []
        for elt in node.elts:
            elems.append(self.visit(elt))
        elem_types = set(type(elt) for elt in elems)
        assert len(elem_types
                   ) == 1, "List with different element types is not supported"
        res = f"std::vector"
        res += "{" + ", ".join(map(str, elems)) + "}"
        return res

    def visit_BinOp(self, node: ast.BinOp) -> str: # pylint: disable=C0103
        '''
        Handle the binary operation node.
        '''
        cpp_op_precedence = {
            ast.Mult: 5,
            ast.Add: 4,
            ast.Sub: 4,
            ast.Div: 5,
            ast.FloorDiv: 5,
            ast.Mod: 5
        }
        left = f"{self.visit(node.left)}"
        right = f"{self.visit(node.right)}"
        op = Operator_to_string(node.op)

        def Need_paren(n, pred):
            is_n_bin = isinstance(n, ast.BinOp)
            is_n_call = isinstance(n, ast.Call)
            is_n_subscript = isinstance(n, (ast.Subscript, ast.Slice))
            is_n_attribute = isinstance(n, ast.Attribute)
            is_n_constant = isinstance(n, ast.Constant)
            is_n_var = isinstance(n, ast.Name)
            if is_n_bin:
                return cpp_op_precedence[n.op.__class__] < pred
            if is_n_call or is_n_subscript or is_n_attribute or is_n_constant or is_n_var:
                return False
            return True

        left_res = left
        right_res = right
        pred = cpp_op_precedence[node.op.__class__]
        if Need_paren(node.left, pred):
            left_res = f"({left})"
        if Need_paren(node.right, pred):
            right_res = f"({right})"

        return f"{left_res} {op} {right_res}"

    def visit_Assign(self, node: ast.Assign) -> str: # pylint: disable=C0103
        '''
        Handle the assignment node.
        '''
        assert len(
            node.targets) == 1, "Only one target of `Assign` is supported"
        assert isinstance(
            node.targets[0],
            ast.Name), "Only `Name` target of `Assign` is supported"

        stack_len = len(self._code_stack)

        lhs = self.visit(node.targets[0])
        rhs = self.visit(node.value)

        res = f"auto {lhs} = {rhs};\n"
        if len(self._code_stack) > stack_len:
            res = self._code_stack.pop() + res

        return res

    def visit_Call(self, node: ast.Call) -> str: # pylint: disable=C0103
        '''
        Handle the call node.
        '''
        func = self.visit(node.func)

        type_kind = Creating_type_trait(func)
        if type_kind == TYPE_TRAIT_KIND.ARRAY:
            if len(node.args) != 2:
                raise ValueError(
                    "ARRAY_TYPE_TRAIT should have two arguments: element type and dimensions"
                )
            etype = self.visit(node.args[0])
            dims = self.visit(node.args[1])
            gs = "GLOB_SCOPE &gs = node->Glob_scope();\n"
            # REFINE: hard code name of array type
            t = "tmp" + self.Get_tmp_id()
            arr = self.Get_indent() + f"ARRAY_TYPE_PTR {t}"
            arr = f"{arr} = gs.New_arr_type(\"hard_code\", {etype}, {dims}, node->Spos());\n"
            self._code_stack.append(gs + arr)
            return t

        # REFINE: dirty hack for attr
        if func.startswith("node->Attr"):
            args = [self.visit(arg) for arg in node.args]
            assert len(args) == 2, "`Attr` method must have two arguments"
            args[1] = "(uint32_t *)(&" + args[1] + ")"
            args = ", ".join(args)
        else:
            args = ", ".join(self.visit(arg) for arg in node.args)
        if func == "len":
            return f"{args}.size()"
        return f"{func}({args})"

    def visit_Attribute(self, node: ast.Attribute) -> str: # pylint: disable=C0103
        '''
        Handle the attribute node.
        '''
        value = self.visit(node.value)
        attr = node.attr.capitalize()

        # handle the case of `ATTRS`
        # REFINE: type of `Attr` is hard coded to `int`
        if value == self._arg_names[1]:
            assert attr == "Get", "Only `get` method of `ATTRS` is supported"
            return "node->Attr<int>"
            # print(f"Attribute: {value}.{attr}")

        # handle the method of `ARRAY_TYPE_TRAIT`
        if Is_method_of(node.attr, ARRAY_TYPE_TRAIT):
            return f"{value}->Cast_to_arr()->{attr}"

        return f"{value}->{attr}"

    def visit_If(self, node: ast.If) -> str: # pylint: disable=C0103
        '''
        Handle the if node.
        '''
        res = ""
        cond = self.visit(node.test)
        res += f"if ({cond}) " + "{\n"

        self._depth += 1
        # handle body of if
        for stmt in node.body:
            r = self.visit(stmt)
            if r:
                res += self.Get_indent() + r
        self._depth -= 1

        # handle orelse
        if node.orelse:
            res += self.Get_indent() + "} else " + "{\n"
            self._depth += 1
            for stmt in node.orelse:
                r = self.visit(stmt)
                if r:
                    res += self.Get_indent() + r
            self._depth -= 1
        res += self.Get_indent() + "}\n\n"
        return res

    def visit_BoolOp(self, node: ast.BoolOp) -> str: # pylint: disable=C0103
        '''
        Handle the bool operation node.
        '''
        op = Operator_to_string(node.op)
        values = [self.visit(val) for val in node.values]
        return "(" + f" {op} ".join(values) + ")"

    def visit_Compare(self, node: ast.Compare) -> str: # pylint: disable=C0103
        '''
        Handle the compare node.
        '''
        left = self.visit(node.left)
        assert len(node.ops) == 1, "Only one compare operator is supported"
        op = Operator_to_string(node.ops[0])
        rhs = self.visit(node.comparators[0])
        return f"({left} {op} {rhs})"

    def visit_Assert(self, node: ast.Assert) -> str: # pylint: disable=C0103
        '''
        Handle the assert node.
        '''
        res = ""
        test = self.visit(node.test)
        msg = str(self.visit(node.msg)).strip('\"')
        res += f"if (!({test})) " + "{\n"
        self._depth += 1
        res += self.Get_indent()
        res += f"std::cerr << \"Assertion failed: {msg}\" << std::endl;\n"
        res += self.Get_indent() + "return NULL_PTR();\n"
        self._depth -= 1
        res += self.Get_indent() + "}\n\n"
        return res

    def visit_Return(self, node: ast.Return) -> str: # pylint: disable=C0103
        '''
        Handle the return node.
        '''
        stack_len = len(self._code_stack)
        res = self.Get_indent() + f"return {self.visit(node.value)};\n"
        if len(self._code_stack) > stack_len:
            res = self._code_stack.pop() + res
        return res

    def visit_FunctionDef(self, node): # pylint: disable=C0103
        '''
        Handle the function definition node.
        '''
        self._depth += 1

        assert len(
            node.args.args) == 2, "VERIFIER must take one argument: types"
        self._arg_names = [node.args.args[0].arg, node.args.args[1].arg]

        res = "TYPE_PTR " + node.name.capitalize()
        res += f" (NODE_PTR node, const std::vector<TYPE_PTR>& {self._arg_names[0]}) "

        # add function to the list
        self._functions.append(res + ";")

        res += "{\n"

        for stmt in node.body:
            r = self.visit(stmt)
            if r:
                res += self.Get_indent() + r
        self._depth -= 1
        res += "}\n"
        return res

    def visit_Module(self, node): # pylint: disable=C0103
        '''
        Handle the module node.
        '''
        res = ""
        for stmt in node.body:
            res += self.visit(stmt)
        return res


class TypeDescEntry: # pylint: disable=C0103
    '''
    Type for DSL intermediate representation.
    '''

    def __init__(self, name: str, trait: TYPE_TRAIT_KIND):
        self._name = name
        self._trait = trait

    def __str__(self):
        return f"TYPE_DESC_ENTRY(name={self.name}, trait={self.trait})"

    @property
    def name(self) -> str: # pylint: disable=C0103
        ''' Getter for name of the type. '''
        return self._name

    @property
    def trait(self) -> TYPE_TRAIT: # pylint: disable=C0103
        ''' Getter for trait of the type. '''
        return self._trait


class TYPE_DESC_ENTRY(TypeDescEntry):
    '''
    TypeDescEntry for DSL intermediate representation.
    '''

    def __init__(self, name: str, trait: TYPE_TRAIT):
        super().__init__(name, trait)


def Is_method_of(name: str, cls) -> bool:
    '''
    Check if the name is a method of the class.
    '''
    print(name, cls)
    all_attributes = dir(cls)
    methods = [
        attr for attr in all_attributes
        if callable(getattr(cls, attr)) and not attr.startswith("__")
    ]
    return name in methods

def Creating_type_trait(func: str) -> TYPE_TRAIT_KIND:
    '''
    Check if the call is creating a TYPE_TRAIT object.
    '''
    type_map = {
        "ARRAY_TYPE_TRAIT": TYPE_TRAIT_KIND.ARRAY,
        "RECORD_TYPE_TRAIT": TYPE_TRAIT_KIND.RECORD,
        "TYPE_TRAIT": TYPE_TRAIT_KIND.PRIMITIVE,
        "POINTER_TYPE_TRAIT": TYPE_TRAIT_KIND.POINTER,
        "SIGNATURE_TYPE_TRAIT": TYPE_TRAIT_KIND.SIGNATURE,
        "SubTYPE_TRAIT": TYPE_TRAIT_KIND.SUBTYPE
    }
    return None if func not in type_map else type_map[func]

def Set_includes():
    '''
    Set the includes.
    '''
    includes = ["air/base/st.h", "air/base/node.h"]
    return "\n".join([f"#include <{inc}>" for inc in includes]) + "\n\n"

def Set_namespace():
    '''
    Set the namespace.
    '''
    namespaces = ["std", "air::base"]
    return "\n".join([f"using namespace {ns};"
                      for ns in namespaces]) + "\n\n"

def Operator_to_string( op):
    '''
    convert operator to string
    '''
    op_not_supported = {ast.Is, ast.In, ast.NotIn}
    assert type( # pylint: disable=C0123
        op
    ) not in op_not_supported, f"Operator {type(op)} is not supported"

    op_map = {
        ast.Eq: "==",
        ast.NotEq: "!=",
        ast.Lt: "<",
        ast.LtE: "<=",
        ast.Gt: ">",
        ast.GtE: ">=",
        ast.Is: "is",
        ast.IsNot: "!=",
        ast.Add: "+",
        ast.Sub: "-",
        ast.Mult: "*",
        ast.Div: "/",
        ast.Mod: "%",
        ast.Pow: "**",
        ast.LShift: "<<",
        ast.RShift: ">>",
        ast.BitOr: "|",
        ast.BitXor: "^",
        ast.BitAnd: "&",
        ast.FloorDiv: "/",
        ast.Or: "||",
        ast.And: "&&"
    }
    return op_map[type(op)]
