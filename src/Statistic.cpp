// This file is part of mara.
//
// Copyright 2017
//
// Contributors: Tom Meyer, Julian Gaede, Marian Stein
//
// mara is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// mara is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with mara.  If not, see <http://www.gnu.org/licenses/>

#include <malloc.h>
#include "../include/Statistic.h"

    void Statistic::newStatic(size_t size) {
        if (__glibc_likely(&firstPage != 0)) {
            currentPage->usedMemory += size;
        } else {
            perror("Tried to register new static block before registering new page!");
        }
    }

    void Statistic::newPage(byte *pointer) {
        if (firstPage == 0) {
            firstPage = (stat_page *) malloc(sizeof(struct stat_page));
            firstPage->usedMemory = 0;
            firstPage->pageNr = 1;
            firstPage->pointer = pointer;
            currentPage = firstPage;
        } else {
            struct stat_page *newp = (stat_page *) malloc(sizeof(struct stat_page));
            newp->pageNr = currentPage->pageNr + 1;
            newp->usedMemory = 0;
            newp->pointer = pointer;
            currentPage->next = newp;
            currentPage = newp;
        }
    }

    void Statistic::printPages() {
        struct stat_page *p = firstPage;
        while (p != 0) {
            printf("%4u %lu\n", p->pageNr, p->usedMemory);
            p = p->next;
        }
    }

    double Statistic::averageFillPercentage() {
        struct stat_page *p = firstPage;
        int pageCount = 0;
        double fillPercentSum = 0.0;
        while (p != 0) {
            fillPercentSum += (double) p->usedMemory / DEFAULT_PAGE_SIZE;
            p = p->next;
            pageCount++;
        }
        return fillPercentSum / pageCount;
    }