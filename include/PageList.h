//
// Created by tom on 14.11.16.
//

#ifndef MARA_PAGELIST_H
#define MARA_PAGELIST_H

#include <stddef.h>

class PageList{
private:
    static size_t pageSize;

public:
    static int setPageSize(size_t sizeInByte);
    static size_t getPageSize();

};

#endif //MARA_PAGELIST_H
