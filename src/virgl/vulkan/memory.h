#ifndef MEMORY_H
#define MEMORY_H

void* vk_malloc(size_t size,
                const VkAllocationCallbacks * allocators,
                VkSystemAllocationScope scope);

void* vk_calloc(size_t size,
                const VkAllocationCallbacks * allocators,
                VkSystemAllocationScope scope);

#endif
