#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include "defs.hpp"
#include "report_gen.hpp"


std::string basename(const std::string& path) {
    return fs::path(path).filename().string();
}

std::string stem(const std::string path) {
    return fs::path(path).filename().stem().string();
}

std::string construct_filename(const std::string& filename) {
    return stem(basename(filename)) + ".html";
}

std::string indent(size_t level, const std::string& line) {
    if (line.empty()) {
        return {};
    }
    return repeat(INDENT, level) + line;
}

void generate_head(std::stringstream& o, const ParsedElf& elf) {
    std::string stylesheet = include_str("data/style.css", repeat(INDENT, 3));

    w(o, 1, "<head>");
    w(o, 2, "<meta charset='utf-8'>");
    w(o, 2, "<meta name='viewport' content='width=900, initial-scale=1'>");
    w(o, 2, "<title>", basename(elf.filename), "</title>");
    w(o, 2, "<style>");
    wnonl(o, 0, stylesheet);
    w(o, 2, "</style>");
    w(o, 1, "</head>");
}

void generate_svg_element(std::stringstream& o) {
    w(o, 2, "<svg width='100%' height='100%'>");

    w(o, 3, "<defs>");

    wnonl(o, 4, "<marker id='arrowhead' viewBox='0 0 10 10' ");
    wnonl(o, 0, "refX='10' refY='5' ");
    w(o, 0, "markerWidth='10' markerHeight='10' orient='auto'>");

    w(o, 5, "<path d='M 0 0 L 10 5 L 0 10 z' />");

    w(o, 4, "</marker>");

    w(o, 3, "</defs>");

    wnonl(o, 3, "<g id='arrows' stroke='black' ");
    wnonl(o, 0, "stroke-width='1' marker-end='url(#arrowhead)'>");
    w(o, 0, "</g>");

    w(o, 2, "</svg>");
}

void generate_file_info_table(std::stringstream& o, const ParsedElf& elf) {
    w(o, 4, "<table>");

    for (const auto& [id, desc, value] : elf.information) {
        wnonl(o, 5, "<tr id='info_", id, "'> ");
        wnonl(o, 0, "<td>", desc, ":</td> ");
        wnonl(o, 0, "<td>", value, "</td> ");
        w(o, 0, "</tr>");
    }

    w(o, 4, "</table>");
}

void generate_phdr_info_table(std::stringstream& o, const ParsedPhdr& phdr, size_t idx) {
    std::vector<std::tuple<std::string, std::string>> items = {
        {"Type", ptype_to_string(phdr.ptype)},
        {"Flags", phdr.flags},
        {"Offset in file", std::to_string(phdr.file_offset)},
        {"Size in file", std::to_string(phdr.file_size)},
        {"Vaddr in memory", int_to_hex(phdr.vaddr)},
        {"Size in memory", int_to_hex(phdr.memsz)},
        {"Alignment", int_to_hex(phdr.alignment)},
    };

    w(o, 5, "<table class='conceal' id='info_phdr", idx, "'>");

    for (const auto& [desc, value] : items) {
        wrow(o, 6, desc, value);
    }

    w(o, 5, "</table>");
}

void generate_phdr_info_tables(std::stringstream& o, const ParsedElf& elf) {
    size_t idx = 0;
    for (const auto& phdr : elf.phdrs) {
        generate_phdr_info_table(o, phdr, idx);
        ++idx;
    }
}

void generate_shdr_info_table(std::stringstream& o, const ParsedElf& elf, const ParsedShdr& shdr, size_t idx) {
    std::vector<std::tuple<std::string, std::string>> items = {
        {"Name", elf.shnstrtab.get(shdr.name)},
        {"Type", shtype_to_string(shdr.shtype)},
        {"Flags", shflags_to_string(shdr.flags)},
        {"Vaddr in memory", int_to_hex(shdr.addr)},
        {"Offset in file", std::to_string(shdr.file_offset)},
        {"Size in file", std::to_string(shdr.size)},
        {"Linked section", std::to_string(shdr.link)},
        {"Extra info", std::to_string(shdr.link)},
        {"Alignment", int_to_hex(shdr.addralign)},
        {"Size of entries", std::to_string(shdr.entsize)},
    };

    w(o, 5, "<table class='conceal' id='info_shdr", idx, "'>");

    for (const auto& [desc, value] : items) {
        wrow(o, 6, desc, value);
    }

    w(o, 5, "</table>");
}

void generate_shdr_info_tables(std::stringstream& o, const ParsedElf& elf) {
    size_t idx = 0;
    for (const auto& shdr : elf.shdrs) {
        generate_shdr_info_table(o, elf, shdr, idx);
        ++idx;
    }
}

std::string format_string_byte(uint8_t byte) {
    if (byte >= 0x21 && byte <= 0x7e) {
        return {static_cast<char>(byte)};
    }
    return std::string("<b>") + int_to_hex(byte) + std::string("</b> ");
}

std::string format_string_slice(const std::vector<uint8_t>& slice) {
    return std::string(slice.cbegin(), slice.cend());
}

void generate_note_data(std::stringstream& o, const Note& note) {
    std::string name = note.name.empty()?"":format_string_slice(byte_range(note.name, 0, note.name.size() - 1));

    wrow(o, 6, "Name", name);

    wrow(o, 6, "Type", int_to_hex(note.ntype));

    if (note.ntype == NT_GNU_BUILD_ID) {
        std::stringstream hash;

        for (const auto& byte : note.desc) {
            append_hex_byte(hash, byte);
        }

        wrow(o, 6, "Build ID", hash.str());
    } else {
        wrow(o, 6, "Desc", format_string_slice(note.desc));
    }
}

void generate_segment_info_table(std::stringstream& o, const ParsedElf& elf, const ParsedPhdr& phdr) {
    if (phdr.ptype == PT_INTERP) {
        auto interp_str = format_string_slice(byte_range(elf.contents, phdr.file_offset, phdr.file_offset + phdr.file_size - 1));
        wrow(o, 6, "Interpreter", interp_str);
    } else if (phdr.ptype == PT_NOTE) {
        // this is really bad and made out of desperation.
        // notes stored in elf.notes don't have to have 1-to-1
        // correspondence with phdrs, yet here we are.
        for (size_t i = 0; i < elf.notes.size(); ++i) {
            const auto& note = elf.notes[i];

            generate_note_data(o, note);

            if (i != elf.notes.size() - 1) {
                w(o, 6, "<tr> <td><br></td> </tr>");
            }
        }
    }
}

void generate_strtab_data(std::stringstream& o, const std::vector<uint8_t>& section) {
    size_t curr_start = 0;

    w(o, 6, "<tr>");
    w(o, 7, "<td></td>");
    w(o, 7, "<td>");
    w(o, 8, "<div>");

    size_t i = 0;
    for (const auto& c : section) {
        if (c == 0) {
            size_t end = (curr_start == 0)?0:i;


            std::string maybe = std::string(section.cbegin() + curr_start, section.cbegin() + end);

            if (section[curr_start] != 0) {
                w(o, 9, maybe);
            }

            curr_start = i + 1;
        }
        ++i;
    }

    w(o, 8, "</div>");
    w(o, 7, "</td>");
    w(o, 6, "</tr>");
}

void generate_section_info_table(std::stringstream& o, const ParsedElf& elf, const ParsedShdr& shdr) {
    auto section = byte_range(elf.contents, shdr.file_offset, shdr.file_offset + shdr.size);

    if (shdr.shtype == SHT_STRTAB) {
        generate_strtab_data(o, section);
    }
}

// this is ugly
bool has_segment_detail(uint32_t ptype) {
    return ptype == PT_INTERP || ptype == PT_NOTE;
}

bool has_section_detail(uint32_t ptype) {
    return ptype == SHT_STRTAB;
}

void generate_segment_info_tables(std::stringstream& o, const ParsedElf& elf) {
    size_t idx = 0;
    for (const auto& phdr : elf.phdrs) {
        w(o, 5, "<table class='conceal' id='info_segment", idx, "'>");
        wrow(o, 6, "Segment type", ptype_to_string(phdr.ptype));
        wrow(o, 6, "Size in file", phdr.file_size);
        wrow(o, 6, "Size in memory", phdr.memsz);

        if (has_segment_detail(phdr.ptype)) {
            w(o, 6, "<tr><td><br></td></tr>");
            generate_segment_info_table(o, elf, phdr);
        }

        w(o, 5, "</table>");
        ++idx;
    }
}

void generate_section_info_tables(std::stringstream& o, const ParsedElf& elf) {
    size_t idx = 0;
    for (const auto& shdr : elf.shdrs) {
        w(o, 5, "<table class='conceal' id='info_section", idx, "'>");
        wrow(o, 6, "Section type", shtype_to_string(shdr.shtype));
        wrow(o, 6, "Size", shdr.size);

        if (has_section_detail(shdr.shtype)) {
            w(o, 6, "<tr><td><br></td></tr>");
            generate_section_info_table(o, elf, shdr);
        }

        w(o, 5, "</table>");
        ++idx;
    }
}

void generate_sticky_info_table(std::stringstream& o, const ParsedElf& elf) {
    w(o, 2, "<table id='sticky_table' cellspacing='0'>");
    w(o, 3, "<tr>");

    w(o, 4, "<td id='desc'></td>");

    w(o, 4, "<td id='struct_infotables'>");
    generate_phdr_info_tables(o, elf);

    generate_shdr_info_tables(o, elf);
    w(o, 4, "</td>");

    w(o, 4, "<td id='data_infotables'>");
    generate_segment_info_tables(o, elf);

    generate_section_info_tables(o, elf);
    w(o, 4, "</td>");

    w(o, 3, "</tr>");
    w(o, 2, "</table>");
}

void add_highlight_script(std::stringstream& o) {
    std::array<std::string, 16> ids = {
        "class",
        "data",
        "abi",
        "abi_ver",
        "e_type",
        "e_machine",
        "e_entry",
        "e_phoff",
        "e_shoff",
        "e_flags",
        "e_ehsize",
        "e_phentsize",
        "e_phnum",
        "e_shentsize",
        "e_shnum",
        "e_shstrndx",
    };

    w(o, 2, "<script type='text/javascript'>");

    wnonl(o, 0, include_str("data/js/highlight.js", repeat(INDENT, 3)));

    for (const auto& id : ids) {
        w(o, 3, "highlightIds('", id, "', 'info_", id, "')");
    }

    w(o, 2, "</script>");
}

void add_description_script(std::stringstream& o) {
    w(o, 2, "<script type='text/javascript'>");

    wnonl(o, 0, include_str("data/js/description.js", repeat(INDENT, 3)));

    w(o, 2, "</script>");
}

void add_conceal_script(std::stringstream& o) {
    w(o, 2, "<script type='text/javascript'>");

    wnonl(o, 0, include_str("data/js/conceal.js", repeat(INDENT, 3)));

    w(o, 2, "</script>");
}

void add_offsets_script(std::stringstream& o, const ParsedElf& elf) {
    w(o, 2, "<script type='text/javascript'>");

    w(o, 3, "let fileLen = ", elf.file_size);

    wnonl(o, 0, include_str("data/js/offsets.js", repeat(INDENT, 3)));

    w(o, 3, "populateOffsets(16)");

    w(o, 2, "</script>");
}

void add_arrows_script(std::stringstream& o, const ParsedElf& elf) {
    w(o, 2, "<script type='text/javascript'>");

    wnonl(o, 0, include_str("data/js/arrows.js", repeat(INDENT, 3)));

    w(o, 3, "connect('#e_phoff', '#bin_phdr0');");
    w(o, 3, "connect('#e_shoff', '#bin_shdr0');");

    for (size_t i = 0; i < elf.phdrs.size(); ++i) {
        w(o, 3, "connect('#bin_phdr", i, " > .p_offset', '#bin_segment", i, "');");
    }

    for (size_t i = 0; i < elf.shdrs.size(); ++i) {
        w(o, 3, "connect('#bin_shdr", i, " > .sh_offset', '#bin_section", i, "');");
    }

    w(o, 2, "</script>");
}


void add_collapsible_script(std::stringstream& o) {
    w(o, 2, "<script type='text/javascript'>");

    wnonl(o, 0, include_str("data/js/collapse.js", repeat(INDENT, 3)));

    w(o, 2, "</script>");
}

void add_scripts(std::stringstream& o, const ParsedElf& elf) {
    add_highlight_script(o);

    add_description_script(o);

    add_conceal_script(o);

    // disabled while working on section headers because it doesn't work for nested elements
    // add_collapsible_script(o);

    add_offsets_script(o, elf);

    add_arrows_script(o, elf);
}

std::string format_magic(uint8_t byte) {
    if (byte >= 0x21 && byte <= 0x7e) {
        return std::string("&nbsp;") + static_cast<char>(byte);
    }
    return int_to_hex(byte);
}

char digit_to_hex(uint8_t digit) {
    if (digit < 10) {
        return '0' + digit;
    }
    return 'a' + (digit - 10);
}

void append_hex_byte(std::stringstream& s, uint8_t byte) {
    if (byte < 0x10) {
        s << '0';

        s << digit_to_hex(byte);
    } else {
        auto trailing_digit = byte % 16;
        auto leading_digit = byte / 16;

        s << digit_to_hex(leading_digit);
        s << digit_to_hex(trailing_digit);
    }
}

void generate_dump_for_byte(size_t idx, std::stringstream& dump, const ParsedElf& elf) {
    auto byte = elf.contents[idx];

    for (const auto& range_type : elf.ranges.data[idx]) {
        if (!range_type->is_end()) {
            dump << "<span " << range_type->span_attributes() << ">";
        }
    }

    if (idx < 4) {
        dump << format_magic(byte);
    } else {
        append_hex_byte(dump, byte);
    }

    dump << repeat("</span>", elf.ranges.lookup_range_ends(idx));
    if ((idx + 1) % 16 == 0) {
        dump << std::endl;
    } else {
        dump << " ";
    }
}

// assumes balance == 1
std::optional<size_t> skip_bytes(size_t idx, size_t len, const ParsedElf& elf) {
    if (!(elf.ranges.data[idx].size() == 1 && elf.ranges.data[idx][0]->skippable())) {
        return std::nullopt;
    }

    auto new_idx = idx + 1;

    while (new_idx < len) {
        switch (elf.ranges.data[new_idx].size()) {
        case 0:
            new_idx += 1;
            break;
        case 1:
            if (elf.ranges.data[new_idx][0]->is_end()) {
                return new_idx;
            }
            return std::nullopt;
        default:
            return std::nullopt;
        }
    }

    return std::nullopt;
}

std::string generate_file_dump(const ParsedElf& elf) {
    std::stringstream dump;
    size_t i = 0;
    size_t len = elf.contents.size();
    int64_t balance = 0;

    while (i < len) {
        // we are iterating twice for one byte, which is not very smart
        for (const auto& r : elf.ranges.data[i]) {
            if (r->is_end()) {
                --balance;
            } else {
                ++balance;
            }
        }

        // account for one potential skippable range which would already start (incr. balance by 1)
        // disable while working on offsets
        if (false && balance == 1) {
            auto new_idx = skip_bytes(i, len, elf);
            if (new_idx) {
                dump << "<span " << elf.ranges.data[i][0]->span_attributes() << ">..</span>";
                if ((i + 1) % 16 == 0) {
                    dump << std::endl;
                } else {
                    dump << " ";
                }
                i = *new_idx;
                continue;
            }
        }

        generate_dump_for_byte(i, dump, elf);
        ++i;
    }

    return dump.str();
}

void generate_ascii_dump(std::stringstream& o, const ParsedElf& elf) {
    size_t i = 0;
    for (const auto& b : elf.contents) {
        if (b >= 0x21 and b <= 0x7e) {
            auto escaped = html_escape(b);
            if (escaped) {
                wnonl(o, 0, *escaped);
            } else {
                wnonl(o, 0, b);
            }
        } else {
            wnonl(o, 0, ".");
        }

        if ((i + 1) % 16 == 0) {
            w(o, 0, "");
        }
        ++i;
    }
}

void generate_body(std::stringstream& o, const ParsedElf& elf) {
    w(o, 1, "<body>");

    generate_svg_element(o);

    w(o, 2, "<table id='headertable'>");
    w(o, 3, "<td>");
    generate_file_info_table(o, elf);
    w(o, 3, "</td>");
    w(o, 3, "<td id='rightmenu'>");
    w(o, 4, "<p id='credits'>generated with elfcat 0.0.1</p>");
    w(o, 3, "</td>");
    w(o, 2, "</table>");

    w(o, 2, "<div id='offsets'></div>");

    w(o, 2, "<div id='bytes'>");
    wnonl(o, 0, generate_file_dump(elf));
    w(o, 2, "</div>");

    w(o, 2, "<div id='ascii'>");
    generate_ascii_dump(o, elf);
    w(o, 2, "</div>");

    generate_sticky_info_table(o, elf);

    add_scripts(o, elf);

    w(o, 1, "</body>");
}

std::string generate_report(const ParsedElf& elf) {
    std::stringstream output;

    w(output, 0, "<!doctype html>");
    w(output, 0, "<html>");

    generate_head(output, elf);
    generate_body(output, elf);

    w(output, 0, "</html>");

    return output.str();
}
