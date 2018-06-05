#ifndef VIRGL_ICD_H
#define VIRGL_ICD_H

struct symbol {
    PFN_vkVoidFunction pfn;
    uint32_t name_hash;
    const char *name;
};

PFN_vkVoidFunction find_entrypoint(const char* name);

VKAPI_ATTR VkResult VKAPI_CALL
vk_icdNegotiateLoaderICDInterfaceVersion(
    uint32_t* pSupportedVersion
);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(
    VkInstance instance,
    const char* pName
);



extern const struct symbol __symtab[];
#endif
