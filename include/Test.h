//
// Created by jg on 29.11.16.
//

#ifndef MARA_TEST_H
#define MARA_TEST_H

class Test {

private:

public:

    static const unsigned int DEFAULT_ITERATIONS   = 1;
    static const unsigned int DEFAULT_VARIABLES    = 50000;
    static const double       DEFAULT_P_DYNAMIC;
    static const double       DEFAULT_P_FREE;
    static const unsigned int DEFAULT_MIN_SIZE     = 4;
    static const unsigned int DEFAULT_AVERAGE_SIZE = 16;
    static const unsigned int DEFAULT_MAX_SIZE     = 1000;
    static const unsigned int DEFAULT_SEED         = 123456789;

    static int test(int argc, char** argv);
    static int distributionTest();
    static int testBucketList();
    static void readArguments(int argc, char** argv, unsigned int * amountNewVariables,
                              double * pDynamic, double * pFree,
                              unsigned int * minSize, unsigned int * averageSize, unsigned int * maxSize,
                              unsigned int * maxIterations, unsigned int * seed);
};

#endif //MARA_TEST_H
