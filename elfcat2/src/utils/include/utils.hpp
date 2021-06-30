#pragma once

#include <array>
#include <cstdint>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>


std::string human_format_bytes(uint64_t bytes);
std::optional<std::string> html_escape(char ch);
std::string repeat(const std::string& input, size_t num);
std::string include_str(const std::string& path, const std::string& indent);

template<class t>
std::string int_to_hex(t value) {
    std::stringstream stream;
    stream << "0x" << std::setfill('0') << std::setw(2 * sizeof(t)) << std::hex << value;
    return stream.str();
}

template<class t, class = typename std::enable_if_t<std::is_integral_v<t>>>
auto byte_range(const std::vector<t>& bytes, size_t start, size_t end) {
    return std::vector<t>(bytes.cbegin() + start, bytes.cbegin() + end);
}

template<class t, class = typename std::enable_if_t<std::is_integral_v<t>>>
t from_le_bytes(const std::vector<uint8_t>& bytes) {
    return *reinterpret_cast<std::decay_t<t>*>(const_cast<uint8_t*>(bytes.data()));
}

template<class t, class = typename std::enable_if_t<std::is_integral_v<t>>>
t from_be_bytes(const std::vector<uint8_t>& bytes) {
    return *reinterpret_cast<std::decay_t<t>*>(std::vector<uint8_t>(bytes.crbegin(), bytes.crend()).data());
}

template<size_t size>
auto to_array(const std::vector<uint8_t>& buffer) {
    std::array<uint8_t, size> array;
    std::copy(buffer.cbegin(), buffer.cbegin() + size, array.begin());
    return array;
}


template<class t>
struct ser_integral_t {
    using type = std::decay_t<t>;

    ser_integral_t(t v) : value(v) {}

    static ser_integral_t from_le_bytes(const std::vector<uint8_t>& bytes) {
        return ser_integral_t(::from_le_bytes<type>(bytes));
    }

    static ser_integral_t from_be_bytes(const std::vector<uint8_t>& bytes) {
        return ser_integral_t(::from_be_bytes<type>(bytes));
    }

    operator type() const {
        return value;
    }

    type value;
};
