//
// Created by tom on 14.11.16.
//

#include <new>
#include <stack>
#include <string>
#include <assert.h>
#include "../include/PageList.h"
#include "../include/Logger.h"
#include "../include/Statistic.h"

size_t PageList::pageSize= 104857600;//100mb
Page* PageList::firstPage = new Page(pageSize, true);
int PageList::setPageSize(size_t sizeInByte) {
    pageSize = sizeInByte;
    return 0;
}

size_t PageList::getPageSize() {
    return pageSize;
}

void *PageList::staticNew(size_t sizeInByte) {
#ifdef PRECONDITION
    assert(sizeInByte!=0); //self-explainatory
#endif
    Page* currentPage = firstPage;
    void* returnBlock = nullptr;
    while ( (returnBlock = currentPage->getStaticBlock(sizeInByte)) == nullptr){
        if(!iteratePage(currentPage)){
            returnBlock = nullptr;
            break;
        }
    }
#ifdef STATISTIC
    Statistic::newStatic(sizeInByte, returnBlock);
#endif
#ifdef POSTCONDITION
    assert(returnBlock == currentPage->getStaticEnd());//the returned block must be at the top of the static area
    assert((byte*) returnBlock + sizeInByte <= (byte*) currentPage->getStartOfPage() + pageSize); //the returned block may not go over the page boundaries

#endif
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
    nextPage->setNextPage(currentPage->getNextPage());
    currentPage->setNextPage(nextPage);
    return true;
}

void *PageList::dynamicNew(size_t sizeInByte) {
#ifdef PRECONDITION
    assert(sizeInByte > 0);
#endif
    Page* currentPage = firstPage;
    OccupiedSpace* returnBlock = nullptr;
    while ( (returnBlock = currentPage->getDynamicBlock(sizeInByte)) == nullptr){
        if(!iteratePage(currentPage)){
            return nullptr;
        }
    }
    void * startOfSpace = ((Space*)returnBlock)->getStartOfSpace();
#ifdef STATISTIC
    Statistic::newDynamic(sizeInByte, startOfSpace);
#endif
#ifdef POSTCONDITION
    assert(returnBlock >= currentPage->getStartOfPage() && (byte*) returnBlock < currentPage->getStaticEnd());
#endif
    firstPage = currentPage;
    return startOfSpace;
}

bool PageList::iteratePage(Page* &currentPage) {
    if(currentPage->getNextPage() == firstPage){
        if (addPageToList(currentPage)){
            currentPage = currentPage->getNextPage();
        }else{
            return false;
        }
    }else{
        currentPage = currentPage->getNextPage();
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

Page *PageList::getFirstPage() {
    return firstPage;
}

unsigned int PageList::getPageCount() {
    unsigned int count = 0;
    Page* page = firstPage;
    while (page != nullptr) {
        count++;
        page = page->getNextPage();
    }
    return count;
}

void PageList::clearPages() {
    std::stack<Page *> pageStack;

    Page * currentPage = firstPage;
    while (currentPage != nullptr) {
        pageStack.push(currentPage);
        currentPage = currentPage->getNextPage();
    }

    Logger::debug(("Stacked " + std::to_string(pageStack.size()) + " pages to clear").c_str());

    while (!pageStack.empty()) {
        currentPage = pageStack.top();
        pageStack.pop();
        currentPage->~Page();
    }

}
