import os
import argparse

from py import parse

parser = argparse.ArgumentParser(description='Convert python AST to AIR')
parser.add_argument('--pyfile', default='./demo.py', type=str, metavar='PATH', help='Loader python file')

args = parser.parse_args()

def main():
    tree = parse.ast_parse(args.pyfile)
    parse.visit_node(tree)


if __name__ == '__main__':
    main()