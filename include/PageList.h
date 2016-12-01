//
// Created by tom on 14.11.16.
//

#ifndef MARA_PAGELIST_H
#define MARA_PAGELIST_H

#include <stddef.h>

class PageList{
private:
    static size_t blockSize;

public:
    static int setBlockSize(size_t sizeInByte);

};

#endif //MARA_PAGELIST_H
