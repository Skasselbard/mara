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


#ifndef MARA_PAGE_H
#define MARA_PAGE_H

#include "predefined.h"
#include "Statistic.h"

class Mara {

private:

    size_t pageSize = DEFAULT_PAGE_SIZE;

    byte *startOfPage = NULL; //equals top of stack on page creation
    byte *endOfPage = NULL; //first byte not in the page anymore
    byte *topOfStack = NULL; //points to the next free byte

    int createNewPage();

public:

/**
     * Reserves memory in the static sector. Memory in this sector is expected to live as long as the program. Memory
     * allocated with this function CANNOT be freed. Mara returns a pointer to the location with an unused block with the
     * given size and completely ignore this space in the future. The advantage is that these blocks will produce absolutely
     * overhead inside of its page.
     * \param sizeInByte size of the block you want to use
     * \return a pointer to the first byte of the block you want to use. After this operation the block will stay allocated
     * until complete program termination.
     */
    void *staticNew(size_t sizeInBytes);

    /**
     * Sets the default size for blocks allocated from the system. These blocks are used bei mara for internal memory management.
     * Calling staticNew will allocate memory in one of these blocks.
     * \param sizeInByte default size for malloc calls from mara
     * \return 0 on success
     */
    int setPageSize(size_t sizeInByte) { pageSize = sizeInByte; return 0; };

    Statistic * statistic = new Statistic();


};


#endif //MARA_PAGE_H
