//
// Created by jg on 1/3/17.
//

#include <stddef.h>
#include "predefined.h"

#ifndef MARA_STATISTIC_H
#define MARA_STATISTIC_H

/**
 * A page for the statistic. Contains a number, the pointer to the actual page and keeps track of used memory
 * on this page. Organized as a linked list.
 */
struct stat_page {
    int pageNr;
    unsigned long usedMemory;
    byte *pointer;
    struct stat_page *next;
};

/**
 * Keeps track of pages and memory requests.
 */
class Statistic {

public:

    /**
     * The first known page.
     */
    stat_page *firstPage = 0;

    /**
     * The currently active page. Is changed whenever newPage is called.
     */
    stat_page *currentPage = 0;

    /**
     * Is called whenever new static memory is requested. Adds the requested size to the memory value of the current page.
     * \param size the requested size.
     */
    void newStatic(size_t size);

    /**
     * Is called whenever a new page is created. Adjusts the linked list accordingly and sets currentPage to the new page.
     * \param pointer the pointer to the new page.
     */
    void newPage(byte *pointer);

    /**
     * Prints the memory value of each page.
     */
    void printPages();

    /**
     * Calculates the average fill percentage of all pages.
     *
     * \return the average fill percentage of all pages.
     */
    double averageFillPercentage();

};
#endif //MARA_STATISTIC_H
