//
// Created by tom on 14.11.16.
//

#include <stdlib.h>
#include <malloc.h>
#include <new>
#include "../include/Page.h"
#include "../include/Logger.h"
#include "../include/PageList.h"
#include "../include/OccupiedSpace.h"
#include "../include/CodeBlock.h"

Page::Page(size_t sizeInBytes):pageSize(sizeInBytes){
    Logger::info("new page requested");
    if(sizeInBytes > MAX_PAGE_SIZE){
        sizeInBytes = MAX_PAGE_SIZE;
        Logger::warning("maximum page size exceeded. Decreased size to the maximum of 4294967295 byte");
    }
    this->startOfPage = malloc(sizeInBytes);
    if (startOfPage == nullptr){
        Logger::error("unable to allocate memory for new page");
    }
    this->staticEnd = (byte*)this->startOfPage+sizeInBytes;
    bucketList.addToList(generateFirstBucketEntry());
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
    return (blockSizeInByte <= (staticEnd - dynamicEnd));
}

size_t Page::allign(size_t requestetSizeInByte) {
    Logger::error("Allignment is not implemented and returns the given block size");
    return requestetSizeInByte;
}

OccupiedSpace * Page::getDynamicBlock(size_t sizeInByte) {
#ifdef ALLIGN_DAYNAMIC
    sizeInByte = allign(sizeInByte);
#endif
    FreeSpace* freeSpace = bucketList.getFreeSpace(sizeInByte);
    OccupiedSpace* returnBlock = (OccupiedSpace*)freeSpace;
    if(freeSpace == nullptr){
        Logger::info("no applicable freespace in this page. Switching to the next one");
        if (nextPage == nullptr){
            Logger::info("last page. Creating new one.");
            try
            {
                nextPage = new Page(PageList::getPageSize());
            }catch (std::bad_alloc& ba){
                Logger::error("unable to allocate new Page");
                Logger::error(ba.what());
                return nullptr;
            }
        }
        returnBlock = nextPage->getDynamicBlock(sizeInByte);
    }else {
        bucketList.deleteFromList(freeSpace);
        FreeSpace *remainingSpace = cutFromFreeSpace(freeSpace, sizeInByte + (2*Space::computeCodeBlockSize(sizeInByte)));
        if (remainingSpace) {
            bucketList.addToList(remainingSpace);
        }
        returnBlock->toOccupied(sizeInByte);
        if (returnBlock->getRightMostEnd() > dynamicEnd) {
            dynamicEnd = freeSpace->getRightMostEnd();
        }
    }
    return returnBlock;
}

FreeSpace *Page::cutFromFreeSpace(FreeSpace *freeSpace, size_t bytesToCutOf) {
    if ((freeSpace->getSize() - bytesToCutOf) < SMALLEST_POSSIBLE_FREESPACE) {
        return nullptr;
    }else{
        freeSpace = freeSpace->resize(((byte*)freeSpace)+bytesToCutOf);
        return freeSpace;
    }
}

FreeSpace *Page::generateFirstBucketEntry() {
    FreeSpace* freeSpace = (FreeSpace*) startOfPage;
    size_t codeBlockSize = 0;
    CodeBlock::getCodeBlock((byte*)startOfPage, pageSize, codeBlockSize);
    freeSpace->setNext(nullptr);
    return freeSpace;
}
