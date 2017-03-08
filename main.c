#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include "include/predefined.h"
#include "include/interface.h"
#include "include/Statistic.h"

unsigned int minSize = 4;
unsigned int maxSize = 4000;
unsigned int sizeSpan = 3996;
unsigned int requests = 10000;
unsigned int seed = 1234;

void writeIntoBlock(unsigned long *address, size_t size) {
    for (unsigned int i = 0; i < size / 8; i++) {
        *(address + i) = (unsigned long) size;
    }
}

// usage: mara test minSize maxSize requests seed
main(int argc, char **argv) {
    if (argc >= 6 && strcmp(argv[1], "test") == 0) {

        sscanf(argv[2], "%u", &minSize);
        sscanf(argv[3], "%u", &maxSize);
        sscanf(argv[4], "%u", &requests);
        sscanf(argv[5], "%u", &seed);
        
        sizeSpan = maxSize - minSize;
        
        srand(seed);

        clock_t begin = clock();

        for (int i = 1; i <= requests; i++) {
            unsigned int size = (rand() % sizeSpan) + 1;
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
        printf("mara   %u %15f %12f\n", seed, timeSpent, statisticAverageFillPercentage());
#else
        printf("mara   %u %15f\n", seed, timeSpent);
#endif
#else
        printf("malloc %u %f\n", seed, timeSpent);
#endif
    }
}

