//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_ATTR_CONVERTER_H
#define AIR_ATTR_CONVERTER_H

#include <map>
#include <string>
#include <any>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace ace {
namespace frontend {

//! @brief ATTR_CONVERTER - Utility class for converting Python attributes to C++
//!
//! Converts py::object attributes to std::any map for use in operation processing.
//! Handles int, float, vector<int>, and vector<float> types.
class ATTR_CONVERTER {
public:
    //! @brief Convert py::object attribute map to std::any map
    //! @param py_attrs Map of attribute name to py::object value
    //! @return Map of attribute name to std::any value
    static std::map<std::string, std::any> Convert(
        const std::map<std::string, py::object>& py_attrs) {

        std::map<std::string, std::any> attrs_any;

        for (const auto& [key, value] : py_attrs) {
            std::any converted = Convert_value(value);
            if (converted.has_value()) {
                attrs_any[key] = std::move(converted);
            }
        }

        return attrs_any;
    }

    //! @brief Convert single py::object value to std::any
    //! @param value py::object value to convert
    //! @return std::any containing the converted value, or empty if type unknown
    static std::any Convert_value(const py::object& value) {
        // Try int
        try {
            return py::cast<int>(value);
        } catch (const py::cast_error&) {}

        // Try double (must be before float: Python float is C double)
        try {
            return py::cast<double>(value);
        } catch (const py::cast_error&) {}

        // Try float
        try {
            return py::cast<float>(value);
        } catch (const py::cast_error&) {}

        // Try vector<int>
        try {
            return py::cast<std::vector<int>>(value);
        } catch (const py::cast_error&) {}

        // Try vector<double> (must be before vector<float>)
        try {
            return py::cast<std::vector<double>>(value);
        } catch (const py::cast_error&) {}

        // Try vector<float>
        try {
            return py::cast<std::vector<float>>(value);
        } catch (const py::cast_error&) {}

        // Unknown type - return empty any
        return std::any();
    }
};

}  // namespace frontend
}  // namespace ace

#endif  // AIR_ATTR_CONVERTER_H