#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "memory.h"

void* vk_malloc(size_t size,
                const VkAllocationCallbacks * allocators,
                VkSystemAllocationScope scope)
{
   do {
      if (allocators == NULL) {
         break;
      }

      if (allocators->pfnAllocation != NULL) {
         return allocators->pfnAllocation(allocators->pUserData, size, 2, scope);
      }

      /* TODO: add other cases here */
   } while (0);


   return malloc(size);
}

void* vk_calloc(size_t size,
                const VkAllocationCallbacks * allocators,
                VkSystemAllocationScope scope)
{
   void *ptr;

   ptr = vk_malloc(size, allocators, scope);
   if (ptr == NULL) {
      return NULL;
   }

   memset(ptr, 0, size);
   return ptr;
}
