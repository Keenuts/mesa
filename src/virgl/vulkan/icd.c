#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "icd.h"
#include "common/macros.h"
#include "vgl_entrypoints.h"
#include "vtest/virgl_vtest.h"

/* state of our ICD. */
struct icd_state icd_state;


PUBLIC VKAPI_ATTR VkResult VKAPI_CALL
vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t * pSupportedVersion)
{
   TRACE_IN();

   int sock_fd;
   const char *vulkan_driver = NULL;

   printf("ICD: requested version: %u\n", *pSupportedVersion);
   if (*pSupportedVersion != 5) {
      fprintf(stderr, "vulkan implementation only supports loader interface 5\n");
      RETURN(VK_ERROR_INCOMPATIBLE_DRIVER);
   }

   memset(&icd_state, 0, sizeof(icd_state));

   /* note about the current vtest handling
    *
    * Now, I only consider vtest. So call will be made directly in ICD's funcions.
    * What should be done is to init a struct here containing callbacks
    * with the backend's API.
    * Thus, we only need to init it here with either vtest's callbacks, or
    * a real backend.
    */
   do {
      vulkan_driver = getenv(ENV_VULKAN_DRIVER);
      if (vulkan_driver == NULL) {
         break;
      }

      if (strncmp
          (vulkan_driver, VIRPIPE_DRIVERNAME,
           sizeof(VIRPIPE_DRIVERNAME)) != 0) {
         break;
      }

      sock_fd = virgl_vtest_connect();
      if (sock_fd < 0) {
         fprintf(stderr,
                 "connection to virglrenderer's vtest server failed.\n");
         RETURN(VK_ERROR_DEVICE_LOST);
      }

      icd_state.available = 1;
      icd_state.io_fd = sock_fd;
      RETURN(VK_SUCCESS);
   } while (0);

   fprintf(stderr,
           "non vtest-based vulkan driver not implemented for now.\n");
   abort();
   RETURN(VK_SUCCESS);
}

PUBLIC VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName)
{
   UNUSED_PARAMETER(instance);

   /* The loader should negociate with our driver first. Otherwise, something
    * went wrong */
   if (icd_state.available == 0) {
      return NULL;
   }

   PFN_vkVoidFunction func = NULL;

   func = find_entrypoint(pName);
   if (func == NULL) {
      fprintf(stderr, "ICD: cannot find the function %s\n", pName);
      return NULL;
   }

   return func;
};

PFN_vkVoidFunction
vgl_vkGetDeviceProcAddr(VkDevice device,
                        const char* name)
{
   UNUSED_PARAMETER(device);
   return vk_icdGetInstanceProcAddr(VK_NULL_HANDLE, name);
}
