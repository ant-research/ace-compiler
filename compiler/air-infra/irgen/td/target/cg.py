#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Code generator for target info.
'''

from pathlib import Path
from typing import List
from target.target_info import TargetInfo  # noqa: T484


class TDCodeGen:
    ''' Code generator for target info. '''

    def __init__(self, targets: List[TargetInfo], output: str) -> None:
        self._targets = targets
        self._output = Path(output)

    @property
    def targets(self) -> List[TargetInfo]:
        ''' Getter for targets. '''
        return self._targets

    @property
    def output(self) -> Path:
        ''' Getter for output. '''
        return self._output

    def generate_target(self, target: TargetInfo) -> None:
        ''' Generate target info for a target. '''
        target_dir = self.output / target.name.upper()
        target_dir.mkdir(parents=True, exist_ok=True)

        # generate reg.h
        with open(target_dir / 'reg.h', 'w', encoding='utf-8') as f:
            content = ""
            guard = f"{target.guard}_REG_H"
            content += f"#ifndef {guard}\n"
            content += f"#define {guard}\n\n"
            content += f"#include <cstdint>\n{target.headers}\n"

            # use namespace
            for ns in target.ns[1]:
                content += f"using namespace {ns};\n"

            # decl namespace
            for ns in target.ns[0]:
                content += f"namespace {ns} {{\n"
            content += "\n"
            suffix = "}\n" * len(target.ns[0])

            # global variables
            content += "".join(target.global_vars)

            # find used macros in includes
            # and inserts them into the global scope
            for include in target.includes:
                for uc in target.used_macros:
                    for var in include.global_vars:
                        if uc in var:
                            content += var
            content += "\n"

            # enum for REG_CLASS
            content += "enum REG_CLASS : uint8_t { "
            content += ", ".join(f"{cls.name} = {cls.id}" for cls in target.reg_cls)
            content += " };\n\n"

            # enum for each register
            for cls in target.reg_cls:
                if len(cls.regs) == 0:
                    continue
                content += f"enum {cls.name} : uint8_t {{ "
                content += ", ".join(f"{reg.name}" for reg in cls.regs)
                content += " };\n"

            content += "\n" + suffix + f"\n#endif // {guard}\n"
            f.write(content)

        # generate reg_cls.h
        with open(target_dir / 'reg_cls.h', 'w', encoding='utf-8') as f:
            content = ""
            guard = f"{target.guard}_REG_CLS_H"
            content += f"#ifndef {guard}\n"
            content += f"#define {guard}\n\n"
            content += "#include \"reg.h\"\n\n"

            # use namespace
            for ns in target.ns[1]:
                content += f"using namespace {ns};\n"

            # decl namespace
            for ns in target.ns[0]:
                content += f"namespace {ns} {{\n"
            content += "\n"
            suffix = "}\n" * len(target.ns[0])

            for cls in target.reg_cls:
                # REG_META for each register
                if len(cls.regs) == 0:
                    continue
                cls_meta_name = f"{target.name.upper()}_INFO_{cls.name.upper()}"
                content += "static constexpr struct REG_META "
                content += f"{cls_meta_name}[{len(cls.regs)}] = {{\n"
                for reg in cls.regs:
                    content += f"  {{ \"{reg.name}\", {reg.code}, {reg.size}, {reg.flags} " + "},\n"
                content += "};\n\n"

                # declare REG_INFO_META for each register class
                content += "static constexpr struct REG_INFO_META "
                content += f"{target.name.upper()}_REG_CLASS_{cls.name.upper()} = {{\n"
                content += f"  \"{cls.name}\", {cls.id}, {len(cls.regs)}, {cls.size}, "
                content += "0, "  # NOTE: register class flags
                content += cls_meta_name + "\n};\n\n"

            content += "\n" + suffix + f"\n#endif // {guard}\n"
            f.write(content)

        with open(target_dir / 'isa.h', 'w', encoding='utf-8') as f:
            content = ""
            guard = f"{target.guard}_ISA_H"
            content += f"#ifndef {guard}\n"
            content += f"#define {guard}\n\n"
            content += "#include \"reg.h\"\n\n"

            # use namespace
            for ns in target.ns[1]:
                content += f"using namespace {ns};\n"

            # decl namespace
            for ns in target.ns[0]:
                content += f"namespace {ns} {{\n"
            content += "\n"
            suffix = "}\n" * len(target.ns[0])

            # OPND_FLAG_KIND for each entry
            content += "enum OPND_FLAG_KIND : uint16_t {\n"
            for flag in target.op_flags:
                content += f"  {flag.name} = {hex(flag.id)},\n"
            content += "};\n\n"

            # OP_FLAG_META for each entry
            op_flags_name = f"{target.name.upper()}_OP_FLAG"
            content += "static const struct OPND_FLAG_META "
            content += f"{op_flags_name}[{len(target.op_flags)}] = {{\n"
            for flag in target.op_flags:
                content += "  {" + f"{ flag.name}, \"{flag.desc}\"" + "},\n"
            content += "};\n\n"

            # INST_META for each instruction
            for inst in target.inst_meta:
                content += "static constexpr struct INST_META "
                content += f"{target.name.upper()}_INST_META_{inst.name.upper()} = {str(inst)};\n"

            # register INST_META in an array
            inst_meta_name = f"{target.name.upper()}_INST_META"
            content += "static constexpr struct INST_META const* "
            content += f"{inst_meta_name}[{len(target.inst_meta)}] = {{\n  "
            content += ", ".join(
                f"&{target.name.upper()}_INST_META_{inst.name.upper()}" for inst in target.inst_meta)
            content += "\n};\n"

            # register ISA_INFO_META
            content += "static constexpr struct ISA_INFO_META "
            content += f"{target.name.upper()}_ISA_INFO = {{\n"
            content += f"  \"{target.name.upper()}\", {target.isa_id},\n"
            content += f"  {len(target.inst_meta)}, {len(target.op_flags)},\n"
            content += f"  {inst_meta_name}, {op_flags_name}\n"
            content += "};\n"

            content += "\n" + suffix + f"\n#endif // {guard}\n"
            f.write(content)

        with open(target_dir / 'abi.h', 'w', encoding='utf-8') as f:
            content = ""
            guard = f"{target.guard}_ABI_H"
            content += f"#ifndef {guard}\n"
            content += f"#define {guard}\n\n"
            content += "#include \"reg.h\"\n\n"

            # use namespace
            for ns in target.ns[1]:
                content += f"using namespace {ns};\n"

            # decl namespace
            for ns in target.ns[0]:
                content += f"namespace {ns} {{\n"
            content += "\n"
            suffix = "}\n" * len(target.ns[0])

            # handle calling convention
            abi = target.abi
            conv_name_list = []
            for conv in abi.convs:
                name = f"REG_CONV_{conv.name.upper()}"
                content += "static constexpr struct REG_CONV_META "
                content += f"{name} = {str(conv)};\n"
                conv_name_list.append(name)
            print(conv_name_list)
            content += "\n"

            # register all calling conventions in an array
            content += "static constexpr struct REG_CONV_META const* "
            content += f"REG_CONV_ALL[{len(conv_name_list)}] = {{\n  "
            content += ", ".join(f"&{name}" for name in conv_name_list)
            content += "\n};\n\n"

            # register ABI_INFO_META
            content += "static constexpr struct ABI_INFO_META "
            content += f"{target.name.upper()}_ABI_INFO = {{\n"
            content += f"  \"{target.name.upper()}\", {abi.id}, {abi.gpr_cls},\n"
            content += f"  {abi.fp}, {abi.sp}, {abi.gp}, {abi.tp}, {abi.ra},\n"
            content += f"  {len(conv_name_list)},\n  REG_CONV_ALL\n"
            content += "};\n"

            content += "\n" + suffix + f"\n#endif // {guard}\n"
            f.write(content)

            # generate targ.h
        with open(target_dir / 'targ.h', 'w', encoding='utf-8') as f:
            content = ""
            guard = f"{target.guard}_TARG_H"
            content += f"#ifndef {guard}\n"
            content += f"#define {guard}\n\n"
            content += "#include \"reg_cls.h\"\n"
            content += "#include \"isa.h\"\n"
            content += "#include \"abi.h\"\n\n"

            # use namespace
            for ns in target.ns[1]:
                content += f"using namespace {ns};\n"

            # decl namespace
            for ns in target.ns[0]:
                content += f"namespace {ns} {{\n"
            content += "\n"
            suffix = "}\n" * len(target.ns[0])

            # register gpr
            gpr_cls_name = f"{target.name.upper()}_REG_CLASS_GPR"
            content += f"static REG_INFO_REGISTER Reg_gpr(&{gpr_cls_name});\n"

            # register ISA
            isa_name = f"{target.name.upper()}_ISA_INFO"
            content += f"static ISA_INFO_REGISTER Reg_isa(&{isa_name});\n"

            # register ABI
            abi_name = f"{target.name.upper()}_ABI_INFO"
            content += f"static ABI_INFO_REGISTER Reg_abi(&{abi_name});\n"

            content += "\n" + suffix + f"\n#endif // {guard}\n"
            f.write(content)

    def generate(self) -> None:
        ''' Generate target info. '''
        # create a subdirectory with target name
        for target in self.targets:
            self.generate_target(target)
