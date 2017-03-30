#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include <thread>
#include "include/predefined.h"
#include "include/Mara.h"

/**
 * Runs a test for the mara library. Test parameters are: min/max requested size, amount of requests and seed for RNG
 * as well as number of threads. However, multithreaded testing is not recommended for performance, as for some reason
 * the tests get exponentially slower the more threads you use. (not a problem with our library though, malloc tests
 * become super slow as well)
 */

unsigned int minSize = 4;
unsigned int maxSize = 4000;
unsigned int sizeSpan = 3996;
unsigned int requests = 10000;
unsigned int seed = 1234;

unsigned int numThreads = 1;

// Arrays of threads and corresponding statistic objects.
std::thread * threads;
Statistic * statistics;

//#define FILLSIMPLE

#ifdef FILLSIMPLE
uint8_t fillWith = 0xff;
#endif

/**
 * Fills a block depending on the FILLSIMPLE flag.
 * If FILLSIMPLE is defined, the first requested block gets filled with 0xff, the second with 0x00, the third 0xff...
 * Else, the block is filled with its size (see writeIntoBlock), which allows for at least some form of consistency check.
 *
 * \param address the address of the block to be filled
 * \param size the size of the block to be filled
 */
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

/**
 * Checks all pages registered in a given statistic object for consistency.
 * Iterates over each page, reads the 4 bytes which should contain a size information X, then checks the next X bytes
 * if they contain the same value. Afterwards, the next 4 bytes specify the size of the next block. Rinse and repeat.
 *
 * \param stat
 * \return 0 if an error occured, 1 if everything went fine. SIGSEGV if somebody messed up.
 */
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

/**
 * Completes the specified number of requests to the given Mara object. After each request, the requested block is
 * filled using writeIntoBlock.
 *
 * @param mara the mara object
 * @return 0
 */
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
    return 0;
}

/**
 * This is called in every testing thread, if multithreading is to be used. Generates a new seed from thread id
 * and given seed (which is not particularly brilliant, but at least not all threads do the same, which was the goal).
 * After generating a new Mara object, inserts its statistic in the appropriate array, which is needed for checking later.
 *
 * \param tid the threads index in the thread array
 * \return 0
 */

void * threadTestingRoutine(unsigned int tid) {
    srand((tid + 1) * seed);
    Mara * mara = new Mara();
    testingRoutine(mara);
    statistics[tid] = *(mara->statistic);
    return 0;
}

/**
 * Evaluates all statistic objects. Collects information about average fill percentage and total memory requested.
 *
 * \param averageFill Output variable, contains the average fill percentage of all pages.
 * \param totalMemory Output variable, contains the total memory which is contained on the pages.
 */
void evaluateStatistics(double * averageFill, unsigned long * totalMemory) {
    *averageFill = 0;
    *totalMemory = 0;
    double fillPercentSum = 0;
    int pageCount = 0;
    for (int i = 0; i < numThreads; i++) {
        Statistic stat = statistics[i];
        struct stat_page *p = stat.firstPage;
        while (p != 0) {
            fillPercentSum += (double) p->usedMemory / DEFAULT_PAGE_SIZE;
            *totalMemory = *totalMemory + p->usedMemory;
            p = p->next;
            pageCount++;
        }
        //printf("fillPs=%f, memory=%lu, thread=%u\n", fillPercentSum, *totalMemory, i);
    }
    *averageFill = fillPercentSum / pageCount;
}

/**
 * Iterates over all statistic objects and checks its pages for consistency.
 *
 * \return the number of unsuccessful checks.
 */
int checkAllPages() {
    int incompleteChecks = numThreads;
    for (int i = 0; i < numThreads; i++) {
        Statistic stat = statistics[i];
        incompleteChecks = incompleteChecks - checkPages(&stat);
    }
    return incompleteChecks;
}

/**
 * usage: mara test minSize maxSize requests seed
 * Reads arguments into variables, then starts the test run and measures the time.
 * Prints results depending on enabled flags.
 *
 * \param argc amount of arguments
 * \param argv arguments
 * \return 0
 */
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
        return 0;
    }
}

