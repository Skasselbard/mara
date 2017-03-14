//
// Created by marian on 07.03.17.
//

#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include "../include/Mara.h"
#include "../include/predefined.h"
#include "../include/Statistic.h"



void* Mara::staticNew(size_t sizeInBytes) {
    //in case we get a request with size 0
    if(__glibc_unlikely(!sizeInBytes)) return NULL;
    //if we don't have a page yet
    if(__glibc_unlikely(!startOfPage)) {
        //if we couldn't get one
        if(__glibc_unlikely(!createNewPage())) return malloc(sizeInBytes);
    }
    //if the request doesn't fit into the page
    if(__glibc_unlikely(topOfStack + sizeInBytes > endOfPage)){
#ifdef USE_REALLOC
        void* newPage = realloc(startOfPage, topOfStack - startOfPage);
        if(newPage != startOfPage){
            fprintf(stderr, "Fatal: realloc changed the pointer!\n");
            fflush(stderr);
            exit(1);
        }
#endif
        //if we couldn't get a new one
        if(__glibc_unlikely(createNewPage())) return malloc(sizeInBytes);
    }
    //default case: shift the top of stack, return the old topOfStack as pointer
    byte* p = topOfStack;
    topOfStack = topOfStack+sizeInBytes;
#ifdef STATISTIC
    statisticNewStatic(sizeInBytes);
#endif
    assert(startOfPage);
    assert(topOfStack);
    assert(endOfPage);
    assert(startOfPage < topOfStack);
    assert(endOfPage >= topOfStack);
    return p;
}

int Mara::createNewPage(){
    startOfPage = (byte *) malloc(pageSize);
    if(__glibc_unlikely(!startOfPage)){
#ifdef LOGGING
        printf("Warn: couldn't allocate a new Page!\n");
#endif
        return 0;
    }
    endOfPage = startOfPage + pageSize;
    topOfStack = startOfPage;
#ifdef STATISTIC
    statisticNewPage();
#endif
    assert(startOfPage);
    assert(topOfStack);
    assert(endOfPage);
    assert(startOfPage = topOfStack);
    assert(endOfPage > topOfStack);
#ifdef LOGGING
    printf("Info:  allocated new page\n");
#endif
    return 1;
}
