#!/usr/bin/env python3 # noqa T499
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
"""
Defines the Python Macro call.
"""
import ast
import inspect
import typing
from abc import ABCMeta, abstractmethod
from collections.abc import Callable
from enum import Enum, auto
from typing import Any
from pydsl.protocols import ToAIRBase  # noqa T484


class Macro(ABCMeta):
    """
    A metaclass whose instances are evaluated during compile time and
    manipulates the AIR output.

    For all instances of Macro, and for all class methods annotated as
    @abstractmethod within the instance, they must be implemented by their
    subclasses.
    """

    def __new__(cls, name, bases, attr, *args, **varargs):  # noqa B902
        new_cls = super().__new__(cls, name, bases, attr, *args, **varargs)

        for base in bases:
            if not hasattr(base, "__abstractmethods__"):
                continue

            for abstract_method in base.__abstractmethods__:
                # check if the abstractmethod is overwritten
                if getattr(new_cls, abstract_method) is getattr(base, abstract_method):
                    raise TypeError(
                        f"macro {new_cls.__name__} did not implement abstract "
                        f"method {abstract_method}, required by base {base}"
                    )

        return new_cls


class ArgRep(Enum):
    """
    Metaclass for argument
    """
    EVALUATED = auto()
    """
    This indicates that the argument is a literal or a variable name that is
    evaluated as a compile-time Python value.

    Please note: once the Python runtime is opened, all CallMacros nested
    inside it will demand all arguments in compile-time Python representation.
    pydsl may no longer be used.
    For instance, if you nest a CallMacro `f` inside a PYTHON ArgType
    argument, and `f` demands a TREE as its first argument,
    then you must pass in an ast.AST Python object, not writing the
    representation of such tree as-is.

    As an example, writing f(4) in pydsl is equivalent to writing
    f(Expr(value=Constant(value=4))) in compile-time Python.
    """

    COMPILED = auto()
    """
    This indicates that the argument should be passed in as expressed in
    target language, i.e. AIR OpViews.
    This accepts any expression that can be outputted by this compiler.
    """

    UNCOMPILED = auto()
    """
    This indicates that the argument should remain as a Python AST.
    """


def iscallmacro(f: Any) -> bool:
    """
    Return true if f is call macro.
    """
    return issubclass(type(f), type) and issubclass(f, CallMacro)


class CallMacro(metaclass=Macro):
    """
    A macro that intends to be called as-is without being at the top level of
    a special code body, such as being the iterator of a `for` statement.

    The macro is responsible for transforming the Call node that called this
    function and its constituents.
    Note that if the context of the Call matches those of other Macro classes,
    they may be prioritized as the intended superclass and error may be thrown.

    This function may be called during compile-time, in which case the function
    is expected to return the AIR that satisfies its behavior.
    The caller is responsible for providing the argument that the macro
    expects, such as Python objects, ASTs, or AIR operations.
    Outside of any source that's to be compiled, the function always expects
    the AIR visitor as the first argument.
    If visited within a source that's to be compiled, the arguments will
    automatically be prepended with the ToAIRBase visitor.
    """

    is_member = False

    @abstractmethod
    def argreps():  # noqa T484
        """
        This function must be overwritten to specify the type of the
        non-variable arguments that the function accepts.
        """

    def argsreps():  # noqa T484
        """
        This function can be overwritten to specify the type of the variable
        argument.
        """

        return None

    @abstractmethod
    def _on_Call(  # noqa N802
        self,
        cls,
        visitor: ast.NodeVisitor,
        args,
    ) -> Any: ...

    @classmethod
    def cast_arg(cls, visitor: ToAIRBase, arg, argtype):
        """
        Helper function for casting a single argument
        """
        if argtype == ArgRep.COMPILED and isinstance(arg, ast.AST):
            return visitor.visit(arg)
        if argtype == ArgRep.UNCOMPILED:
            return arg
        raise TypeError(
            f"argtype() in {cls.__name__}, must return List[ArgType], "
            f"got {argtype} as an element in the returned list"
        )

    @classmethod
    def _parse_args(cls, visitor: ToAIRBase, node: ast.Call, prefix_args,):
        args = prefix_args + node.args

        if cls.argsreps() is None and len(cls.argreps()) != len(args):
            raise ValueError(
                f"{cls.__name__} expected {len(cls.argreps())} arguments, got "
                f"{len(args)}"
            )

        ret = []

        # evaluate regular args
        if len(args) != len(cls.argreps()):
            raise ValueError("zip() arguments must have the same length")
        for arg, argtype in zip(args, cls.argreps()):
            ret.append(cls.cast_arg(visitor, arg, argtype))

        # evaluate varargs
        if (varargtype := cls.argsreps()) is not None:
            for arg in args[len(cls.argreps()):]:
                ret.append(cls.cast_arg(visitor, arg, varargtype))

        return ret

    def on_Call(attr_chain, visitor: ToAIRBase, node: ast.Call, prefix_args):  # noqa N802
        """
        Abstract method for on_Call
        """
        cls = attr_chain[-1]  # noqa T484

        if cls.is_member:
            assert (
                len(attr_chain) >= 2  # noqa T484
            ), f"Attribute chain does not contain the class of {cls}"
            return cls._on_Call(
                attr_chain[-2],  # noqa T484
                visitor,
                cls._parse_args(visitor, node, prefix_args=prefix_args),
            )

        return cls._on_Call(
            visitor, cls._parse_args(visitor, node, prefix_args=prefix_args)
        )

    def generate(is_member=False):  # noqa T484
        """
        Decorator that converts a function into a CallMacro.

        This decorator enforce strict type hinting in the function arguments,
        as such:
        - The first argument must be ToAIRBase (although this is not
          enforced at runtime)
        - The remaining arguments must be type-hinted Annotation[a, b], where a
          can be any type, and b must be an ArgRep.
            - For your convenience, you can use Evaluated[T], Compiled, and
              Uncompiled for your remaining arguments.

        WARNING: this decorator does not work with string type hints that are
        not imported at runtime, even if it's done with `if TYPE_CHECKING`. The
        runtime semantics of the type hint are examined to inform its behavior.

        If the macro is to be a @classmethod of a class, pass is_member=True.
        The class will be passed as the first argument and it won't need to be
        type-hinted.
        """

        def generate_sub(f: Callable) -> CallMacro:
            calling_locals = inspect.currentframe().f_back.f_locals  # noqa T484
            calling_globals = inspect.currentframe().f_back.f_globals  # noqa T484

            (
                args,
                varargs,
                varkw,
                defaults,
                kwonlyargs,
                kwonlydefaults,
            ) = inspect.getfullargspec(f)  # noqa T484

            if varargs or varkw or defaults or kwonlyargs or kwonlydefaults:
                raise NotImplementedError(
                    f"CallMacro cannot yet support variable args, keyword "
                    f"args, or default values. Provided by '{f.__name__}'"
                )

            hints = typing.get_type_hints(f, localns=calling_locals, globalns=calling_globals,)

            def _on_Call(visitor, args):  # noqa N802
                return f(visitor, *args)

            if is_member:
                args = args[1:]

                def _on_Call(cls, visitor, args):  # noqa T484
                    return f(cls, visitor, *args)

            try:
                arg_types = [hints[a] for a in args]
            except KeyError:
                raise TypeError(
                    f"CallMacro generation requires ToAIRBase and following "
                    f"positional arguments of '{f.__name__}' to be "
                    f"type-hinted"
                )

            if len(arg_types) == 0 or arg_types[0] is not ToAIRBase:
                raise TypeError(
                    f"CallMacro generation requires first positional argument "
                    f"of '{f.__name__}' to be hinted as ToAIRBase type"
                )

            # this excludes the first argument which is always ToAIRBase, and
            # extract metadata from Annotation type
            var_argreps = [a.__metadata__ for a in arg_types[1:]]
            if any([len(a) != 1 or (type(a[0]) is not ArgRep) for a in var_argreps]):  # noqa C419
                raise TypeError(
                    f"CallMacro requires non-first positional argument types "
                    f"of '{f.__name__}' to be annotated with exactly one "
                    f"ArgRep metadata"
                )
            var_argreps = [a[0] for a in var_argreps]

            # dynamically generate a new subclass of CallMacro that is based
            # on f
            return type(  # noqa T484
                f.__name__,
                (CallMacro,),
                {
                    "is_member": is_member,
                    "argreps": lambda: var_argreps,
                    "_on_Call": _on_Call,
                },
            )

        return generate_sub
