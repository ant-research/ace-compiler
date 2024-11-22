# protobuf on macOS

## Install the specified version of protobuf

Because brew install protobuf for macOS installs the latest version by default, and protobuf introduced the absl library after version v22.0-rc1, there are some issues with the linking of the library, so it is recommended to manually install v21.12 and earlier using protobuf on macOS.

- Check the current version of protobuf

```
brew info protobuf
==> protobuf: stable 24.3 (bottled)
Protocol buffers (Google's data interchange format)
https://protobuf.dev/
/usr/local/Cellar/protobuf/24.3 (395 files, 13.2MB) *
  Poured from bottle using the formulae.brew.sh API on 2023-09-14 at 19:29:15
From: https://github.com/Homebrew/homebrew-core/blob/HEAD/Formula/p/protobuf.rb
License: BSD-3-Clause
==> Dependencies
Build: cmake ✘, python@3.10 ✘, python@3.11 ✔
Required: abseil ✔, jsoncpp ✔
==> Caveats
Emacs Lisp files have been installed to:
  /usr/local/share/emacs/site-lisp/protobuf
==> Analytics
install: 72,183 (30 days), 166,333 (90 days), 326,803 (365 days)
install-on-request: 32,263 (30 days), 71,816 (90 days), 148,769 (365 days)
build-error: 62 (30 days)
```

- Finds the specified version rb file
 
    - Open your browser to the address for protobuf.rb and click History
        https://github.com/Homebrew/homebrew-core/blob/HEAD/Formula/p/protobuf.rb

    - Locate the commit for the specified version (here version v21.12 is an example) and click the commit number to view the file.

    - Click view file to view the contents of the file.

    - Save the file and name it protobuf.rb

- Install the specified version of protobuf

Attached is version v21.12 of protobuf.rb

```
brew unlink protobuf
brew install ./protobuf.rb
protoc --version
libprotoc 21.12
```