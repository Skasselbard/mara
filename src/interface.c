//
// Created by tom on 14.11.16.
//

#include <stddef.h>
#include <malloc.h>
#include "../include/interface.h"
#include "../include/Page.h"

int setPageSize(size_t sizeInByte) {
    pageSize = sizeInByte;
}

void *staticNew(size_t sizeInByte) {
#ifdef USE_MARA
    return allocateStatic(sizeInByte)
#else
    return malloc(sizeInByte);
#endif
}
