The devtools directory contains files used to format and check naming convention of codes, check naming convention documents for details.

In naming convention check, we reuse clang-tidy's Camel_Snake_Case as our Capitalized_snake_case.
And we replace llvm::Regex with std::regex because the former has bugs in matching, check regex.patch for details. And the patch is based on llvm 16.0.0.

IMPORTANT:
The script will not walk through files not in repository. Add your files to repository first so that format and naming convention check are performed automatically in make command.
