#include <cmath>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <string>

#include "include/utils.hpp"


std::string human_format_bytes(uint64_t bytes) {
    constexpr uint64_t base = 1024;
    constexpr char prefixes[] = {'K', 'M', 'G', 'T', 'P', 'E'};

    if (bytes < base) {
        return std::to_string(bytes) + " B";
    }

    double exponent = static_cast<uint32_t>(log(static_cast<double>(bytes)) / log(static_cast<double>(base)));
    std::stringstream formatting;
    formatting << std::setprecision(1) << static_cast<double>(bytes) / pow(static_cast<double>(base), exponent) << " " << prefixes[static_cast<uint32_t>(exponent) - 1] << "iB";
    return formatting.str();
}

std::optional<std::string> html_escape(char ch) {
    switch (ch) {
        case '&': return "&amp;";
        case '<': return "&lt;";
        case '>': return "&gt;";
        case '"': return "&quot;";
        default: return std::nullopt;
    }
}

std::string repeat(const std::string& input, size_t num) {
    std::ostringstream os;
    std::fill_n(std::ostream_iterator<std::string>(os), num, input);
    return os.str();
}

std::string include_str(const std::string& path, const std::string& indent) {
    std::stringstream ss;
    std::ifstream file(path);
    std::string string; 
    while (std::getline(file, string)) {
        ss << indent << string << std::endl;
    }
    return ss.str();
}
