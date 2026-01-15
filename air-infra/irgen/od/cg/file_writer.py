#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
'''
Write generated code to file
'''

from typing import Optional
from pathlib import Path
from cg.base import BaseCG  # noqa: T484


class FileWriter:
    '''
    Write generated code to file
    '''

    def __init__(self, path: Path):
        self._path = path
        self._od_path: Optional[Path] = None

    @property
    def path(self) -> Path:
        ''' Getter for path '''
        return self._path

    @property
    def od_path(self) -> Optional[Path]:
        ''' Getter for od path '''
        return self._od_path

    def set_od_path(self, od_path: Path) -> 'FileWriter':
        ''' Set od path '''
        self._od_path = od_path
        return self

    def write(self, content: str) -> Path:
        ''' Write content to file '''
        with self._path.open('w', encoding='UTF-8') as file:
            file.write(content)
        return self._path

    def write_file(self, generator: BaseCG):
        ''' Generate content and write to file '''
        content = generator.generate()
        self.write(content)
