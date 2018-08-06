#include <vector>
#include <ios>
#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <cstdio>
#include "AVR.h"
#include "Window.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <hex file>" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1]);

    std::string s;
    std::vector<uint8_t> buf;
    unsigned int segment = 0;
    while (std::getline(input, s)) {
        if (s[0] != ':') continue;
        unsigned int count;
        sscanf(s.c_str() + 1, "%02x", &count);
        unsigned int offset_hi, offset_lo;
        sscanf(s.c_str() + 3, "%02x%02x", &offset_hi, &offset_lo);
        unsigned int offset = (segment << 16) | (offset_hi << 8) | offset_lo;
        unsigned int record;
        sscanf(s.c_str() + 7, "%02x", &record);
        switch (record) {
        case 0:
            for (unsigned int i = 0; i < count; i++) {
                unsigned int t;
                sscanf(s.c_str() + 9 + 2 * i, "%02x", &t);
                buf.push_back(t);
            }
            break;
        case 4:
            unsigned int segment_hi, segment_lo;
            sscanf(s.c_str() + 3, "%02x%02x", &segment_hi, &segment_lo);
            segment = (segment_hi << 8) | segment_lo;
        }
    }

    QApplication app(argc, argv);
    Window window(buf);
    window.show();
    return app.exec();
}
