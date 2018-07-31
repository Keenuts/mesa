#ifndef VIRGL_ICD_H
#define VIRGL_ICD_H

/* to run an app using vtest:
 * VULKAN_DRIVER=virpipe ./myvulkanapp */
#define ENV_VULKAN_DRIVER "VULKAN_DRIVER"
#define VIRPIPE_DRIVERNAME "virpipe"

/* physical devices enumeration only shows a virtio v-gpu for now. */
#define REDHAT_VENDOR_ID 0x1af4
#define VIRTIOGPU_DEVICE_ID 0x1012

#define TO_HANDLE(Handle) (void*)(Handle)
#define FROM_HANDLE(Dst, Src) (__typeof__(Dst))(Src)

#include "vk_structs.h"
#include "util/list.h"

struct vk_physical_device_list {
   struct list_head list;
   struct vk_physical_device device;
};

struct icd_state {
   int available;
   int io_fd;


   struct vk_physical_device_list physical_devices;
};

extern struct icd_state icd_state;


PFN_vkVoidFunction find_entrypoint(const char *name);

VKAPI_ATTR VkResult VKAPI_CALL
vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t * pSupportedVersion);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName);

int initialize_physical_devices(void);

#endif
