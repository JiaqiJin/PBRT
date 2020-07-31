//
//  memory.cpp
//  Paladin
//
//  Created by SATAN_Z on 2019/7/17.
//  Copyright © 2019 Zero. All rights reserved.
//

#include "memory.hpp"
#include <stdlib.h>
KAWAII_BEGIN

/*
对齐分配内存
 */
 /*
 对齐分配内存
  */
    void* allocAligned(size_t size) {
#if defined(KAWAII_HAVE_ALIGNED_MALLOC)
    return _aligned_malloc(size, KAWAII_L1_CACHE_LINE_SIZE);
#elif defined(KAWAII_HAVE_ALIGNED_MALLOC)
    void* ptr;
    if (posix_memalign(&ptr, PALADIN_L1_CACHE_LINE_SIZE, size) != 0) {
        ptr = nullptr;
    }
    return ptr;
#else
    return memalign(KAWAII_L1_CACHE_LINE_SIZE, size);
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


KAWAII_END