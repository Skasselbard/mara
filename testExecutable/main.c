#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include "../library/include/predefined.h"
#include "../library/include/mara.h"
#include "../library/include/Statistic.h"

unsigned int minSize = 4;
unsigned int maxSize = 4000;
unsigned int sizeSpan = 3996;
unsigned int requests = 10000;
unsigned int seed = 1234;

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

int checkPages() {
    struct stat_page * p = firstPage;
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

// usage: mara test minSize maxSize requests seed
int main(int argc, char **argv) {
    if (argc >= 6 && strcmp(argv[1], "test") == 0) {

        sscanf(argv[2], "%u", &minSize);
        sscanf(argv[3], "%u", &maxSize);
        sscanf(argv[4], "%u", &requests);
        sscanf(argv[5], "%u", &seed);
        
        sizeSpan = maxSize - minSize;
        
        srand(seed);

        clock_t begin = clock();

        for (int i = 1; i <= requests; i++) {
            unsigned int size = (rand() % sizeSpan) + minSize;
            unsigned long *address;
#ifdef USE_MARA
            address = (unsigned long *) staticNew(size);
#else
            address = (unsigned long *) malloc(size);
#endif
            writeIntoBlock(address, size);

        }

        clock_t end = clock();

        double timeSpent = (double) (end - begin) / CLOCKS_PER_SEC;

#ifdef USE_MARA
#ifdef STATISTIC
#ifdef FILLSIMPLE
        printf("mara   %8u %12f %12f\n", seed, timeSpent, statisticAverageFillPercentage());
#else
        printf("mara   %8u %12f %12f %5u\n", seed, timeSpent, statisticAverageFillPercentage(), checkPages());
#endif
#else
        printf("mara   %8u %12f\n", seed, timeSpent);
#endif
#else
        printf("malloc %8u %12f\n", seed, timeSpent);
#endif
    }
}

