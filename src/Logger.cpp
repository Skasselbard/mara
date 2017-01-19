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
#ifdef MARA_LOG
    writeTimeStamp();
    strcat(buffer,"Warning: ");
    strcat(buffer, message);
    writeToLog();
#endif
}

void Logger::writeToLog() {
#ifdef MARA_LOG
    strcat(buffer,"\n");
    printf("%s",buffer);
    fflush(stdout);
    if (logFile != nullptr){
        fprintf((FILE*)logFile, "%s", buffer);
        //fflush((FILE*)logFile);
    }
    clearBuffer();
#endif
}

int Logger::clearBuffer() {
#ifdef MARA_LOG
    for (int i = 0; i < LOG_BUFFER_SIZE; ++i) {
        buffer[i] = '\0';
    }
    return 0;
#endif
}

void Logger::writeTimeStamp() {
#ifdef MARA_LOG
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
#endif
}

void Logger::info(const char *message) {
#ifdef MARA_LOG
    writeTimeStamp();
    strcat(buffer,"Info: ");
    strcat(buffer, message);
    writeToLog();
#endif
}

void Logger::error(const char *message) {
#ifdef MARA_LOG
    writeTimeStamp();
    strcat(buffer,"Error: ");
    strcat(buffer, message);
    writeToLog();
#endif
}

void Logger::debug(const char *message) {
#ifdef MARA_LOG
#ifdef MARA_DEBUG
    writeTimeStamp();
    strcat(buffer,"Debug: ");
    strcat(buffer, message);
    writeToLog();
#endif
#endif
}

void Logger::fatal(const char *message, int exitcode) {
#ifdef MARA_LOG
    writeTimeStamp();
    strcat(buffer,"Fatal Error: ");
    strcat(buffer, message);
    writeToLog();
#ifdef EXIT_ON_FATAL
    exit(exitcode);
#endif
#endif
}
