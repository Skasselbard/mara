//
// Created by tom on 14.11.16.
//

#include <new>
#include "../include/PageList.h"
#include "../include/Logger.h"

size_t PageList::pageSize= 104857600;//100mb
Page* PageList::firstPage = new Page(pageSize);
int PageList::setPageSize(size_t sizeInByte) {
    pageSize = sizeInByte;
    return 0;
}

size_t PageList::getPageSize() {
    return pageSize;
}

void *PageList::staticNew(size_t sizeInByte) {
    Page* currentPage = firstPage;
    void* returnBlock = nullptr;
    while ( (returnBlock = firstPage->getStaticBlock(sizeInByte)) == nullptr){
        if(!iteratePage(currentPage)){
            return nullptr;
        }
    }
    return returnBlock;
}

bool PageList::addPageToList(Page *currentPage) {
    Logger::info("last page. Creating new one.");
    Page* nextPage = nullptr;
    try{
        nextPage = new Page(PageList::getPageSize());
    }catch (std::bad_alloc& ba){
        Logger::error("unable to allocate new Page");
        Logger::error(ba.what());
        return false;
    }
    currentPage->setNextPage(nextPage);
    return true;
}

void *PageList::dynamicNew(size_t sizeInByte) {
    Page* currentPage = firstPage;
    OccupiedSpace* returnBlock = nullptr;
    while ( (returnBlock = firstPage->getDynamicBlock(sizeInByte)) == nullptr){
        if(!iteratePage(currentPage)){
            return nullptr;
        }
    }
    return returnBlock;
}

bool PageList::iteratePage(Page* &currentPage) {
    if (currentPage->getNextPage()){
        currentPage = currentPage->getNextPage();
    } else{
        if (addPageToList(currentPage)){
            currentPage = currentPage->getNextPage();
        }else{
            return false;
        }
    }
    return true;
}

bool PageList::dynamicDelete(void *address) {
    Page* currentPage = firstPage;
    while (!currentPage->blockIsInSpace(address)){
        if(!iteratePage(currentPage)){
            return false;
        }
    }
    currentPage->deleteBlock(address);
    return true;
}
