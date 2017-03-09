//
// Created by marian on 07.03.17.
//

#ifndef MARA_PAGE_H
#define MARA_PAGE_H

#include <stddef.h>
#include "predefined.h"

size_t pageSize;

void* allocateStatic(size_t sizeInBytes);
int createNewPage();

#endif //MARA_PAGE_H
