import argparse
import re
import xml.etree.cElementTree as et
import codegen_utils as code_gen
import json
import os
import sys

CREATION_JSON = "./vtest_protocol_objects.json"

INDENT_SIZE = 3
INDENT = INDENT_SIZE * " "
EOL = "\n"
OPEN_SCOPE = "{" + EOL
CLOSE_SCOPE = "}" + EOL
DEFAULT_TYPENAME = 'uint32_t'

def camel_to_snake(name):
    return ''.join([ ("_" + c.lower() if c.isupper() else c) for c in name ])

def generate_prototype(spec):
    old_name = spec.name
    spec.name = "vtest_" + spec.name
    output = spec.to_code()
    output += ";"

    spec.name = old_name
    return output

def generate_structs(protocol):
    indent = " " * INDENT_SIZE;
    structs = []

    for name in protocol['chunks']:
        chunk = protocol['chunks'][name]
        code = 'struct payload_{0}_{1} '.format(protocol['function'], name)
        code += OPEN_SCOPE

        for field in chunk['content']:
            typename = DEFAULT_TYPENAME
            if 'type' in field:
                typename = field['type']
            code += '{0}{1} {2}{3}'.format(indent, typename, field['name'], EOL)
            
        code += CLOSE_SCOPE

        structs.append(code)

    return structs;

def generate_code_init(protocol):
    indent = " " * INDENT_SIZE
    code = []

    code.append("int res;")
    code.append("struct vtest_hdr cmd;")

    for name in protocol['chunks']:
        c = protocol['chunks'][name]
        code.append('{0} {1};'.format(c['typename'], name))

    code = [ indent + c for c in code]
    return code

def generate_code_send_header(id):
    indent = " " * INDENT_SIZE * 1
    code = [ "" ]

    code.append('INITIALIZE_HDR(cmd, {0}, sizeof(cmd));'.format(id))
    code.append('res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));')
    code.append('CHECK_IO_RESULT(res, sizeof(cmd));')

    code = [ indent + c for c in code]
    return code

def generate_code_simple_chunk(chunk):
    code = []

    for field in chunk['content']:
        code.append('{0}.{1} = create_info->{1};'.format(chunk['name'], field['name']))

    code.append('res = virgl_block_write(sock_fd, &{0}, sizeof({0}));'
                .format(chunk['name']))
    code.append('CHECK_IO_RESULT(res, sizeof({0}));'.format(chunk['name']))

    return code

def get_chunk_dependencies(protocol, chunk):
    deps = [ ]

    parent_node = chunk
    while parent_node['parent'] != None:
        node = protocol['chunks'][parent_node['parent']]
        deps.insert(0, (node, parent_node))
        parent_node = node

    return deps

def generate_code_nested_chunk(indent_level, protocol, chunk):
    indent = " " * INDENT_SIZE * indent_level
    code = [ "" ]

    parent = protocol['chunks'][chunk['parent']]
    dependencies = get_chunk_dependencies(protocol, chunk)

    loop_indent = indent
    iterator = chr(ord('i') + indent_level)
    size_var = '{}.{}'.format(chunk['parent'], chunk['count'])
    size_var = 'create_info->'
    to_close = 0

    # opening scopes
    for deps in dependencies:
        index_var = size_var + '{}'.format(deps[1]['count'])
        code.append('{0}for (uint32_t {1} = 0; {1} < {2}; {1}++) {{'
                    .format(loop_indent, iterator, index_var))
        loop_indent += INDENT

        size_var += '{}[{}].'.format(deps[1]['name'], iterator)
        iterator = chr(ord(iterator) + 1)
    
    # assignations

    for field in chunk['content']:
        code.append('{0}{1}.{2} = {3}{2};'
                    .format(loop_indent, chunk['name'], field['name'], size_var))

    code.append('{0}res = virgl_block_write(sock_fd, &{1}, sizeof({1}));'
                .format(loop_indent, chunk['name']))
    code.append('{}CHECK_IO_RESULT(res, sizeof({}));'
                .format(loop_indent, chunk['name']))


    # Closing scopes
    while len(loop_indent) > len(indent):
        loop_indent = loop_indent[:-INDENT_SIZE]
        code += [ loop_indent + "}" ]

    code = [ indent + c for c in code]
    return code

def generate_code_send_chunks(protocol):
    indent = " " * INDENT_SIZE
    code = [ "" ]

    for name in protocol['chunks']:
        chunk = protocol['chunks'][name]

        if chunk['parent'] == None:
            code += generate_code_simple_chunk(chunk)
        else:
            code += generate_code_nested_chunk(0, protocol, chunk)


    code = [ indent + c for c in code]
    return code

def generate_code_read_result():
    indent = " " * INDENT_SIZE
    code = [ "" ]

    code.append('res = virgl_block_read(sock_fd, &result, sizeof(result));')
    code.append('CHECK_IO_RESULT(res, sizeof(res));')
    code.append('*output = result.result')
    code.append('RETURN(result.error_code)')

    code = [ indent + c for c in code]
    return code
    

def generate_function(spec, protocol):
    code = []

    code.append(generate_prototype(spec).replace(";", ""))
    code.append(OPEN_SCOPE)

    code += generate_code_init(protocol)
    code += generate_code_send_header(protocol['id'])
    code += generate_code_send_chunks(protocol)
    code += generate_code_read_result()

    code.append(CLOSE_SCOPE)
    return EOL.join(code)

def cook_entry(protocol):

    for name in protocol['chunks']:
        entry = protocol['chunks'][name]

        entry['name'] = name

        s_name = 'payload_{0}_{1}'.format(protocol['function'], name)
        entry['typename'] = 'struct ' + s_name


def generate_code(to_generate, vk_functions):
    prototype_declarations = []
    struct_declarations = []
    function_implems = []

    for entry in to_generate:
        if entry['function'] not in vk_functions:
            print("unknown vk function %s" % entry['function'])
            return (-1, None, None)

        # Camel to snake case
        spec = vk_functions[entry['function']]
        spec.name = camel_to_snake(spec.name).replace("vk_", "")
        entry['function'] = camel_to_snake(entry['function']).replace("vk_", "")
        
        cook_entry(entry)

        prototype_declarations.append(generate_prototype(spec))
        struct_declarations += generate_structs(entry);
        function_implems.append(generate_function(spec, entry))

    header = EOL.join(prototype_declarations)
    header += EOL + EOL
    header += EOL.join(struct_declarations)

    body = (EOL * 2).join(function_implems)
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
