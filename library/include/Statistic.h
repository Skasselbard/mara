//
// Created by jg on 1/3/17.
//

#include <stddef.h>
#include "predefined.h"

#ifndef MARA_STATISTIC_H
#define MARA_STATISTIC_H

struct stat_page {
    int pageNr;
    long usedMemory;
    byte * pointer;
    struct stat_page * next;
};

struct stat_page * firstPage;
struct stat_page * currentPage;

void statisticNewStatic(size_t size);
void statisticNewPage(byte * pointer);
void printPages();
double statisticAverageFillPercentage();

#endif //MARA_STATISTIC_H
