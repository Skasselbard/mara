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