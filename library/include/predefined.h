//
// Created by tom on 21.11.16.
//

#ifndef MARA_PREDEFINED_H
#define MARA_PREDEFINED_H

#include <stdint.h>

#define USE_MARA


// Stored page sizes: 104857600 (100MB),
//                    4294967296 (4GB)
#define DEFAULT_PAGE_SIZE 104857600
#define USE_REALLOC
#define STATISTIC
#define LOGGING

typedef uint8_t byte;

#endif //MARA_PREDEFINED_H
