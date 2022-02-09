#include "Extentions.hpp"

#include <GL/glext.h>

GLint glxGpuTotalMemory() {
    GLint totalMemKb = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &totalMemKb);

#ifdef _WIN32
    if (totalMemKb == 0 && glGetGPUIDsAMD) {
        UINT n = wglGetGPUIDsAMD(0, 0);
        UINT *ids = new UINT[n];
        size_t totalMemMb = 0;
        wglGetGPUIDsAMD(n, ids);
        wglGetGPUInfoAMD(ids[0], WGL_GPU_RAM_AMD, GL_UNSIGNED_INT, sizeof(size_t), &totalMemMb);
        totalMemKb = totalMemMb * 1024;
    }
#endif

    return totalMemKb;
}

GLint glxGpuAvailMemory() {
    GLint curAvailMemKb = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &curAvailMemKb);

    if (curAvailMemKb == 0) {
        glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &curAvailMemKb);
    }

    return curAvailMemKb;
}