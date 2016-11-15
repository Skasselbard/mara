#include <iostream>
#include "include/Logger.h"
#include "include/Page.h"

int main() {
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
    return 0;
}