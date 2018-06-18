import argparse
import codegen_utils as code_gen
import os
import sys

HEADER_TEMPLATE_PATH = "./templates/vgl_entrypoints_template.h"
CODE_TEMPLATE_PATH = "./templates/vgl_entrypoints_template.c"
IMPLEM_LIST = "./implemented.json"

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

    to_generate = []

    to_generate += [ (
        os.path.join(script_path, HEADER_TEMPLATE_PATH),
        os.path.join(args.outdir, 'vgl_entrypoints.h'),
    ) ]

    to_generate += [ (
        os.path.join(script_path, CODE_TEMPLATE_PATH),
        os.path.join(args.outdir, 'vgl_entrypoints.c'),
    ) ]

    function_list = os.path.join(script_path, IMPLEM_LIST)

    return code_gen.generate_from_templates(to_generate, function_list, args.xml_files)

if __name__ == '__main__':
    main()
