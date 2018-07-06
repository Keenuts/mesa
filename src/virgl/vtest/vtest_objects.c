             
#include <string.h>         
#include <vulkan/vulkan.h>  
                            
#include "common/macros.h" 
#include "virgl_vtest.h" 
#include "vtest_protocol.h" 
#include "vtest_objects.h"  

int vtest_create_descriptor_set_layout(int sock_fd,
	uint32_t handle,
	const VkDescriptorSetLayoutCreateInfo *create_info,
	uint32_t  *output)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_descriptor_set_layout_intro intro;
   struct payload_create_descriptor_set_layout_pBindings pBindings;
   
   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_DESCRIPTOR_LAYOUT, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));
   
   intro.handle = handle;
   intro.flags = create_info->flags;
   intro.bindingCount = create_info->bindingCount;
   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));
   
   for (uint32_t i = 0; i < create_info->bindingCount; i++) {
      pBindings.binding = create_info->pBindings[i].binding;
      pBindings.descriptorType = create_info->pBindings[i].descriptorType;
      pBindings.descriptorCount = create_info->pBindings[i].descriptorCount;
      pBindings.stageFlags = create_info->pBindings[i].stageFlags;
      res = virgl_block_write(sock_fd, &pBindings, sizeof(pBindings));
      CHECK_IO_RESULT(res, sizeof(pBindings));
   }
   
   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}


int vtest_create_buffer(int sock_fd,
	uint32_t handle,
	const VkBufferCreateInfo *create_info,
	uint32_t  *output)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_buffer_intro intro;
   
   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_BUFFER, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));
   
   intro.handle = handle;
   intro.flags = create_info->flags;
   intro.size = create_info->size;
   intro.usage = create_info->usage;
   intro.sharingMode = create_info->sharingMode;
   intro.queueFamilyIndexCount = create_info->queueFamilyIndexCount;
   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));
   
   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}

int vtest_allocate_descriptor_sets(int sock_fd,
	uint32_t handle,
	const VkDescriptorSetAllocateInfo *create_info,
   uint32_t *handles,
	uint32_t  *output)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_allocate_descriptor_sets_intro intro;
   
   INITIALIZE_HDR(cmd, VCMD_VK_ALLOCATE_DESCRIPTORS, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   intro.handle = handle;
   intro.descriptorPool = 0; //FIXME: better pools
   intro.descriptorSetCount = create_info->descriptorSetCount;
   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));
   
   res = virgl_block_write(sock_fd, handles, sizeof(*handles * intro.descriptorSetCount));
   CHECK_IO_RESULT(res, sizeof(sizeof(*handles * intro.descriptorSetCount)));
   
   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}

int vtest_create_shader_module(int sock_fd,
	uint32_t handle,
	const VkShaderModuleCreateInfo *create_info,
	uint32_t  *output)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_shader_module_intro intro;
   
   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_SHADER_MODULE, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));
   
   intro.handle = handle;
   intro.flags = create_info->flags;
   intro.codeSize = create_info->codeSize;

   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

   res = virgl_block_write(sock_fd, create_info->pCode, create_info->codeSize);
   CHECK_IO_RESULT(res, create_info->codeSize);
   
   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}
