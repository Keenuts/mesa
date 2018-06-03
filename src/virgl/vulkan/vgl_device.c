#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "vgl_entrypoints.h"

#ifdef DEBUG
    #define TRACE_IN() fprintf(stderr, "--> %s\n", __func__)
    #define TRACE_OUT(...) \
        do {    \
            char *fmt = "<-- %s (%d)\n"; \
            fprintf(stderr, fmt, __func__ __VA_OPT__(,) __VA_ARGS__ , 0); \
        } while (0)
#else
    #define TRACE_IN()
    #define TRACE_OUT(...)
#endif

#define UNUSED_PARAMETER(Param) (void)(Param)
#define RETURN(...)         \
    TRACE_OUT(__VA_ARGS__); \
    return __VA_ARGS__

VkResult vgl_vkCreateInstance(
    const VkInstanceCreateInfo *create_info,
    const VkAllocationCallbacks *allocator,
    VkInstance *instance)
{
    TRACE_IN();
    (void)create_info;
    (void)allocator;
    (void)instance;

    *instance = malloc(sizeof(int));

    RETURN(VK_SUCCESS);
}

void vgl_vkDestroyInstance(
    VkInstance instance,
    const VkAllocationCallbacks *allocators)
{
    TRACE_IN();

    (void)allocators;
    free(instance);
    
    RETURN();
}

VkResult vgl_vkEnumerateInstanceExtensionProperties(
    const char *layer_name,
    uint32_t *property_count,
    VkExtensionProperties *properties)
{
    TRACE_IN();
    (void)layer_name;
    (void)properties;

    *property_count = 0;

    RETURN(VK_SUCCESS);
}

struct vk_api_version {
    union {
        struct {
            uint32_t major : 10;
            uint32_t minor : 10;
            uint32_t patch : 12;
        };
        uint32_t raw;
    };
} __attribute__((__packed__));

VkResult vgl_vkEnumerateInstanceVersion(
    uint32_t *pApiVersion)
{
    TRACE_IN();

    struct vk_api_version version;
    version.major = 1;
    version.minor = 1;
    version.patch = 0;

    *pApiVersion = version.raw;
    RETURN(VK_SUCCESS);
}

VkResult vgl_vkEnumeratePhysicalDevices(
    VkInstance instance,
    uint32_t *device_count,
    VkPhysicalDevice *physical_devices)
{
    TRACE_IN();
    (void)instance;

    if (physical_devices == NULL) {
        *device_count = 1;
        RETURN(VK_SUCCESS);
    }

    *physical_devices = malloc(sizeof(int));

    RETURN(VK_SUCCESS);
}
