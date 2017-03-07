#include <stdio.h>
#include <memory.h>
#include <time.h>

// usage: mara test minSize maxSize requests seed
main(int argc, char **argv) {
    if (argc >= 6 && strcmp(argv[1], "test") == 0) {
        unsigned int minSize = 4;
        unsigned int maxSize = 4000;
        unsigned int requests = 10000;
        unsigned int seed = 1234;
        sscanf(argv[2], "%u", &minSize);
        sscanf(argv[3], "%u", &maxSize);
        sscanf(argv[4], "%u", &requests);
        sscanf(argv[5], "%u", &seed);

        std::mt19937 generator(seed);
        std::uniform_int_distribution<unsigned int> size_distr(minSize, maxSize);

        clock_t begin = clock();

        for (int i = 1; i <= requests; i++) {
            unsigned int size = size_distr(generator);
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
        printf("mara   %u %f", seed, timeSpent);
#else
        printf("malloc %u %f", seed, timeSpent);
#endif
    }
}

void writeIntoBlock(unsigned long *address, size_t size) {
    for (unsigned int i = 0; i < size / 8; i++) {
        *(address + i) = size;
    }
}