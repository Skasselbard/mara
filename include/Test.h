//
// Created by jg on 29.11.16.
//

#ifndef MARA_TEST_H
#define MARA_TEST_H

#define FILL_REQUESTED_MEMORY 2
//-1 = don't fill at all
// 0 = fill with zeros
// 1 = fill with ones
// 2 = fill with the address of the start of the memory array

class Test {

private:

    /**
     * Number of FreeSpaces which can not be found in the BucketList ( => memory leaks)
     */
    static unsigned int freeSpaceNotInBucketList;

    /**
     * Number of blocks, which do not contain the original content.
     */
    static unsigned int corruptedBlocks;
public:

    // Default values for test parameters
    static const unsigned int DEFAULT_ITERATIONS   = 1;
    static const unsigned int DEFAULT_VARIABLES    = 50000;
    static const double       DEFAULT_P_DYNAMIC;
    static const double       DEFAULT_P_FREE;
    static const unsigned int DEFAULT_MIN_SIZE     = 4;
    static const unsigned int DEFAULT_AVERAGE_SIZE = 16;
    static const unsigned int DEFAULT_MAX_SIZE     = 1000;
    static const unsigned int DEFAULT_SEED         = 123456789;

    /**
     * Runs a test with the arguments given. Customizable parameters are amount of requests, probability to request
     * dynamic memory, probability to free a dynamic block after each (dynamic AND static) request, size information
     * about the requested blocks (min, max, [ average is currently ignored ]), iterations (after each iteration, all
     * pages are deleted and another n requests are made), seed for the RNG.
     *
     * Syntax: {@code test amountNewVariables pDynamic pFree minSize averageSize maxSize iterations seed}
     *
     * After a request, the received block is filled with values depending on FILL_REQUESTED_MEMORY. Before freeing a
     * block, it is filled with zeroes to make debugging easier.
     *
     * The time for each run is measured. After a run, a consistency check is performed.
     * After completion, prints information about the test in the following order:
     *
     * type seed time dynamicMemoryPeak dynamicBlocksPeak staticMemoryPeak staticBlockPeak corruptedBlocks freeSpaceNotInBL
     *
     * \param argc amount of program arguments
     * \param argv the program arguments
     * \return 0
     */
    static int test(int argc, char** argv);

    /**
     * Reads the command line arguments into the given variables.
     */
    static void readArguments(int argc, char** argv, unsigned int * amountNewVariables,
                              double * pDynamic, double * pFree,
                              unsigned int * minSize, unsigned int * averageSize, unsigned int * maxSize,
                              unsigned int * maxIterations, unsigned int * seed);

    /**
     * Writes values into the block at the given address of the given size. Content depends on the of the
     * FILL_REQUESTED_MEMORY macro. If the block is to be filled with its starting address, any rest of 7 bytes or
     * smaller is ignored.
     *
     * @param address the block's starting address
     * @param size the block's size
     */
    static void writeIntoBlock(unsigned long * address, size_t size);

    /**
     * Checks the pages for consistency by iterating over them. At each page, reads the first block's size and status
     * from the CodeBlock at the start of the page.
     * If the block is free, tries to find it in the BucketList of the page to ensure that it is still allocatable.
     * If the block is not free, checks it for consistency by checking if the content corresponds to what writeIntoBlock
     * had written into it after requesting it.
     *
     * Errors are counted using the variables "freeSpaceNotInBucketList" and "corruptedBlocks".
     *
     * \return 0
     */
    static int checkPages();
};

#endif //MARA_TEST_H
