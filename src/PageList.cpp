//
// Created by tom on 14.11.16.
//

#include "../include/PageList.h"

size_t PageList::pageSize= 104857600;//100mb
int PageList::setPageSize(size_t sizeInByte) {
    pageSize = sizeInByte;
    return 0;
}

size_t PageList::getPageSize() {
    return pageSize;
}
