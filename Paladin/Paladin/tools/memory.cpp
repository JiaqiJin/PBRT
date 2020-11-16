#include "memory.hpp"
#include <stdlib.h>
PALADIN_BEGIN

/*
对齐分配内存
 */
void *allocAligned(size_t size) {
#if defined(PALADIN_HAVE_ALIGNED_MALLOC)
    return _aligned_malloc(size, PALADIN_L1_CACHE_LINE_SIZE);
#elif defined(PALADIN_HAVE_POSIX_MEMALIGN)
    void * ptr;
    if (posix_memalign(&ptr, PALADIN_L1_CACHE_LINE_SIZE, size) != 0) {
    	ptr = nullptr;
    }
    return ptr;
#else
    return memalign(PALADIN_L1_CACHE_LINE_SIZE, size);
#endif
}

void freeAligned(void *ptr) {
    if (!ptr) {	
		return;
    } 
#if defined(PALADIN_HAVE_ALIGNED_MALLOC)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

PALADIN_END
