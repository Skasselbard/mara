//
// Created by tom on 14.11.16.
//

#include <stddef.h>
#include "../include/interface.h"
#include "../include/PageList.h"

int setDefaultBlockSize(size_t sizeInByte) {
    return PageList::setPageSize(sizeInByte);
}

void *staticNew(size_t sizeInByte) {
#ifdef USE_MARA
    return PageList::staticNew(sizeInByte);
#else
    return malloc(sizeInByte);
#endif
}

void *dynamicNew(size_t sizeInByte) {
#ifdef USE_MARA
    return PageList::dynamicNew(sizeInByte);
#else
    return malloc(sizeInByte);
#endif
}

int dynamicDelete(void *address) {
#ifdef USE_MARA
    return PageList::dynamicDelete(address);
#else
    free(address);
    return 0;
#endif
}
