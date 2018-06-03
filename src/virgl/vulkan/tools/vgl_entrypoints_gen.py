import argparse
import json
import os
import sys
import xml.etree.cElementTree as et

from mako.template import Template

HEADER_TEMPLATE_PATH = "./templates/vgl_entrypoints_template.h"
CODE_TEMPLATE_PATH = "./templates/vgl_entrypoints_template.c"
IMPLEM_LIST = "./implemented.json"

class function_param():
    def __init__(self, typename, name, decl):
        self.type = typename
        self.name = name
        self.decl = decl

    def __str__(self):
        return self.type + " " + self.name

def hash_name(name):
    h = 0
    g = 0

    for c in name:
        h = (h << 4) + ord(c)
        g = h & 0xf0000000
        if g:
            h ^= g >> 24;
        h &= ~g

    return h

class function():
    def __init__(self, name, ret_value, params):
        self.name = name
        self.ret_value = ret_value
        self.params = params
        self.hash = hash_name(name)

    def __str__(self):
        params = ""

        for i in range(len(self.params)):
            if i == 0:
                params = str(self.params[i])
            else:
                params += "," + str(self.params[i])

        return format("%s %s(%s);" % (str(self.ret_value), str(self.name), params))

def get_entrypoints(doc):
    entrypoints = {}

    for command in doc.findall('./commands/command'):
        if 'alias' in command.attrib:
            continue
        else:
            name = command.find('./proto/name').text
            ret_type = command.find('./proto/type').text
            params = [ 
                function_param(
                    p.find('./type').text,
                    p.find('./name').text,
                    ''.join(p.itertext())
                ) for p in command.findall('./param') ]

            entrypoints[name] = function(name, ret_type, params)

    return entrypoints

def filter_implemented(implem_list, entrypoints):
    implemented, non_implemented = [], []

    for f in entrypoints:
        if f in implem_list:
            implemented += [ entrypoints[f] ]
        else:
            non_implemented += [ 
                function(f, "void", [ function_param("void", "", "void") ]),
            ]

    return (implemented, non_implemented)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--outdir', help='Where to write the files.', required=True)
    parser.add_argument('--xml',
                        help='Vulkan API XML file.',
                        required=True,
                        action='append',
                        dest='xml_files')
    args = parser.parse_args()

    script_path = os.path.dirname(sys.argv[0])

    with open(os.path.join(script_path, HEADER_TEMPLATE_PATH)) as f:
        header_template = Template(f.read())
    with open(os.path.join(script_path, CODE_TEMPLATE_PATH)) as f:
        code_template = Template(f.read())

    with open(os.path.join(script_path, IMPLEM_LIST)) as f:
        implem_list = json.loads(f.read());

    implemented = []
    non_implemented = []

    for filename in args.xml_files:
        doc = et.parse(filename)
        imp, non_imp = filter_implemented(implem_list, get_entrypoints(doc))
        implemented += imp
        non_implemented += non_imp

    with open(os.path.join(args.outdir, 'vgl_entrypoints.h'), 'wb') as f:
        f.write(header_template.render(
            implemented=implemented,
            non_implemented=non_implemented)
        )

    with open(os.path.join(args.outdir, 'vgl_entrypoints.c'), 'wb') as f:
        f.write(code_template.render(
            implemented=implemented,
            non_implemented=non_implemented)
        )
    
    return 0

if __name__ == '__main__':
    main()
