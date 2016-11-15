//
// Created by tom on 14.11.16.
//

#include <cstdlib>
#include "../include/Page.h"

Page::Page(size_t sizeInBytes) {
    this->startOfPage = malloc(sizeInBytes);

}

Page::~Page() {
    free(startOfPage);
}

int Page::getStaticBlock(size_t sizeInByte) {
    return 0;
}
