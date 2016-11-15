//
// Created by tom on 14.11.16.
//

#include "../include/PageList.h"

size_t PageList::blockSize= 104857600;//100mb
int PageList::setBlockSize(size_t sizeInByte) {
    blockSize = sizeInByte;
    return 0;
}
