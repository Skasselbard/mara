//
// Created by tom on 14.11.16.
//

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "../include/Logger.h"

FILE const *Logger::logFile = fopen(".mara.log", "w");
char Logger::buffer[LOG_BUFFER_SIZE];

void Logger::warning(const char *message) {
    writeTimeStamp();
    strcat(buffer,"Warning: ");
    strcat(buffer, message);
    writeToLog();
}

void Logger::writeToLog() {
    strcat(buffer,"\n");
    printf("%s",buffer);
    fflush(stdout);
    if (logFile != nullptr){
        fprintf((FILE*)logFile, "%s", buffer);
        fflush((FILE*)logFile);
    }
    clearBuffer();
}

int Logger::clearBuffer() {
    for (int i = 0; i < LOG_BUFFER_SIZE; ++i) {
        buffer[i] = '\0';
    }
    return 0;
}

void Logger::writeTimeStamp() {
    time_t t = time(0);   // get time now
    struct tm *now = localtime( & t );
    sprintf(buffer,
            "[%i.%i.%i-%i:%i-%i]",
            now->tm_year + 1900,
            now->tm_mon + 1,
            now->tm_mday,
            now->tm_hour,
            now->tm_min,
            now->tm_sec
    );
}

void Logger::info(const char *message) {
    writeTimeStamp();
    strcat(buffer,"Info: ");
    strcat(buffer, message);
    writeToLog();
}

void Logger::error(const char *message) {
    writeTimeStamp();
    strcat(buffer,"Error: ");
    strcat(buffer, message);
    writeToLog();
}

void Logger::debug(const char *message) {
#ifdef DEBUG
    writeTimeStamp();
    strcat(buffer,"Debug: ");
    strcat(buffer, message);
    writeToLog();
#endif
}

void Logger::fatal(const char *message, int exitcode) {
    writeTimeStamp();
    strcat(buffer,"Fatal Error: ");
    strcat(buffer, message);
    writeToLog();
#ifdef EXIT_ON_FATAL
    exit(exitcode);
#endif
}
