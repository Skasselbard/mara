// This file is part of mara.
//
// Copyright 2017
//
// Contributors: Tom Meyer, Julian Gaede, Marian Stein
//
// mara is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// mara is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with mara.  If not, see <http://www.gnu.org/licenses/>

//
// Created by marian on 07.03.17.
//

#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include "../include/Mara.h"

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
    statistic->newStatic(sizeInBytes);
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
    statistic->newPage(startOfPage);
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
