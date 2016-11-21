//
// Created by tom on 21.11.16.
//

#ifndef MARA_PREDEFINED_H
#define MARA_PREDEFINED_H

#define LOG_BUFFER_SIZE 4096
#define ALLIGNMENT
#ifdef ALLIGNMENT
#define ALLIGN_STATIC
#define ALLIGN_DAYNAMIC
#endif
#define MAX_PAGE_SIZE 4294967295 //4GigaByte - 1Byte
#define SMALLEST_POSSIBLE_FREESPACE 6 //6 byte
#define MAX_CODEBLOCK_ARRAY_SIZE 128 //TODO: probably enough?

typedef char byte;

#endif //MARA_PREDEFINED_H
