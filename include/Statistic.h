//
// Created by jg on 1/3/17.
//

#include <stddef.h>

#ifndef MARA_STATISTIC_H
#define MARA_STATISTIC_H

struct stat_page {
    int pageNr;
    long usedMemory;
    struct stat_page * next;
};

void statisticNewStatic(size_t size);
void statisticNewPage();
void printPages();
double statisticAverageFillPercentage();

#endif //MARA_STATISTIC_H
