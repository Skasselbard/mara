//
// Created by jg on 1/3/17.
//

#include <stddef.h>
#include "predefined.h"

#ifndef MARA_STATISTIC_H
#define MARA_STATISTIC_H

struct stat_page {
    int pageNr;
    unsigned long usedMemory;
    byte *pointer;
    struct stat_page *next;
};

class Statistic {

public:

    stat_page *firstPage = 0;
    stat_page *currentPage = 0;

    void newStatic(size_t size);

    void newPage(byte *pointer);

    void printPages();

    double averageFillPercentage();

};
#endif //MARA_STATISTIC_H
