# Command Line Option Specification and Design

**Important**: Markdown file (.md) is the master copy. PDF file (.pdf) is exported from markdown file only for review.


## Revision History

|Version|Author       |Date      |Description|
|-------|-------------|----------|-----------|
|0.1    |tianxiang.sui|2023.09.01|Initial version.|

## Introduction
We want every phase(VVHIR, VHIR, HIR, MIR, LIR) be able to run as standalone programs with onnx or AIR file as input file.
Every phase will introduce some options to help debugging and bug triaging. Such options will also be usable in the integrated compiler
and show up as group option.
Besides, we hope other companies and research team can add new components easily with our infrastructure.
With these requirements, the option process is to provide service for all components as standalone program or as a phase of the compiler with simplicity
and compatibility to Open64, GCC and LLVM command line.

## Common Feature
Common features:
* option name is case sensitive, consists of alphanumeric, underline, dash
* option name begins with dash -, dash dash --
* option is unordered
* some complex option name has abbreviation name as its alias
* unrecgnized option will trigger a warning then continue or error then stop???
* ...

## Top Level Option Feature
Top level option feature:
* no value, default is false, true if option provided
* with value, value type can be int,bool,string,list; 
* the value maker of option name and value is space or =
* ...

## Group Option Feature
Group option feature, if ther is a conflict with commnon feature, will override common feature.
* group name must be uppercase letter
* the separator of group option is :
* the value maker of group option only support equal sign =, and it will override the value maker of option desc except option desc's value maker is V_NONE
* group option format: -XXX:a:b=value, this example means 'a' is true, 'b'=value
* ...

## Design and Implement
Using .yaml file to hold all the command line option meta information.
A tool will parse .yaml file and generate a .inc file.
Register all the options in .inc file to option manager.
Access option through option manager or driver.

### Design Detail
See the option.h in air-infra/include/air/driver/option.h

### How To Incorporate In Each Phase
* Write all the command line option meta information into .yaml file
* Using the tool parse .yaml file and generate a .inc file
* Define your driver class and register all the options to option manager

### Current Status
End to end running examples are provided.

### Work To Do In The Future
* Write a tool to parse yaml file and generate .inc file. This tool will probably use python3.
* Improve and support rich command line usage scenarios
* Add some test cases
* Improve error handling
* top level option overlapping problem






