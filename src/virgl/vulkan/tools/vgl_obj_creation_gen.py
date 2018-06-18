import argparse
import xml.etree.cElementTree as et
import code_generator as code_gen
import json
import os
import sys

CREATION_JSON = "./obj_creation.json"
HEADER_TEMPLATE_PATH = "./templates/vgl_objects_template.h"
CODE_TEMPLATE_PATH = "./templates/vgk_objects_template.c"

INDENT_SIZE = 3
OPEN_SCOPE = "{\n"
CLOSE_SCOPE = "}\n"

def generate_struct_field(indent, field):
    # simple field (uint32_t)
    if 'size' not in field:
        return format("%suint32_t %s;\n" % (indent, field['name']))

    # simple built-in array
    if 'content' not in field:
        return format("%suint32_t %s[];\n" % (indent, field['name']))

    out = format("%sstruct {\n" % indent)
    for p in field['content']:
        out += generate_struct_field(indent + " " * INDENT_SIZE, p)

    out += format("%s} %s[];\n" % (indent, field['name']))
    return out

def generate_struct(info):
    code = ""

    code += format("struct payload_%s {\n" % info['function'])
    indent = " " * INDENT_SIZE;

    for p in info['infos']:
        code += generate_struct_field(indent, p)

    return code + "};\n"

def generate_function(info, prototype):
    header = ""
    function = ""

    decl = prototype.to_code()

    function += decl + "\n"
    header += decl + ";\n\n"
    header += generate_struct(info)

    function += OPEN_SCOPE

    
    function += CLOSE_SCOPE

    return (0, function, header)

def generate_code(to_generate, vk_functions):
    body = ""
    header = ""

    for f in to_generate:
        if f['function'] not in vk_functions:
            print("unknown vk function %s" % f['function'])
            return (-1, None, None)

        prototype = vk_functions[f['function']]

        err, b, h = generate_function(f, prototype)
        if err != 0:
            return (err, None, None);

        body += b
        header += h

    return (0, body, header)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--outdir', help='Where to write the files.', required=True)
    parser.add_argument('--xml', help='Vulkan API XML file.', required=True)
    args = parser.parse_args()

    script_path = os.path.dirname(sys.argv[0])
    to_generate = []
    vk_functions = {}

    with open(CREATION_JSON) as f:
        to_generate += json.loads(f.read())

    vk_functions = code_gen.get_entrypoints(et.parse(args.xml))
            
    body_code = ""
    header_code = ""

    err, body_code, header_code = generate_code(to_generate, vk_functions)
    if err != 0:
        return err;

    print(header_code)
    print()
    print()
    print()
    print(body_code)

if __name__ == '__main__':
    exit(main())
