//
// Created by marian on 07.03.17.
//

#include <malloc.h>
#include <assert.h>
#include "../include/Page.h"
#include "../include/predefined.h"

byte* startOfPage = NULL; //equals top of stack on page creation
byte* endOfPage = NULL; //first byte not in the page anymore
byte* topOfStack = NULL; //points to the next free byte

size_t pageSize = DEFAULT_PAGE_SIZE;


void* allocateStatic(size_t sizeInBytes) {
    //in case we get a request with size 0
    if(__glibc_unlikely(!sizeInBytes)) return NULL;
    //if we don't have a page yet
    if(__glibc_unlikely(!startOfPage)) {
        //if we couldn't get one
        if(__glibc_unlikely(!createNewPage())) return malloc(sizeInBytes);
    }
    //if the request doesn't fit into the page
    if(__glibc_unlikely(topOfStack + sizeInBytes > endOfPage)){
        realloc(startOfPage, topOfStack - startOfPage);
        //if we couldn't get a new one
        if(__glibc_unlikely(createNewPage())) return malloc(sizeInBytes);
    }
    //default case: shift the top of stack, return the old topOfStack as pointer
    byte* p = topOfStack;
    topOfStack = topOfStack+sizeInBytes;
    assert(startOfPage);
    assert(topOfStack);
    assert(endOfPage);
    assert(startOfPage < topOfStack);
    assert(endOfPage >= topOfStack);
    return p;
}

int createNewPage(){
    startOfPage = malloc(pageSize);
    if(__glibc_unlikely(!startOfPage)) return 0;
    endOfPage = startOfPage + pageSize;
    topOfStack = startOfPage;
    assert(startOfPage);
    assert(topOfStack);
    assert(endOfPage);
    assert(startOfPage = topOfStack);
    assert(endOfPage > topOfStack);
    return 1;
}
