//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_DOCS_BASIC_H
#define AIR_DOCS_BASIC_H

// Include necessary headers
#include <cstdio>

#include "air/base/node.h"

namespace my_project {

//! Define enum
enum BASIC_ENUM {
  BASIC_NONE = 0,
  BASIC_ONE  = 1,
  BASIC_TWO  = 2,
};

//! Declare a class
class BASIC {
public:
  //! @brief Construct a new BASIC object
  //! @param v Init value for data
  BASIC(int v) : _data(v){};

  //! Destroy the BASIC object
  ~BASIC(){};

  //! @brief Member function declaration
  //! @param f Print out target
  void Print(FILE* f);

  //! @brief Add v with BASIC's data
  //! @param v Add operand
  //! @return int
  int Add_val(int v);

  //! Class enum
  enum CLASS_ENUM {
    CLASS_NONE = 0,  //!< comment CLASS_NONE
    CLASS_ONE  = 1,  //!< comment CLASS_ONE
    CLASS_TWO  = 2   //!< comment CLASS_TWO
  };

private:
  BASIC(void);                     // REQUIRED UNDEFINED UNWANTED methods
  BASIC(const BASIC&);             // REQUIRED UNDEFINED UNWANTED methods
  BASIC& operator=(const BASIC&);  // REQUIRED UNDEFINED UNWANTED methods

  int _data;
};

}  // namespace my_project

#endif
