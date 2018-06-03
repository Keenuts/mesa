import argparse
import json
import os

from mako.template import Template

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--out', help='output json filename.', required=True)
    parser.add_argument('--lib-path', help='path to libvulkan_*.so.', required=True)
    args = parser.parse_args()

    json_data = {
        'file_format_version': '1.0.0',
        'ICD': {
            'library_path': os.path.join(args.lib_path, "libvulkan_virgl.so"),
            'api_version': "1.1.0",
        },
    };

    with open(args.out, 'w') as f:
        json.dump(json_data, f, indent=4, sort_keys=True)
    
    return 0

if __name__ == '__main__':
    main()
