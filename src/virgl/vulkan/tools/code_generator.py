from mako.template import Template

import json
import xml.etree.cElementTree as et

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

def filter_listed(function_list, entrypoints):
    listed, non_listed = [], []

    for f in entrypoints:
        if f in function_list:
            listed += [ entrypoints[f] ]
        else:
            non_listed += [ 
                function(f, "void", [ function_param("void", "", "void") ]),
            ]

    return (listed, non_listed)


def generate_from_templates(to_generate, function_list, xml_files):
    template_list = []
    output_filenames = []
    functions = []

    for t in to_generate:
        in_name = t[0]
        output_filenames += [ t[1] ]

        with open(in_name) as f:
            template_list += [ Template(f.read()) ]

    with open(function_list) as f:
        functions = json.loads(f.read())


    listed = []
    non_listed = []

    for filename in xml_files:
        doc = et.parse(filename)
        t_listed, t_non_listed = filter_listed(functions, get_entrypoints(doc))
        listed += t_listed
        non_listed += t_non_listed

    for i in range(len(template_list)):
        f = open(output_filenames[i], "w")

        generated = template_list[i].render(listed=listed, non_listed=non_listed)

        f.write(generated)
        f.close()
        print("generated %s." % output_filenames[i])

    return 0