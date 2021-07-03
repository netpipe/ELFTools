#include <algorithm>
#include <stdexcept>
#include <string>
#include <variant>

#include <utils.hpp>

#include "include/parser.hpp"
#include "include/elf32.hpp"
#include "include/elf64.hpp"


std::string RangeType::span_attributes() const {
    if (needs_class()) {
        return (needs_id()?(std::string("id='") + id() + "'"):std::string("")) 
            + "class='" + class_() + (always_highlight()?" hover":"") + "'";
    }
    return std::string("id='") + id() + "'" + (always_highlight()?std::string(" class='hover'"):"");
}

bool RangeType::is_end() const {
    return false;
}

bool RangeTypeEnd::is_end() const {
    return true;
}

std::string RangeTypeIdent::id() const {
    return "ident";
}

std::string RangeTypeFileHeader::id() const {
    return "ehdr";
}

RangeTypeHeaderField::RangeTypeHeaderField(const std::string& v) : value(v) {}

std::string RangeTypeHeaderField::id() const {
    return value;
}

bool RangeTypeHeaderField::always_highlight() {
    return highlight_values.find(value) != highlight_values.end();
}

RangeTypeProgramHeader::RangeTypeProgramHeader(uint32_t v) : value(v) {}

std::string RangeTypeProgramHeader::id() const {
    return std::string("bin_phdr") + std::to_string(value);
}

std::string RangeTypeProgramHeader::class_() const {
    return std::string("phdr");
}

RangeTypeSectionHeader::RangeTypeSectionHeader(uint32_t v) : value(v) {}

std::string RangeTypeSectionHeader::id() const {
    return std::string("bin_shdr") + std::to_string(value);
}

std::string RangeTypeSectionHeader::class_() const {
    return std::string("shdr");
}

RangeTypePhdrField::RangeTypePhdrField(const std::string& v) : value(v) {}

std::string RangeTypePhdrField::class_() const {
    return value + std::string(" phdr_hover");
}

RangeTypeShdrField::RangeTypeShdrField(const std::string& v) : value(v) {}

std::string RangeTypeShdrField::class_() const {
    return value + std::string(" shdr_hover");
}

RangeTypeSegment::RangeTypeSegment(uint16_t v) : value(v) {}

std::string RangeTypeSegment::id() const {
    return std::string("bin_segment") + std::to_string(value);
}

std::string RangeTypeSegment::class_() const {
    return std::string("segment");
}

bool RangeTypeSegment::always_highlight() const {
    return true;
}

RangeTypeSection::RangeTypeSection(uint16_t v) : value(v) {}

std::string RangeTypeSection::id() const {
    return std::string("bin_section") + std::to_string(value);
}

std::string RangeTypeSection::class_() const {
    return std::string("section");
}

bool RangeTypeSection::always_highlight() const {
    return true;
}

std::string RangeTypeSegmentSubrange::class_() const {
    return std::string("segment_subrange");
}

bool RangeTypeSegmentSubrange::always_highlight() const {
    return true;
}

Ranges::~Ranges() {
    for (auto& range : data) {
        for (auto range_type : range) {
            delete range_type;
        }
    }
}

Ranges::Ranges(size_t capacity) {
    data.resize(capacity);
}

void Ranges::add_range(size_t start, size_t end, RangeType* range_type) {
    data[start].emplace_back(range_type);
    data[start + end - 1].emplace_back(new RangeTypeEnd());
}

size_t Ranges::lookup_range_ends(size_t point) const {
    return std::count_if(data[point].begin(), data[point].begin(), [](const auto& item){ return item->is_end(); });
}

ParsedIdent ParsedIdent::from_bytes(const std::vector<uint8_t>& buf) {
    return ParsedIdent{
        {buf[0], buf[1], buf[2], buf[3]},
        buf[static_cast<size_t>(ELF_EI_CLASS)],
        buf[static_cast<size_t>(ELF_EI_DATA)],
        buf[static_cast<size_t>(ELF_EI_VERSION)],
        buf[static_cast<size_t>(ELF_EI_OSABI)],
        buf[static_cast<size_t>(ELF_EI_ABIVERSION)],
    };
}

ParsedElf ParsedElf::from_bytes(const std::string& filename, const std::vector<uint8_t>& buf) {
    if (buf.size() < static_cast<size_t>(ELF_EI_NIDENT)) {
        throw std::runtime_error("file is smaller than ELF header's e_ident");
    }

    auto ident = ParsedIdent::from_bytes(buf);

    if (ident.magic != std::array<uint8_t, 4>{0x7f, 'E', 'L', 'F'}) {
        throw std::runtime_error("mismatched magic: not an ELF file");
    }

    ParsedElf elf{
        filename,
        buf.size(),
        {},
        buf,
        Ranges(buf.size()),
        {},
        {},
        StrTab::empty(),
        0,
        StrTab::empty(),
        {},
    };

    elf.push_file_info();

    elf.push_ident_info(ident);

    if (ident.class_ == ELF_CLASS32) {
        Elf32().parse(buf, ident, elf);
    } else {
        Elf64().parse(buf, ident, elf);
    }

    elf.add_ident_ranges();

    elf.parse_string_tables();

    elf.parse_notes(ident.endianness);

    return elf;
}

void ParsedElf::push_file_info() {
    information.emplace_back("file_name", "File name", filename);

    auto file_size_str = (file_size < 1024)?
        (std::to_string(file_size) + " B"):(human_format_bytes(file_size) + " (" + std::to_string(file_size) + " B)");

    information.emplace_back("file_size", "File size", file_size_str);
}

void ParsedElf::push_ident_info(const ParsedIdent& ident) {
    std::string ident_class;
    switch (ident.class_) {
        case ELF_CLASS32:
            ident_class = "32-bit";
            break;
        case ELF_CLASS64:
            ident_class = "64-bit";
            break;
        default:
            ident_class = "Unknown bitness: " + ident.class_;
    }
    
    information.emplace_back(
        "class",
        "Object class",
        ident_class
    );

    std::string ident_endianness;
    switch (ident.endianness) {
        case ELF_DATA2LSB:
            ident_endianness = "Little endian";
            break;
        case ELF_DATA2MSB:
            ident_endianness = "Big endian";
            break;
        default:
            ident_endianness = "Unknown endianness: " + std::to_string(ident.endianness);
    }
    
    information.emplace_back(
        "data",
        "Data encoding",
        ident_endianness
    );

    if (ident.version != ELF_EV_CURRENT) {
        information.emplace_back(
            "ver",
            "Uncommon version(!)",
            std::to_string(ident.version)
        );
    }

    information.emplace_back(
        "abi",
        (ident.abi == ELF_OSABI_SYSV)?"ABI":"Uncommon ABI(!)",
        abi_to_string(ident.abi)
    );

    if (!(ident.abi == ELF_OSABI_SYSV && ident.abi_ver == 0)) {
        information.emplace_back(
            "abi_ver",
            (ident.abi == ELF_OSABI_SYSV && ident.abi_ver != 0)?"Uncommon ABI version(!)":"ABI version",
            std::to_string(ident.abi_ver)
        );
    }
}

void ParsedElf::add_ident_ranges() {
    ranges.add_range(0, static_cast<size_t>(ELF_EI_NIDENT), new RangeTypeIdent());
    ranges.add_range(0, 4, new RangeTypeHeaderField("magic"));
    ranges.add_range(4, 1, new RangeTypeHeaderField("class"));
    ranges.add_range(5, 1, new RangeTypeHeaderField("data"));
    ranges.add_range(6, 1, new RangeTypeHeaderField("ver"));
    ranges.add_range(7, 1, new RangeTypeHeaderField("abi"));
    ranges.add_range(8, 1, new RangeTypeHeaderField("abi_ver"));
    ranges.add_range(9, 7, new RangeTypeHeaderField("pad"));
}

std::optional<ParsedShdr> ParsedElf::find_strtab_shdr(const std::vector<ParsedShdr>& shdrs) {
    for (const auto& shdr : shdrs) {
        if (shdr.shtype == SHT_STRTAB) {
            return shdr;
        }
    }

    return std::nullopt;
}

void ParsedElf::parse_string_tables() {
    auto shdr = ParsedElf::find_strtab_shdr(shdrs);

    if (shdr) {
        const auto& section = byte_range(contents, shdr->file_offset, shdr->file_offset + shdr->size);

        strtab.populate(section, shdr->size);
    }

    if (shstrndx != SHN_UNDEF) {
        const auto& shdr = shdrs[static_cast<size_t>(shstrndx)];
        const auto& section = byte_range(contents, shdr.file_offset, shdr.file_offset + shdr.size);

        shnstrtab.populate(section, shdr.size);
    }
}

void ParsedElf::parse_notes(uint8_t endianness) {
    std::vector<std::tuple<size_t, size_t>> areas;

    for (const auto& phdr : phdrs) {
        if (phdr.ptype == PT_NOTE) {
            areas.emplace_back(phdr.file_offset, phdr.file_size);
        }
    }

    for (auto[start, len] : areas) {
        parse_note_area(start, len, endianness);
    }
}

// this is pretty ugly in terms of raw addressing, unwieldly offsets, etc.
// area here stands for segment or section because notes may come from either of them.
void ParsedElf::parse_note_area(size_t area_start, size_t area_size, uint8_t endianness) {
    const auto& area = byte_range(contents, area_start, area_start + area_size);
    auto start = 0;

    for (;;) {
        if (start >= area_size) {
            break;
        }

        const auto& [note, len_taken] = Note::from_bytes(byte_range(area, start, area_size), endianness);

        ranges.add_range(
            area_start + start,
            len_taken,
            new RangeTypeSegmentSubrange()
        );
        notes.emplace_back(note);
        start += len_taken;
    }
}

std::tuple<Note, size_t> Note::from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness) {
    auto [namesz, descsz, ntype] = Note::read_header(buf, endianness);

    auto name = byte_range(buf, 12, 12 + namesz);
    auto desc = byte_range(buf, 12 + namesz, 12 + namesz + descsz);

    size_t len = 12 + namesz + descsz;

    while (len % 4 != 0) {
        ++len;
    }

    return {Note{name, desc, ntype}, len};
}

std::tuple<uint32_t, uint32_t, uint32_t> Note::read_header(const std::vector<uint8_t>& buf, uint8_t endianness) {
    if (endianness == ELF_DATA2LSB) {
        return std::make_tuple(
            ::from_le_bytes<uint32_t>(byte_range(buf, 0, 4)),
            ::from_le_bytes<uint32_t>(byte_range(buf, 4, 8)),
            ::from_le_bytes<uint32_t>(byte_range(buf, 8, 12))
        );
    }
    return std::make_tuple(
        from_be_bytes<uint32_t>(byte_range(buf, 0, 4)),
        from_be_bytes<uint32_t>(byte_range(buf, 4, 8)),
        from_be_bytes<uint32_t>(byte_range(buf, 8, 12))
    );
}

// decently ugly
StrTab StrTab::empty() {
    return StrTab{{}, 0};
}

// something could be better than references with lifetimes
void StrTab::populate(const std::vector<uint8_t>& section, size_t section_size) {
    strings = section;
    section_size = section_size;
}

std::string StrTab::get(size_t idx) const {
    auto start_idx = idx;

    for (auto end_idx = start_idx; end_idx < start_idx + section_size; ++end_idx) {
        if (strings[end_idx] == 0) {
            return std::string(strings.cbegin() + start_idx, strings.cbegin() + end_idx);
        }
    }
    return "";
}
