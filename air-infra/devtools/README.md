The devtools directory contains files used to format and check naming convention of codes.

IMPORTANT:
The script to run tools listed below will not walk through files not in repository. Add your files to repository first so that format and naming convention check are performed automatically in make command.

## 1. C/C++ Coding Style Tools
### 1.1 clang-format
> version 16.0.0

is used to format C/C++ codes automatically, with the formats set in the file [*.clang-format*](.clang-format).

### 1.2 clang-tidy
> built on modified LLVM version 16.0.0 with [*regex.patch*](regex.patch)

is used to check naming convension of C/C++ codes described in the file [*.clang-tidy*](.clang-tidy).

We reuse clang-tidy's Camel_Snake_Case as our Capitalized_snake_case.
And we replace llvm::Regex with std::regex because the former has bugs in matching,
check [*regex.patch*](regex.patch) for details.

## 2. Python Coding Style Tools
### 2.1 autopep8
> version 2.3.1 with (pycodestyle: 2.12.1)

is used to format Python codes automatically according to the [PEP8](https://peps.python.org/pep-0008/#naming-conventions) coding style.

### 2.2 flake8
> version 7.1.1 with
> (TorchFix: 0.3.0, flake8-bugbear: 24.10.31, flake8-comprehensions: 3.15.0, flake8-executable: 2.1.3, flake8-logging-format: 2024.24.12, flake8-mypy: 17.8.0, flake8_simplify: 0.21.0, mccabe: 0.7.0, pep8-naming: 0.14.1, pycodestyle: 2.12.1, pyflakes: 3.2.0)

is used to check Python naming and coding issues with configuration set in [*.flake8*](.flake8), which is taken from Pytorch project with minor changes (add pep8-naming check, delete Pytorch source file specific configurations). We follow [PEP8](https://peps.python.org/pep-0008/#naming-conventions) for Python codes.

### 2.3 mypy
> version 1.13.0

is used to type check Python codes with configuration set in [*.mypy.ini*](.mypy.ini). You need to run this tool manually for the detailed issues, if there is a **T499** issue reported in flake8 checks.
