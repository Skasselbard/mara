#include <cstring>
#include "include/Test.h"

int main(int argc, char** argv) {


    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        Test::test(argc, argv);
    }

    return 0;
}