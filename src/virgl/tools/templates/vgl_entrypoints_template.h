#ifndef VGL_ENTRYPOINT_H
#define VGL_ENTRYPOINT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

% for e in listed:
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

#endif
