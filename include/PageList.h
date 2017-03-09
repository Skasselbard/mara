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

    /**
     * rotates the cyclic list one step. if it has reached the end it adds the page to the list
     * @param currentPage th current page
     * @return if the addition was successful
     */
    static bool iteratePage(Page *&currentPage);

public:
    /**
     * sets the page size
     * @param sizeInByte the page size
     * @return 0
     */
    static int setPageSize(size_t sizeInByte);

    /**
     *
     * @return the page size
     */
    static size_t getPageSize();

    /**
     *
     * @return the first page
     */
    static Page* getFirstPage();

    /**
     * reserves a new static block
     * @param sizeInByte size of the block
     * @return a pointer to the block
     */
    static void* staticNew(size_t sizeInByte);

    /**
     * reserves a new dynamic block
     * @param sizeInByte size of the block
     * @return a pointer to the block
     */
    static void* dynamicNew(size_t sizeInByte);

    /**
     * frees a dynamic block
     * @param address a pointer to the block
     * @return if it was successful
     */
    static bool dynamicDelete(void *address);

    /**
     *
     * @return how many pages are in the cyclic list
     */
    static unsigned int getPageCount();

    /**
     * deletes all pages, not used
     */
    static void clearPages();

};

#endif //MARA_PAGE_LIST_H
