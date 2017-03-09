//
// Created by tom on 14.11.16.
//

#ifndef MARA_PAGE_LIST_H
#define MARA_PAGE_LIST_H

#include <stddef.h>
#include "Page.h"

class PageList{
private:
    static size_t pageSize;
    /**
     * the first page in the ring that will be searched
     */
    static Page *firstPage;

    /**
     * Inserts a new page into cyclic list after the specified page.
     * @param currentPage the page after which a new page will be inserted
     * @return {@code true} if a new page was successfully inserted.
     */
    static bool addPageToList(Page *currentPage);
    static bool iteratePage(Page *&currentPage);

public:
    static int setPageSize(size_t sizeInByte);
    static size_t getPageSize();
    static Page* getFirstPage();

    static void* staticNew(size_t sizeInByte);
    static void* dynamicNew(size_t sizeInByte);
    static bool dynamicDelete(void *address);

    static unsigned int getPageCount();

    static void clearPages();

    static FreeSpace* foo;//TODO: Delete

};

#endif //MARA_PAGE_LIST_H
