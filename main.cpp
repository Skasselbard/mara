#include <iostream>
#include <cstring>
#include "include/Logger.h"
#include "include/Test.h"

int main(int argc, char** argv) {
    Logger::info("some info");
    Logger::warning("some warning");
    Logger::error("some error");
    Logger::debug("some debug message");
    //Logger::fatal("fatal", 129);

    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        Test::test(argc, argv);
    }
    return 0;
}