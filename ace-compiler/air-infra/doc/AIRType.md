## Domain specific types
In a domain layer, it might be interested in certain types though constructed via basic built-in types. Tagging such types provide more convenience for internal processing.

To define domain specific type tags:
```
DEF_DOMAIN_TAG(POLY, PLAIN, CIPH, CIPH3, ...)
```
An internal global variable is then generated and used to represent the tag:
```
static constexpr uint32_t CKKS_POLY = (DOMAIN_ID << 10) | ENUM_POLY;
```
The tag value is unique and is the combination of domain id and enum value of the tag.

To assign a domain specific type tag to an AIR type, use:
```
TYPE::Set_tag(uint32_t);  // e.g. Set_tag(CKKS_POLY);
```

To get type id of a domain specific type tag:
```
TYPE_ID Tag_type_id(uint32_t);  // e.g. Tag_type_id(CKKS_POLY);
```
We assume one-one mapping for tags and types for now.
We may expect one tag maps to multiple types in the future.
For a tag that maps to multiple types, return the first match type found.
If no match type found, return none.

Generate API to determine if a type is of a domain tag like:
```
bool Is_poly_type(TYPE_ID);
```
Or use a more generic API like
```
bool Is_domain_type(uint32_t, TYPE_ID);
```

## Domain specific typing rules
A domain may defines its own opcodes, thus typing rules for these opcodes are needed for type checking and type inference.
Domain specific typing rules will be interpreted and processed automatically by the compiler framework.
If typing rules for an opcode is not specified, type checking will not be done for it and type inference needed for such opcode will lead to compiler errors.
For an opcode with multiple typing rules, the checks and inference will refer to the first matched rule.

To define a type rule, one first need to define how oprands or result of an opcode is typed:
```
enum OPC_TYPING_KIND {
  NONE,        // no type, no checks
  BUILTIN,     // refer to built-in types like uint32_t
  OPND,        // refer to operand type
  DOMAIN_TAG,  // refer to domain type tag
  ANY,         // don't care, no checks
};
```
Then the rule comes straightforward:
```
DEF_OP_TYPE(MUL, OPND, 1, DOMAIN_TAG, CKKS_CIPH, OPND, 1)
DEF_OP_TYPE(MUL, OPND, 1, DOMAIN_TAG, CKKS_CIPH, DOMAIN_TAG, CKKS_PLANT)
DEF_OP_TYPE(ENCODE, DOMAIN_TAG, CKKS_PLANT, ANY, 0)
DEF_OP_TYPE(RESCALE, OPND, 1, DOMAIN_TAG, CKKS_CIPH)
```

TODO: how to make TENSOR typing work
```
DEF_OP_TYPE(GEMM, OPND, 3, DOMAIN_TAG, TENSOR, DOMAIN_TAG, TENSOR, DOMAIN_TAG, TENSOR)
DEF_OP_TYPE(CONV, DOMAIN_TAG, TENSOR, DOMAIN_TAG, TENSOR, DOMAIN_TAG, TENSOR, DOMAIN_TAG, TENSOR)
```
