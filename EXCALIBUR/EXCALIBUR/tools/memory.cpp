#include "memory.h"
#include <stdlib.h>

#if defined(_MSC_VER)
#define RENDERING_HAVE_ALIGNED_MALLOC
#endif

#define RENDERING_HAVE_POSIX_MEMALIGN

namespace Rendering
{
    void* allocAligned(size_t size) {
#if defined(RENDERING_HAVE_ALIGNED_MALLOC)
        return _aligned_malloc(size, RENDERING_L1_CACHE_LINE_SIZE);
#elif defined(RENDERING_HAVE_POSIX_MEMALIGN)
        void* ptr;
        if (posix_memalign(&ptr, RENDERING_L1_CACHE_LINE_SIZE, size) != 0) {
            ptr = nullptr;
        }
        return ptr;
#else
        return memalign(RENDERING_L1_CACHE_LINE_SIZE, size);
#endif
    }

    void freeAligned(void* ptr) {
        if (!ptr) {
            return;
        }
#if defined(PALADIN_HAVE_ALIGNED_MALLOC)
        _aligned_free(ptr);
#else
        free(ptr);
#endif
    }
}