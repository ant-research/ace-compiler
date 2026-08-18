#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
Code generator for opcode header/CXX file
'''

import inspect
from pathlib import Path
from functools import reduce
from od.domain import DomainDescEntry
from od.cg.base import BaseCG
from od.cg.config import Config
from od.cg.file_writer import FileWriter


class OpcodeHeaderConfig(Config):
    '''
    Config class for the opcode header code generator
    '''

    def __init__(self):
        super().__init__()
        (self.add_include("air/base/opcode.h", False)
             .add_include("cstdint", True))


class OpcodeCXXConfig(Config):
    '''
    Config class for the opcode CXX code generator
    '''

    def __init__(self):
        super().__init__()
        self.set_opcode_header("opcode.h")
        (self.add_include("air/base/meta_info.h", False)
             .add_using_namespace("air::base"))

    def set_opcode_header(self, header: str) -> 'OpcodeCXXConfig':
        ''' Set the header file of opcode '''
        self._opcode_header = header
        self.add_include(header, False)
        return self

    def get_opcode_header(self) -> str:
        ''' Get the header file of opcode '''
        return self._opcode_header

    @property
    def opcode_header(self) -> str:
        ''' Getter for opcode header '''
        return self._opcode_header


class OpcodeCXXBaseCG(BaseCG):
    '''
    Base class for opcode CXX code generator
    '''

    def __init__(self, config: Config, domain: DomainDescEntry):
        super().__init__(config)
        self._domain = domain
        self._content = ""

    @property
    def domain(self) -> DomainDescEntry:
        ''' Getter for domain '''
        return self._domain

    @property
    def content(self) -> str:
        ''' Getter for content '''
        return self._content

    @content.setter
    def content(self, value: str):
        ''' Setter for content '''
        self._content = value

    def check_origin_master(self, origin_master: str):
        ''' Check the origin master file '''
        if not Path(origin_master).exists():
            return True
        with open(origin_master, "r", encoding='UTF-8') as f:
            if self.domain.name == f.readline().rstrip(" = [\n"):
                i = 0
                for line in f:
                    if line == "]":
                        break
                    op = line.strip(",\n").strip()
                    if op != self.domain.ops[i].name:
                        error_msg = (f"Operator \"{op}\" at line {i + 1} is changed, "
                                     f"should be \"{self.domain.ops[i].name}\"")
                        raise ValueError(error_msg)
                    i += 1
                if i != len(self.domain.ops):
                    raise ValueError("Number of operators is changed")
            else:
                raise ValueError("Domain name is changed")
        return True

    def generate_master(self, origin_master: str) -> str:
        ''' Generate master file '''
        if not self.check_origin_master(origin_master):
            raise ValueError("Origin master file is not correct")

        content = self.domain.name + " = ["
        content += reduce(lambda acc, op: acc + f"\n  {op.name},", self.domain.ops, "").strip(",")
        content += "\n]"
        return content

    def generate_operator(self) -> str:
        ''' Generate operator code in  '''
        return ""

    def generate_body(self) -> str:
        ''' Generate the body of the file '''
        # Generate includes
        content = self.generate_includes()

        # Generate using namespace
        content += self.generate_using_namespace()

        # Generate namespaces
        content += self.generate_namespace_prefix()

        # Generate domain
        content += self.generate_domain()

        # Generate namespaces
        content += self.generate_namespace_suffix()
        return content


class OpcodeHeaderFileCG(OpcodeCXXBaseCG):
    '''
    Code generator for opcode header file
    '''
    def __init__(self, config: OpcodeHeaderConfig, domain: DomainDescEntry):
        super().__init__(config, domain)
        if not config.has_namespaces():
            raise ValueError("OpcodeHeaderFileCG should have namespaces")
        if not config.has_macro_guard():
            raise ValueError("OpcodeHeaderFileCG should have macro guard")

    def generate_domain(self) -> str:
        ''' Generate domain code '''
        # Generate domain id
        content = f"static constexpr uint32_t {self.domain.name} = {self.domain.domain_id};\n\n"

        # Generate opcode enum
        content += "enum OPCODE : uint32_t {\n"
        for op in self.domain.ops:
            content += f"    {op.name.upper()},\n"
        content += "    LAST = 0xff\n"
        content += "};\n\n"

        # Generate OPCODE objects
        for op in self.domain.ops:
            content += (f"static constexpr air::base::OPCODE OPC_{op.name.upper()}"
                        f"({self.domain.name}, OPCODE::{op.name.upper()});\n")
        content += "\n"

        # Generate register function
        content += f"bool Register_{self.domain.name.lower()}();\n"
        return content

    def generate(self) -> str:
        ''' Generate the whole file '''
        self.content = self.config.get_cpp_license()

        # Generate macro guard
        self.content += f"#ifndef {self.config.get_macro_guard()}\n"
        self.content += f"#define {self.config.get_macro_guard()}\n\n"

        # Generate body
        self.content += self.generate_body()

        # Generate macro guard
        self.content += f"#endif // {self.config.get_macro_guard()}\n\n"
        return self.content

class OpcodeCXXFileCG(OpcodeCXXBaseCG):
    '''
    Code generator for opcode file
    '''

    def __init__(self, config: OpcodeCXXConfig, domain: DomainDescEntry):
        super().__init__(config, domain)
        if not config.has_namespaces():
            raise ValueError("OpcodeCXXFileCG should have namespaces")
        if config.opcode_header == "":
            raise ValueError("OpcodeCXXFileCG should have opcode header")

    def generate_domain(self) -> str:
        ''' Generate domain code '''
        # Generate operator info for all operators
        content = "static const air::base::OPR_INFO Opr_info[] = {\n"
        for op in self.domain.ops:
            content += f"    {{\"{op.name}\", {op.arg_num}, 0, "
            content += " | ".join(reduce(lambda acc, prop: acc + [str(prop)], op.properties, []))
            content += "},\n"
        content += "};\n\n"

        # Generate domain info for call
        content += "static const air::base::DOMAIN_INFO Domain_info = {\n"
        content += f"    \"{self.domain.name}\", "
        content += "::".join(self.config.get_namespaces()) + "::" + self.domain.name
        content += ", sizeof(Opr_info) / sizeof(Opr_info[0]), Opr_info};\n\n"

        # Register domain to meta_info registry
        content += f"bool Register_{self.domain.name.lower()}() {{\n"
        content += "    return air::base::META_INFO::Register_domain(&Domain_info);"
        content += "}\n\n"
        return content

    def generate(self) -> str:
        ''' Generate the whole file '''
        self.content = self.config.get_cpp_license()

        # Generate body
        self.content += self.generate_body()

        return self.content

class HeaderWriter(FileWriter):
    '''
    Write generated header file
    '''

    def __init__(self, path: str):
        super().__init__(path / "opcode.h")

    def write_file(self, generator: OpcodeHeaderFileCG = None):
        ''' Generate content and write '''
        content = generator.generate()
        self.write(content)


class CXXWriter(FileWriter):
    '''
    Write generated CXX file
    '''

    def __init__(self, path: str):
        super().__init__(path / "opcode.cxx")

    def write_file(self, generator: OpcodeCXXFileCG = None):
        ''' Generate content and write '''
        content = generator.generate()
        self.write(content)


class MasterWriter(FileWriter):
    '''
    Write generated master file
    '''

    def write_file(self, generator: OpcodeHeaderFileCG = None):
        ''' Generate content and write '''
        origin_master = self.od_path / "master"
        self._path = origin_master
        content = generator.generate_master(origin_master)
        self.write(content)


class OpcodeWrite(FileWriter):
    '''
    Write generated opcode to header/CXX file
    '''

    def __init__(self, path: str):
        super().__init__(path)

        # Get the path of the od file
        caller_frame = inspect.stack()[1]
        caller_path = caller_frame.filename
        self._od_path = Path(caller_path).parent

        self._header_cg = None
        self._cxx_cg = None

    def set_header_cg(self, header_cg: OpcodeHeaderFileCG) -> 'OpcodeWrite':
        ''' Set the header code generator '''
        self._header_cg = header_cg
        return self

    def set_cxx_cg(self, cxx_cg: OpcodeCXXFileCG) -> 'OpcodeWrite':
        ''' Set the CXX code generator '''
        self._cxx_cg = cxx_cg
        return self

    def write_file(self, generator: BaseCG = None):
        ''' Generate content and write to file '''
        assert generator is None
        if self._header_cg is None:
            raise ValueError("Header code generator is not set")
        if self._cxx_cg is None:
            raise ValueError("CXX code generator is not set")

        out_path = self.path
        Path(out_path).mkdir(parents=True, exist_ok=True)
        CXXWriter(out_path).write_file(self._cxx_cg)
        HeaderWriter(out_path).write_file(self._header_cg)
        MasterWriter(out_path).set_od_path(self.od_path).write_file(self._header_cg)
