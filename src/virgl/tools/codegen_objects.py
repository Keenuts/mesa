import argparse
import xml.etree.cElementTree as et
import codegen_utils as code_gen
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

def generate_payload_field_size_exp(parent_input, parent_payload, field):
    for p in fields
    size = 'sizeof(*{0}->{1}) * {2}->{3}'.format(parent_payload,
                                                 field['name'],
                                                 parent_input,
                                                 field['size'])
    if not 'content' in p:
        return [ size ]

    for p in field['content']:
        if not 'size' in p:
            continue

        p_input = '{0}->{1}'.format(parent_input, field['name'])
    size += 
    return sizeof

def generate_payload_size_expr(v_input, v_payload, fields):
    expr = []
    for f in fields:
        if 'size' not in f:
            continue

        size = 'sizeof(*{0}->{2}) * {1}->{3}'.format(
                    v_payload, v_input, f['name'], f['size'])
        expr.append(size)
        
        if not 'content' in f:
            continue

        sub_size = generate_payload_size_expr(
            '{0}->{1}'.format(v_input, f['name'])

    return expr
    
def generate_payload_allocation(indent, info):
    code = '{0}struct payload_{1} *payload = NULL;\n'.format(indent, info['function'])

    size_component = [ 'sizeof(*payload)' ]
    size_component += generate_payload_size_expr('pCreateInfo', 'payload', info['infos'])

    size_expr = ' +\n{0}{0}'.format(indent).join(size_component)
    code = '{0}const uint64_t payload_size = \n{0}{0}{1};\n'.format(indent, size_expr)


    code += '{0}payload = alloca(payload_size);\n'.format(indent)
    return code

def generate_function(info, prototype):
    header = ""
    function = ""
    decl = prototype.to_code()

    # Header generation
    header += decl + ";\n\n"
    header += generate_struct(info)

    # function generation
    function += decl + "\n" + OPEN_SCOPE

    function += generate_payload_allocation(" " * INDENT_SIZE, info)
    
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
