#include <iostream>
#include "include/Logger.h"
#include "include/interface.h"
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

int main() {
    Logger::info("some info");
    Logger::warning("some warning");
    Logger::error("some error");
    Logger::debug("some debug message");
    //Logger::fatal("fatal", 129);

    char* hundert;
    char* funfzig;
    char* funfhundert;

    for (int i = 0; i<100; i++){
        hundert = (char*)dynamicNew(100* sizeof(char));
        funfzig = (char*)dynamicNew(50* sizeof(char));
        funfhundert = (char*)dynamicNew(500* sizeof(char));
        writeArray(hundert, 100, 'a');
        writeArray(funfzig, 50, 'b');
        writeArray(funfhundert, 500, 'c');
        printArray(hundert, 100);
        printf("\n");
        printArray(funfzig, 50);
        printf("\n");
        printArray(funfhundert, 500);
        printf("\n");
        dynamicDelete(hundert);
        dynamicDelete(funfzig);
        dynamicDelete(funfhundert);
    }

    return 0;
}