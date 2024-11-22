# Attribute Support in AIR

**Important**: Markdown file (.md) is the master copy. PDF file (.pdf) is exported from markdown file only for review.

## Revision History

|Version|Author     |Date      |Description|
|-------|-----------|----------|-----------|
|0.1    ||2023.09.28|Initial version.|

## Introduction
Attribute is used to carry auxiliary information for NODE, TYPE and SYMBOL, etc.

## Data Structures
### ATTR_ID
ATTR_ID is the unique identifier for a single attribute.
```c++
typedef ID<ATTR_DATA> ATTR_ID;
```

### ATTR_DATA
ATTR_DATA represents one attribute, include key, value and ID to next ATTR entry.
```c++
struct ATTR_DATA {
  ATTR_ID _next;    // next attribute ID
  STR_ID  _key;     // key of the attribute
  STR_ID  _value;   // value of the attribute
};
```

### Attribute linked list
All attributes annotated on the same object are represent by a linked list. NODE_DATA/TYPE_DATA/etc with __'ATTR'__ property contains an ATTR_ID to indicate the header of the attribute linked list.
```c++
class NODE_DATA {
  ......
  ATTR_ID _attr;
};
class TYPE_DATA {
  ......
  ATTR_ID _attr;
};
class SYM_DATA {
  ......
  ATTR_ID _attr;
};
``` 

### ATTR_ITER
ATTR_ITER is introduced to iterate all attributes annotated on 1 NODE/TYPE/SYM.
```c++
class ATTR_ITER {
  ATTR_ITER& operator++();
  ATTR_PTR   operator*();
  bool       operator==(const ATTR_ITER& iter) const;
  bool       operator!=(const ATTR_ITER& iter) const;
}
```

### ATTR_TAB
Similar to other tables, ATTR_TAB is created to contains all ATTR_DATAs. Each FUNC_SCOPE has 1 ATTR_TAB for all attributes annotated on NODE and local symbols. GLOBAL_SCOPE has 1 ATTR_TAB for all attributes on TYPE and global symbols.

```c++
typedef ARENA<sizeof(ATTR_DATA), 4, false> ATTR_TAB;
class FUNC_SCOPE {
  ......
  ATTR_TAB* _attr_tab;
};
class GLOB_SCOPE {
  ......
  ATTR_TAB* _attr_tab;
};
```

## NODE Attribute
NODE attributes are stored in ATTR_TAB in FUNC_SCOPE.

### ATTR Property
To indicate node have attributes, the META_INFO for this OPCODE must have __'ATTR'__ property. For example:
```c++
DEF_OPCODE(GEMM, gemm, OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_ATTR)
```

### Node interface
To get and set attribute, use NODE_PTR interface Attr() and Set_attr():
```c++
NODE_PTR node;
// use string as value type
node->Set_attr("key", "value");
const char* attr = node->Attr("key");  // returns "value"
// use template parameter as value type
node->Set_attr<VAL_TYPE>("key", value);
const VAL_TYPE& attr = node->Attr<VAL_TYPE>("key");  // returns value
```

So far __no__ interface to update or delete attribute. Not sure if these two interfaces are needed.

To traverse node's attribute list, use ATTR_ITER and Begin_attr(), End_attr():
```c++
ATTR_ITER it;
for (it = node->Begin_attr(); it != node->End_attr(); ++it) {
  const char* key = (*it)->Key()->Char_str();
  const char* val = (*it)->Val()->Char_str();
}
```

## TYPE Attribute
TYPE attributes are stored in ATTR_TAB in GLOBAL_SCOPE.

### Type interface
To get and set attribute, use TYPE_PTR interface Attr() and Set_attr():
```c++
TYPE_PTR type;
// use string as value type
type->Set_attr("key", "value");
const char* attr = type->Attr("key");  // returns "value"
// use template parameter as value type
type->Set_attr<VAL_TYPE>("key", value);
const VAL_TYPE& attr = type->Attr<VAL_TYPE>("key");  // returns value
```

## SYMBOL Attribute
Attributes for local symbol are stored in ATTR_TAB in FUNC_SCOPE. Attributes for global symbol are stored in ATTR_TAB in GLOB_SCOPE.

### Symbol interface
To get and set attribute, use SYM_PTR interface Attr() and Set_attr():
```c++
SYM_PTR sym;
// use string as value type
sym->Set_attr("key", "value");
const char* attr = sym->Attr("key");  // returns "value"
// use template parameter as value type
sym->Set_attr<VAL_TYPE>("key", value);
const VAL_TYPE& attr = sym->Attr<VAL_TYPE>("key");  // returns value
```
