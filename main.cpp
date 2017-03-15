#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include <thread>
#include "include/predefined.h"
#include "include/Mara.h"


unsigned int minSize = 4;
unsigned int maxSize = 4000;
unsigned int sizeSpan = 3996;
unsigned int requests = 10000;
unsigned int seed = 1234;

unsigned int numThreads = 1;
std::thread * threads;
Statistic * statistics;

//#define FILLSIMPLE

#ifdef FILLSIMPLE
uint8_t fillWith = 0xff;
#endif

void writeIntoBlock(unsigned long *address, size_t size) {
    uint32_t *firstByte = (uint32_t *) address;
#ifdef FILLSIMPLE
    for (int i = 0; i < size; i++) {
        *((byte *)firstByte + i) = fillWith;
    }
    fillWith = (uint8_t) ((fillWith == 0xff) ? 0x00 : 0xff);
#else
    for (unsigned int i = 0; i < size / sizeof(uint32_t); i++) {
        *(firstByte + i) = (uint32_t) size;
    }
#endif
}

int checkPages(Statistic * stat) {
    struct stat_page * p = stat->firstPage;
    while (p != 0) {
        byte * currentByte = p->pointer;
        while (currentByte < p->pointer + p->usedMemory) {
            uint32_t size = * (uint32_t *) currentByte;
            for (int i = 0; i < size / sizeof(uint32_t); i++) {
                if (* (uint32_t *) currentByte != size) {
                    return 0;
                }
                currentByte += sizeof(uint32_t);
            }
            currentByte += size % sizeof(uint32_t);
        }
        p = p->next;
    }
    return 1;
}

void * testingRoutine(Mara * mara) {
    for (int i = 1; i <= requests; i++) {
        unsigned int size = (rand() % sizeSpan) + minSize;
        unsigned long *address;
#ifdef USE_MARA
        address = (unsigned long *) mara->staticNew(size);
#else
        address = (unsigned long *) malloc(size);
#endif
        writeIntoBlock(address, size);
    }
}

void * threadTestingRoutine(unsigned int tid) {
    srand((tid + 1) * seed);
    Mara * mara = new Mara();
    testingRoutine(mara);
    statistics[tid] = *(mara->statistic);
}

void evaluateStatistics(double * averageFill, unsigned long * memoryRequested) {
    *averageFill = 0;
    *memoryRequested = 0;
    double fillPercentSum = 0;
    int pageCount = 0;
    for (int i = 0; i < numThreads; i++) {
        Statistic stat = statistics[i];
        struct stat_page *p = stat.firstPage;
        while (p != 0) {
            fillPercentSum += (double) p->usedMemory / DEFAULT_PAGE_SIZE;
            *memoryRequested = p->usedMemory;
            p = p->next;
            pageCount++;
        }
    }
    *averageFill = fillPercentSum / pageCount;
}

int checkAllPages() {
    int incompleteChecks = numThreads;
    for (int i = 0; i < numThreads; i++) {
        Statistic stat = statistics[i];
        incompleteChecks = incompleteChecks - checkPages(&stat);
    }
    return incompleteChecks;
}

// usage: mara test minSize maxSize requests seed
int main(int argc, char **argv) {

    if (argc >= 6 && strcmp(argv[1], "test") == 0) {

        sscanf(argv[2], "%u", &minSize);
        sscanf(argv[3], "%u", &maxSize);
        sscanf(argv[4], "%u", &requests);
        sscanf(argv[5], "%u", &seed);

        if (argc >= 7) {
            sscanf(argv[6], "%u", &numThreads);
        }

        sizeSpan = maxSize - minSize;

        if (sizeSpan == 0) sizeSpan = 1;

        srand(seed);

        clock_t begin = clock();
        clock_t end;

        if (numThreads > 1) {
            threads = new std::thread[numThreads];
            statistics = new Statistic[numThreads];
            int rc;

            for (unsigned int i = 0; i < numThreads; i++) {
                threads[i] = std::thread(threadTestingRoutine, i);
            }

            void * ret;
            for (unsigned int i = 0; i < numThreads; i++) {
                threads[i].join();
            }
            end = clock();
        } else {
            Mara * mara = new Mara();
            testingRoutine(mara);
            end = clock();
            statistics = new Statistic[1];
            statistics[0] = *(mara->statistic);
        }

        double timeSpent = (double) (end - begin) / CLOCKS_PER_SEC;

#ifdef USE_MARA
#ifdef STATISTIC
        double averageFill = 0;
        unsigned long memoryRequested = 0;
        evaluateStatistics(&averageFill, &memoryRequested);
#ifdef FILLSIMPLE
        printf("mara   %8u %12f %12lu %12f\n", seed, timeSpent, memoryRequested, averageFill);
#else
        printf("mara   %8u %12f %12lu %12f %5u\n", seed, timeSpent, memoryRequested, averageFill, checkAllPages());
#endif
#else
        printf("mara   %8u %12f\n", seed, timeSpent);
#endif
#else
        printf("malloc %8u %12f\n", seed, timeSpent);
#endif
    }
}

