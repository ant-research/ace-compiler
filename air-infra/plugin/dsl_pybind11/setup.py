"""
Provides necessary Python setup
"""
from setuptools import setup, find_packages

setup(
    name='instantiators',
    version='0.0.1',
    packages=[
        *find_packages(include=['instantiator', 'instantiator/*']),
        *find_packages(include=['instantiator_ckks', 'instantiator_ckks/*'])
    ],
    install_requires=[],
    package_dir={
        'instantiator': 'instantiator',
        'instantiator_ckks': 'instantiator_ckks'
    },
)
