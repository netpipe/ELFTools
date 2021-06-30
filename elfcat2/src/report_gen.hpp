#include <sstream>
#include <string>
#include <vector>
#include "utils.hpp"
#include <parser.hpp>


const std::string INDENT = "  ";


template<class... parameters>
void wnonl(std::stringstream& ss, uint32_t indent_level, const parameters&... values) {
    ss << repeat(INDENT, indent_level);
    ((ss << values), ...);
}

template<class... parameters>
void w(std::stringstream& ss, uint32_t indent_level, const parameters&... values) {
    wnonl(ss, indent_level, values...);
    ss << std::endl;
}

template<class lhs, class rhs>
void wrow(std::stringstream& ss, uint32_t indent_level, const lhs& lhs_value, const rhs& rhs_value) {
    wnonl(ss, indent_level, "<tr> ");
    wnonl(ss, 0, "<td>", lhs_value, ":</td> ");
    wnonl(ss, 0, "<td>", rhs_value, "</td> ");
    w(ss, 0, "</tr>");
}

std::string basename(const std::string& path);
std::string stem(const std::string path);
std::string construct_filename(const std::string& filename);
std::string indent(size_t level, const std::string& line);
void generate_head(std::stringstream& o, const ParsedElf& elf);
void generate_svg_element(std::stringstream& o);
void generate_file_info_table(std::stringstream& o, const ParsedElf& elf);
void generate_phdr_info_table(std::stringstream& o, const ParsedPhdr& phdr, size_t idx);
void generate_phdr_info_tables(std::stringstream& o, const ParsedElf& elf);
void generate_shdr_info_table(std::stringstream& o, const ParsedElf& elf, const ParsedShdr& shdr, size_t idx);
void generate_shdr_info_tables(std::stringstream& o, const ParsedElf& elf);
std::string format_string_byte(uint8_t byte);
std::string format_string_slice(const std::vector<uint8_t>& slice);
void generate_note_data(std::stringstream& o, const Note& note);
void generate_segment_info_table(std::stringstream& o, const ParsedElf& elf, const ParsedPhdr& phdr);
void generate_strtab_data(std::stringstream& o, const std::vector<uint8_t>& section);
void generate_section_info_table(std::stringstream& o, const ParsedElf& elf, const ParsedShdr& shdr);
bool has_segment_detail(uint32_t ptype);
bool has_section_detail(uint32_t ptype);
void generate_segment_info_tables(std::stringstream& o, const ParsedElf& elf);
void generate_section_info_tables(std::stringstream& o, const ParsedElf& elf);
void generate_sticky_info_table(std::stringstream& o, const ParsedElf& elf);
void add_highlight_script(std::stringstream& o);
void add_description_script(std::stringstream& o);
void add_conceal_script(std::stringstream& o);
void add_offsets_script(std::stringstream& o, const ParsedElf& elf);
void add_arrows_script(std::stringstream& o, const ParsedElf& elf);
void add_collapsible_script(std::stringstream& o);
void add_scripts(std::stringstream& o, const ParsedElf& elf);
std::string format_magic(uint8_t byte);
char digit_to_hex(uint8_t digit);
void append_hex_byte(std::stringstream& s, uint8_t byte);
void generate_dump_for_byte(size_t idx, std::stringstream& dump, const ParsedElf& elf);
std::optional<size_t> skip_bytes(size_t idx, size_t len, const ParsedElf& elf);
std::string generate_file_dump(const ParsedElf& elf);
void generate_ascii_dump(std::stringstream& o, const ParsedElf& elf);
void generate_body(std::stringstream& o, const ParsedElf& elf);
std::string generate_report(const ParsedElf& elf);
