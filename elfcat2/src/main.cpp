#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include <config.h>

#include "report_gen.hpp"


void usage(int ret) {
    std::cout << "Usage: elfcat <filename>" << std::endl;
    std::cout << "Writes <filename>.html to CWD." << std::endl;
    std::exit(ret);
}

std::string parse_arguments(int argc, char** argv) {
    if (argc != 2) {
        usage(1);
    }

    auto argument = std::string(argv[1]);

    if (argument == "-h" || argument == "--help") {
        usage(0);
    }

    if (argument == "-v" || argument == "--version") {
        std::cout << "elfcat " << ELFCAT_VERSION_MAJOR << "." << ELFCAT_VERSION_MAJOR << std::endl;
        std::exit(0);
    }

    return std::string(argv[1]);
}

int main(int argc, char** argv) {
    std::string filename = parse_arguments(argc, argv);

    std::ifstream ifile(filename);
    std::vector<uint8_t> contents((std::istreambuf_iterator<char>(ifile)), std::istreambuf_iterator<char>());

    auto elf = ParsedElf::from_bytes(filename, contents);
    auto report_filename = construct_filename(filename);
    auto report = generate_report(elf);

    std::ofstream ofile(report_filename);
    ofile << report;
}
