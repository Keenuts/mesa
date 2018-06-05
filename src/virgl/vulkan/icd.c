#include <stdio.h>
#include <vulkan/vulkan.h>

#include "icd.h"
#include "util/macros.h"
#include "vtest/virgl_vtest.h"

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

    PFN_vkVoidFunction func = NULL;

    func = find_entrypoint(pName);
    if (func == NULL) {
        fprintf(stderr, "ICD: cannot find the function %s\n", pName);
        return NULL;
    }

    return func;
};
