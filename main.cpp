#include <iostream>
#include <cstring>
#include "include/Logger.h"
#include "include/Test.h"
#include "include/Page.h"
#include "include/interface.h"
#include "include/MemDump.h"
#include <stdlib.h>

void writeArray(char* array, size_t size, char content){
    for (int i = 0; i<size; i++){
        array[i] = content;
    }
}

void printArray(char* array, size_t size){
    for (int i = 0; i<size; i++){
        printf("%c",array[i]);
    }
}

int main(int argc, char** argv) {

    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        Test::test(argc, argv);
    }

    return 0;
}