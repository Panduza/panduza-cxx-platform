/**
 * @file   main.cxx
 *  Main program
 * @author Adel, Valentin
 */

#include <iostream>

#include "meta_platform.hxx"
#include "loguru/loguru.hxx"

// Main function, initializes all the objects and starts the program
int main(int argc, char *argv[])
{
    // Create a Loguru logs file, initialize it with timestamps and
    // detects verbosity level on command line (-v LEVEL)
    // -v command line is used for loguru, it is not usable for other command

    loguru::add_file("/etc/panduza/logs/Platform.log", loguru::Append, loguru::Verbosity_MAX);
    loguru::init(argc, argv);

    Metaplatform *plat = new Metaplatform(argc, argv);
    return plat->run();
}
