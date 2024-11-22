#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

'''
Generate the user message code according to the yaml configuration file
'''

import re
import argparse
import yaml

def read_yaml(file):  # -> Union[dict, list, None]:
    '''
    Read the yaml configuration file
    '''
    with open(file, 'r', encoding='utf-8') as f:
        return yaml.load(f, yaml.Loader)

def write_file(file, text):
    '''
    Generate the code file errMsg.inc
    '''
    with open(file, 'w', encoding='utf-8') as f:
        f.write(text)

def translate(text, trans):
    '''
    Batch replacement
    '''
    regex = re.compile('|'.join(map(re.escape, trans)))
    return regex.sub(lambda match: trans[match.group(0)], text)

def format_list(text):
    '''
    Remove the dict to string identifier
    '''
    table = ''.maketrans("{}\'", "   ")
    string = str(text).translate(table).replace(',', ',\n').replace(':', '\t=')

    return string

def format_line(text):
    '''
    Add line feed
    '''
    return text + '\n'

def user_msg(def_dict):
    '''
    Processing User message
    '''
    # msg_list = data.get('errMsgDef', {}).get('def', {}).get('messages')
    msg_list = def_dict['messages']

    msg_text = ''
    first = True
    for s in msg_list:
        table = ''.maketrans("{}[]\'", "     ")
        msg_str = str(s['inStrVarDef']).translate(table).replace(' ', '').replace(':', '::')

        trans = {
           "{{type}}": s['type'],
            "{{key}}": s['key'], 
            "{{errLevel}}": s['errLevel'], 
           "{{msg}}": "\"" + s['msg'] + "\"",
            "{{inStrVarDef}}": msg_str
            }

        item = translate(def_dict['codeConstructor'], trans)
        if first:
            first = False
            msg_text += item
        else:
            msg_text += ',' + item

    return def_dict['wrapper'].replace('{{code}}', msg_text)

def code_gen(msg_dict, data_dict):
    '''
    Generate the source code according to the yaml configuration file
    '''
    text = ''

    # process: user_define.yml
    text += format_line(data_dict['errMsgDef']['comments'])
    text += user_msg(data_dict['errMsgDef']['def'])

    # print(text)

    return text

def head_gen(msg_dict, data_dict):
    '''
    Generate the header file according to the yaml configuration file
    '''
    text = ''

    # process: err_msg.yml
    text += format_line(msg_dict['overallHeader'])
    text += format_line(msg_dict['userDefinedVEnums']['comments'])

    u_code = format_list(msg_dict['userDefinedVEnums']['def']['enums'])
    text += format_line(msg_dict['userDefinedVEnums']['def']['wrapper'].replace('{{code}}', u_code))

    text += format_line(msg_dict['postText'])

    # print(text)

    return text

def get_parser():
    '''
    parameter analysis
    '''
    parser = argparse.ArgumentParser(description='Custom defined User_msg generates err_msg.inc')
    parser.add_argument('-i', '--include', metavar='path', default='./',
                        help='path for output, if not set, output to current path as err_msg.inc.h')
    parser.add_argument('-s', '--src', metavar='path', default='./',
                        help='path for output, if not set, output to current path as err_msg.inc.c')
    return parser

def main():
    '''
    main function entry
    '''
    parser = get_parser()
    args = parser.parse_args()

    include = args.include
    if include[-1] != "/":
        include += "/"
    inc_file = include + 'err_msg.inc.h'

    src = args.src
    if src[-1] != "/":
        src += "/"
    src_file = src + 'err_msg.inc.c'

    msg = read_yaml("err_msg.yml")
    data = read_yaml("user_define.yml")

    head_text = head_gen(msg, data)
    code_text = code_gen(msg, data)

    write_file(inc_file, head_text)
    write_file(src_file, code_text)

if __name__ == '__main__':
    main()
