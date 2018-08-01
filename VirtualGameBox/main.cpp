#include <vector>
#include <ios>
#include <fstream>
#include <iostream>
#include <ctime>
#include <thread>
#include "AVR.h"

int main(int argc, char *argv[]) {
    Display display;
    AVR avr(display);
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <bin file>" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1], std::ios::binary);


    std::vector<char> buf((std::istreambuf_iterator<char>(input)),
                          (std::istreambuf_iterator<char>()));

    for (int i = 0; i < buf.size(); i += 2) {
        avr.flash[i >> 1] =
                (static_cast<uint16_t>(static_cast<uint8_t>(buf[i + 1])) << 8) | static_cast<uint8_t>(buf[i]);
    }

    std::thread avr_thread(&AVR::run, &avr);
    display.start();
    //avr.run();

    return 0;
}

