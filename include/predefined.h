//
// Created by tom on 21.11.16.
//

#ifndef MARA_PREDEFINED_H
#define MARA_PREDEFINED_H

#include <stdint.h>

#define LOG_BUFFER_SIZE 4096

// enables/disables the alignment
//#define ALIGNMENT
#ifdef ALIGNMENT
#define ALIGN_STATIC
#define ALIGN_DYNAMIC
#endif

#define MAX_PAGE_SIZE 4294967295 //4GigaByte - 1Byte
#define SMALLEST_POSSIBLE_FREE_SPACE 6 //6 byte
#define MAX_CODE_BLOCK_ARRAY_SIZE 128 //TODO: probably enough?
#define EXIT_ON_FATAL //call exit if LOGGER::fatal() was called
#define DEBUG //mutes/unmutes debugLog
#define STATISTIC
#define STATISTIC_VERBOSE
#define ERROR_NEXT_POINTER (uint32_t)~0 // just ones from front to back (0b11111111111111111111111111111111111111111111....)
#define PRECONDITION
#define POSTCONDITION
typedef uint8_t byte;

enum ERROR_CODES{
    STATIC_AND_DYNAMIC_SECTORS_OVERLAP
};

#endif //MARA_PREDEFINED_H
