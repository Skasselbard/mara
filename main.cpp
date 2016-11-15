#include <iostream>
#include "include/Logger.h"

int main() {
    Logger::info("some info");
    Logger::warning("some warning");
    Logger::error("some error");
    Logger::debug("some debug message");
    //Logger::fatal("fatal", 129);
    return 0;
}