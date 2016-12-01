//
// Created by tom on 14.11.16.
//

#include <stdlib.h>
#include <malloc.h>
#include "../include/Page.h"
#include "../include/Logger.h"

Page::Page(size_t sizeInBytes):pageSize(sizeInBytes){
    Logger::info("new page requested");
    this->startOfPage = malloc(sizeInBytes);
    this->staticEnd = (char*)this->startOfPage+sizeInBytes;
    if (startOfPage == nullptr){
        Logger::error("unable to allocate memory for new page");
    }
}

Page::~Page() {
    Logger::warning("Page destructor called");
    free(startOfPage);
}

void* Page::getStaticBlock(size_t sizeInByte) {
    Logger::info("static block requested");
#ifdef ALLIGN_STATIC
    sizeInByte = allign(sizeInByte);
#endif
    if (staticBlockFitInPage(sizeInByte)) {
        this->staticEnd = this->staticEnd - sizeInByte;
        return this->staticEnd;
    }else{
        Logger::error("requested block does not fit in page");
        return nullptr;
    }
}

size_t Page::getDynamicSectorSize() {
    Logger::error("getDynamicSectorSize is not implemented and returns wrong results");
    return 0;
}

bool Page::staticBlockFitInPage(size_t blockSizeInByte) {
    return (blockSizeInByte+staticSectorSize+getDynamicSectorSize() <= pageSize);
}

size_t Page::allign(size_t requestetSizeInByte) {
    Logger::error("Allignment is not implemented and returns the given block size");
    return requestetSizeInByte;
}
