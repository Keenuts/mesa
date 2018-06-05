#include <string.h>
#include <vulkan/vulkan.h>

#include "vgl_entrypoints.h"
#include "icd.h"

static uint32_t get_hash(const char *name)
{
    unsigned long h = 0;
    unsigned long g;

    while (*name) {
        h = (h << 4) + *name++;
        if ((g = h & 0xf0000000)) {
            h ^= g >> 24;
        }
        h &= ~g;
    }

    return h;
}

% for e in non_implemented:
    static void vgl_${e.name}(void)
    {
        fprintf(stderr, "ICD: called unimplemented '%s'. Aborting now.\n", __func__);
        abort();
    }
% endfor

const struct symbol __symtab[] = {
    % for e in implemented:
        { (PFN_vkVoidFunction)vgl_${e.name}, ${e.hash}, "${e.name}" },
    % endfor

    % for e in non_implemented:
        { vgl_${e.name}, ${e.hash}, "${e.name}" },
    % endfor
};

PFN_vkVoidFunction find_entrypoint(const char* name)
{
    const uint32_t ARRAY_SIZE = sizeof(__symtab) / sizeof(__symtab[0]);
    uint32_t hash = get_hash(name);

    for (uint32_t i = 0; i < ARRAY_SIZE; i++) {
        if (__symtab[i].name_hash != hash) {
            continue;
        }
        
        if (strcmp(__symtab[i].name, name) != 0) {
            continue;
        }

        return __symtab[i].pfn;
    };

    return NULL;
}
