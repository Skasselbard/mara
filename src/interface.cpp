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
    return PageList::staticNew(sizeInByte);
}

void *dynamicNew(size_t sizeInByte) {
    return PageList::dynamicNew(sizeInByte);
}

int dynamicDelete(void *address) {
    return PageList::dynamicDelete(address);
}
