#ifndef VIRGL_ICD_H
#define VIRGL_ICD_H

/* to run an app using vtest:
 * VULKAN_DRIVER=virpipe ./myvulkanapp */
#define ENV_VULKAN_DRIVER "VULKAN_DRIVER"
#define VIRPIPE_DRIVERNAME "virpipe"

/* physical devices enumeration only shows a virtio v-gpu for now. */
#define REDHAT_VENDOR_ID 0x1af4
#define VIRTIOGPU_DEVICE_ID 0x1012

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
#define FROM_HANDLE(Dst, Src) (__typeof__(Dst))(Src)


/* Dummy handle to give when we do not have anything useful to give */
static const int handle_dummy;
#define DUMMY_HANDLE (void*)(&handle_dummy)


struct icd_state {
   int available;
   int io_fd;
};

extern struct icd_state icd_state;


PFN_vkVoidFunction find_entrypoint(const char *name);

VKAPI_ATTR VkResult VKAPI_CALL
vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t * pSupportedVersion);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName);

#endif
