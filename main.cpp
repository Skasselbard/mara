#include <iostream>
#include <cstring>
#include "include/Logger.h"
#include "include/Test.h"
#include "include/Page.h"

int main(int argc, char** argv) {
    Logger::info("some info");
    Logger::warning("some warning");
    Logger::error("some error");
    Logger::debug("some debug message");
    //Logger::fatal("fatal", 129);
    Page testPage = Page(104857600);//100mb

    char* block1 = (char*)testPage.getStaticBlock(20);
    char* block2 = (char*)testPage.getStaticBlock(789345);
    char* mightyBlock = (char*)testPage.getStaticBlock(99999999999999999);
    sprintf(block1, "%s", "das ist ein test");
    sprintf(block2, "%s", "das ist ein test2");
    if ( mightyBlock != nullptr){

    }else{
        Logger::error("mighty block was not initialized");
    }
    printf("%s\n%s", block1, block2);

    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        Test::test(argc, argv);
    }
    return 0;
}