#include <vector>
#include <ios>
#include <fstream>
#include <iostream>
#include <ctime>
#include "AVR.h"
#include "Window.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <bin file>" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1], std::ios::binary);


    std::vector<char> buf((std::istreambuf_iterator<char>(input)),
                          (std::istreambuf_iterator<char>()));

    QApplication app(argc, argv);
    Window window(buf);
    window.show();
    return app.exec();
}
