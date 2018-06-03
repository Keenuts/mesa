#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

VKAPI_ATTR VkResult VKAPI_CALL
vk_icdNegotiateLoaderICDInterfaceVersion(
    uint32_t* pSupportedVersion
);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(
    VkInstance instance,
    const char* pName
);


% for e in implemented:
${e.ret_value} vgl_${e.name}(
    % for p in e.params:
        % if loop.last:
            ${p.decl}
        % else:
            ${p.decl},
        % endif
    % endfor
);
% endfor
