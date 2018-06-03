#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "util/macros.h"
#include "vgl_entrypoints.h"

struct symbol {
    PFN_vkVoidFunction pfn;
    uint32_t name_hash;
    const char *name;
};

% for e in non_implemented:
    static void vgl_${e.name}(void)
    {
        fprintf(stderr, "ICD: called unimplemented '%s'. Aborting now.\n", __func__);
        abort();
    }
% endfor

static struct symbol __symtab[] = {
    % for e in implemented:
        { (PFN_vkVoidFunction)vgl_${e.name}, ${e.hash}, "${e.name}" },
    % endfor

    % for e in non_implemented:
        { vgl_${e.name}, ${e.hash}, "${e.name}" },
    % endfor
};

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

static void unimplemented_function()
{
    fprintf(stderr, "ICD: unimplemented function called. Aborting now.\n");
    abort();
}

PUBLIC VKAPI_ATTR VkResult VKAPI_CALL
vk_icdNegotiateLoaderICDInterfaceVersion(
    uint32_t* pSupportedVersion)
{
    printf("ICD: requested version: %u\n", *pSupportedVersion);
    return VK_SUCCESS;
}

PUBLIC VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(
    VkInstance instance,
    const char* pName)
{
    (void)instance;

    const uint32_t ARRAY_SIZE = sizeof(__symtab) / sizeof(__symtab[0]);
    uint32_t hash = get_hash(pName);

    for (uint32_t i = 0; i < ARRAY_SIZE; i++) {
        if (__symtab[i].name_hash != hash) {
            continue;
        }
        
        if (strcmp(__symtab[i].name, pName) != 0) {
            continue;
        }

        return __symtab[i].pfn;
    };

    fprintf(stderr, "ICD: cannot find the function %s\n", pName);
    return unimplemented_function;
};
