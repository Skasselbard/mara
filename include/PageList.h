//
// Created by tom on 14.11.16.
//

#ifndef MARA_PAGELIST_H
#define MARA_PAGELIST_H

#include <stddef.h>
#include "Page.h"

class PageList{
private:
    static size_t pageSize;
    static Page *firstPage;

    static bool addPageToList(Page *currentPage);
    static bool iteratePage(Page *&currentPage);

public:
    static int setPageSize(size_t sizeInByte);
    static size_t getPageSize();

    static void* staticNew(size_t sizeInByte);
    static void* dynamicNew(size_t sizeInByte);
    static bool dynamicDelete(void *address);

};

#endif //MARA_PAGELIST_H
