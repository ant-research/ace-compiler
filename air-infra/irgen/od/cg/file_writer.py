#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
Write generated code to file
'''

from pathlib import Path
from od.cg.base import BaseCG

class FileWriter:
    '''
    Write generated code to file
    '''

    def __init__(self, path: str):
        self._path = Path(path)
        self._od_path = None

    @property
    def path(self) -> Path:
        ''' Getter for path '''
        return self._path

    @property
    def od_path(self) -> Path:
        ''' Getter for od path '''
        return self._od_path

    def set_od_path(self, od_path: Path) -> 'FileWriter':
        ''' Set od path '''
        self._od_path = od_path
        return self

    def write(self, content: str):
        ''' Write content to file '''
        with self._path.open('w', encoding='UTF-8') as file:
            file.write(content)

    def write_file(self, generator: BaseCG = None):
        ''' Generate content and write to file '''
        content = generator.generate()
        self.write(content)
