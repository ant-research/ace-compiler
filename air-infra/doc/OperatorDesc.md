# Operator Description

## Introduction

This document introduces the design and specification of the DSL that describes domain operators.

## Specification

### Domain

As we are describing operators for a specific domain, we should specify the domain info prior defining the operators.

```python
CREATE_DOMAIN(domain_name: str, domain_id: int) -> DomainDescEntry
```

It takes two arguments, where the `domain_id` should be the uniqure id of this domain.

### Methods of DomainDescEntry

#### description(desc: str) -> DomainDescEntry

The description of the current domain


### Types

To register corresponding types of operators, we have `REGISTER_TY` to create an `TypeDescEntry`.

```python
REGISTER_TY(type_name: str, trait: TypeTrait) -> TypeDescEntry
```

The class `TypeDescEntry` should have a few subclass, such as `ArrayTypeDescEntry`, `PrimeTypeDescEntry`, `RecordTypeDescEntry` and so on.

For instance:

```python3
from air import TypeTrait, PrimeType, ArrayType

arr_2_3 = 
  REGISTER_TY("reshape_float_2x3", TypeTrait.ARRAY)
    .set_elem_type(PrimeType.FLOAT_32)
    .set_dim([2, 3])

tensor =
  REGISTER_TY("tensor", TypeTrait.ARRAY)
```

### Methods of TypeDescEntry

This part is not clear for now. Different `Entry` classes should have different methods.

### Operator

To add a new operator in the description file, use the function `REGISTER_OP` to create an object called `OpDescEntry`. It has several method allow developers to specify the following information:

1. Description of this operator
2. Number of arguments
3. Name, type and description of the arguments
4. Category of this operator
5. Number of fields
6. Properties
7. Closure to describe the rule for type/shape inference

All the related APIs should return the updated `OpDescEntry`, maintaining the declarative style.

### Methods of OpDescEntry

#### REGISTER_OP(op_name: str) -> OpDescEntry

Register a new operator in the current domain.

#### OpDescEntry(name: str) -> OpDescEntry

The constructor of OpDescEntry takes the operator name as the argument.

#### description(desc: str) -> OpDescEntry

Set the description of this operator.

#### set_arg_num(num: int) -> OpDescEntry

Set the number of arguments.

#### add_arg(name: str, type: TypeDescEntry, desc: str) -> OpDescEntry

Create an argument for this operator. The `type` argument might be refined in the future.

#### add_prop(prop: OprProp) -> OpDescEntry

Add a properity for this operator.

#### add_attr(name: str, value: Generic[V], desc: str) -> OpDescEntry

Add an attribute for this operator.

#### type_and_shape_inference(ty_rule: TypeRule) -> OpDescEntry

Register the function for type and shape inference. The type rules should be written as assertions. The definition of `TypeRule` is:

```python
class TypeRule:
  def __init__(self, name: str):
    self.name = name

  def rule(self, *args, **kwargs):
    assert True

  def get_name(self) -> str:
    return self.name
```

## Examples of OD framework

```python
CREATE_DOMAIN("NN", 1)

def conv_type_rule()

class ConvTypeRule(TypeRule):
  def __init__(self):
    super().__init__("ConvTypeRule")
  
  def rule(self, ctx, args, attrs):
    input_tensor = args[0]
    kernel = args[1]
    padding = attrs.get('padding')
    strides = attrs.get('strides')

    assert input_tensor.has_shape()
    assert attrs.has('kernel_shape')
    assert input_tensor.get_elem_type() == kernel.get_elem_type()

    # info of input tensor
    input_shape = input_tensor.get_shape()
    batch_size = input_shape.get_batch_size()
    in_channels = input_shape.get_channels()
    input_height = input_shape.get_height()
    kernel_width = input_shape.get_width()

    # info of kernel
    kernel_shape = attrs.get('kernel_shape')
    out_channels = kernel_shape.get_channels()
    kernel_height = kernel_shape.get_height()
    kernel_width = kernel_shape.get_width()

    out_height = (input_height - kernel_height + 2 * padding[0]) // stride[0] + 1
    output_width = (input_width - kernel_width + 2 * padding[1]) // stride[1] + 1

    ctx.set_out_height(out_height)
    ctx.set_out_width(output_width)


REGISTER_OP("CONV", "conv")
  .description("Performs a convolution on given the tensor")
  .set_arg_num(3)
  .add_arg("X", tensor, "the input tensor for convolution")
  .add_arg("W", tensor, "the kernel")
  .add_arg("B", tensor, "1D bias")
  .add_prop(OprProp.EXPR)
  .add_prop(OprProp.ATTR)
  .type_and_shape_inference(ConvTypeRule)
```

An AIR OPCODE entry should cover:

| Field   | Requirement          | Description                               | Type                            |
| ------- | -------------------- | ----------------------------------------- | ------------------------------- |
| desc    | required             | OPCODE description                        | string                          |
| arg_num | required             | number of operands                        | uint32                          |
| prop    | required             | OPCODE properties (AIR pre-defined)       | uint32 bits                     |
| typing  | required             | Typing relation between operands & result | ??? (TBD)                       |
| arg     | optional (0 or many) | operand information                       | string(name, type, description) |
| attr    | optional (0 or many) | OPCODE attributes (for user defined)      | string(key)                     |


## Format of Generated CXX/Header File

## TODO

1. OD -> python (nn, vector, user)
    - OD spec
2. OD Framework
    - Input file
        - python
        - x.h/x.c++
    - Process OD
    - master file + validate
    - API reference ???
3. cmake/integration
    - How to integrated it into build system
    - build process