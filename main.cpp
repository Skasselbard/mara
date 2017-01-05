#include <iostream>
#include <cstring>
#include "include/Logger.h"
#include "include/Test.h"
#include "include/Page.h"
#include "include/interface.h"
#include "include/MemDump.h"
#include "include/PageList.h"
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
     /*
    for (unsigned int i = 0; i <= 12000; i++) {
        Test::writeIntoBlock((unsigned long *) dynamicNew(10000), 10000);
        Logger::debug(("Iteration: " + std::to_string(i)).c_str());
    }

    Test::checkPages();

    Logger::debug(("Pages before clear: " + std::to_string(PageList::getPageCount())).c_str());

    PageList::clearPages();

    Logger::debug(("Pages after clear: " + std::to_string(PageList::getPageCount())).c_str());

     */
    return 0;
}