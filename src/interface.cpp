//
// Created by tom on 14.11.16.
//

#include <stddef.h>
#include "../include/interface.h"
#include "../include/PageList.h"

PageList pagelist;

int setDefaultBlockSize(size_t sizeInByte) {
    return 0;
}

void *staticNew(size_t sizeInByte) {
    return NULL;
}

void *dynamicNew(size_t sizeInByte) {
    return NULL;
}

int dynamicDelete(void *address) {
    return 0;
}
