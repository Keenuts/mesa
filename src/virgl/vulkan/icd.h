#ifndef VIRGL_ICD_H
#define VIRGL_ICD_H

#define REDHAT_VENDOR_ID 0x1af4
#define VIRTIOGPU_DEVICE_ID 0x1012
#define ENV_VULKAN_DRIVER "VULKAN_DRIVER"
#define VIRPIPE_DRIVERNAME "virpipe"

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
#define TO_HANDLE(Handle) (void*)(Handle)

extern int handle_dummy;
#define DUMMY_HANDLE (void*)(&handle_dummy)

struct symbol
{
   PFN_vkVoidFunction pfn;
   uint32_t name_hash;
   const char *name;
};

struct vk_api_version
{
   union
   {
      struct
      {
         uint32_t major:10;
         uint32_t minor:10;
         uint32_t patch:12;
      };
      uint32_t raw;
   };
} __attribute__ ((__packed__));


extern const struct symbol __symtab[];
extern struct virgl_vtest vtest_context;


PFN_vkVoidFunction find_entrypoint(const char *name);

VKAPI_ATTR VkResult VKAPI_CALL
vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t * pSupportedVersion);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName);

#endif
