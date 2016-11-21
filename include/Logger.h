//
// Created by tom on 14.11.16.
//

#ifndef MARA_LOGGER_H
#define MARA_LOGGER_H

#include <stdio.h>
#include "predefined.h"

class Logger {

private:
    static char buffer[LOG_BUFFER_SIZE];
    static const FILE *logFile;

    static int clearBuffer();
    static void writeTimeStamp();
    static void writeToLog();

public:
    static void warning(const char* message);
    static void info(const char* message);
    static void error(const char* message);
    static void debug(const char* message);
    static void fatal(const char* message, int exitcode);
};


#endif //MARA_LOGGER_H
