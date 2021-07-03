#pragma once

#include <array>
#include <memory>
#include <tuple>
#include <vector>
#include <set>

#include "defs.hpp"
//#include "elf32.hpp"
//#include "elf64.hpp"
//#include "elfxx.hpp"


using InfoTuple = std::tuple<std::string, std::string, std::string>;


struct RangeType {
    virtual ~RangeType() = default;

    virtual bool needs_class() const = 0;
    virtual bool needs_id() const = 0;
    virtual std::string id() const = 0;
    virtual std::string class_() const = 0;
    virtual bool always_highlight() const = 0;
    std::string span_attributes() const;
    virtual bool skippable() const = 0;
    virtual bool is_end() const;
};


template<bool needs_class_v = false, bool needs_id_v = false, bool skippable_v = false>
struct ConfigurableRangeType : RangeType {
    bool needs_class() const {
        return needs_class_v;
    }

    bool needs_id() const {
        return needs_id_v;
    }

    std::string id() const {
        return "";
    }

    std::string class_() const {
        return "";
    }

    bool always_highlight() const {
        return false;
    }

    std::string span_attributes() const {
        return "";
    }

    bool skippable() const {
        return skippable_v;
    }
};


struct RangeTypeEnd : ConfigurableRangeType<> {
    bool is_end() const;
};


struct RangeTypeIdent : ConfigurableRangeType<> {
    std::string id() const;
};


struct RangeTypeFileHeader : ConfigurableRangeType<> {
    std::string id() const;
};


struct RangeTypeHeaderField : ConfigurableRangeType<false, true> {
    RangeTypeHeaderField(const std::string& v);

    std::string id() const;
    bool always_highlight();

    const std::string value;

    inline static const std::set<std::string> highlight_values = {
        "magic",
        "ver",
        "abi_ver",
        "pad",
        "e_version",
        "e_flags",
        "e_ehsize",
        "e_shstrndx",
    };
};


struct RangeTypeProgramHeader : ConfigurableRangeType<true, true> {
    RangeTypeProgramHeader(uint32_t v);

    std::string id() const;
    std::string class_() const;

    const uint32_t value;
};


struct RangeTypeSectionHeader : ConfigurableRangeType<true, true> {
    RangeTypeSectionHeader(uint32_t v);

    std::string id() const;
    std::string class_() const;

    const uint32_t value;
};


struct RangeTypePhdrField : ConfigurableRangeType<true> {
    RangeTypePhdrField(const std::string& v);

    std::string class_() const;

    std::string value;
};


struct RangeTypeShdrField : ConfigurableRangeType<true> {
    RangeTypeShdrField(const std::string& v);

    std::string class_() const;

    std::string value;
};


struct RangeTypeSegment : ConfigurableRangeType<true, true, true> {
    RangeTypeSegment(uint16_t v);

    std::string id() const;
    std::string class_() const;
    bool always_highlight() const;

    const uint16_t value;
};


struct RangeTypeSection : ConfigurableRangeType<true, true, true> {
    RangeTypeSection(uint16_t v);

    std::string id() const;
    std::string class_() const;
    bool always_highlight() const;

    const uint16_t value;
};


struct RangeTypeSegmentSubrange : ConfigurableRangeType<true, false, true> {
    std::string class_() const;
    bool always_highlight() const;
};


// Interval tree that allows querying point for all intervals that intersect it should be better.
// We can't beat O(n * m) but the average case should improve.
struct Ranges {
    ~Ranges();
    Ranges(size_t capacity);
    void add_range(size_t start, size_t end, RangeType* range_type);
    size_t lookup_range_ends(size_t point) const;

    std::vector<std::vector<RangeType*>> data;
};


struct ParsedIdent {
    static ParsedIdent from_bytes(const std::vector<uint8_t>& buf);

    std::array<uint8_t, 4> magic;
    uint8_t class_;
    uint8_t endianness;
    uint8_t version;
    uint8_t abi;
    uint8_t abi_ver;
};


struct StrTab {
    static StrTab empty();
    void populate(const std::vector<uint8_t>& section, size_t section_size);
    std::string get(size_t idx) const;

    std::vector<uint8_t> strings;
    size_t section_size;
};


struct Note {
    static std::tuple<Note, size_t> from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness);
    static std::tuple<uint32_t, uint32_t, uint32_t> read_header(const std::vector<uint8_t>& buf, uint8_t endianness);

    std::vector<uint8_t> name;
    std::vector<uint8_t> desc;
    uint32_t ntype;
};


struct ParsedPhdr {
    uint32_t ptype;
    std::string flags;
    size_t file_offset;
    size_t file_size;
    size_t vaddr;
    size_t memsz;
    size_t alignment;
};


struct ParsedShdr {
    size_t name;
    uint32_t shtype;
    uint64_t flags;
    size_t addr;
    size_t file_offset;
    size_t size;
    size_t link;
    size_t info;
    size_t addralign;
    size_t entsize;
};


struct ParsedElf {
    static ParsedElf from_bytes(const std::string& filename, const std::vector<uint8_t>& buf);
    void push_file_info();
    void push_ident_info(const ParsedIdent& ident);
    void add_ident_ranges();
    std::optional<ParsedShdr> find_strtab_shdr(const std::vector<ParsedShdr>& shdrs);
    void parse_string_tables();
    void parse_notes(uint8_t endianness);
    void parse_note_area(size_t area_start, size_t area_size, uint8_t endianness);

    std::string filename;
    size_t file_size;
    std::vector<std::tuple<std::string, std::string, std::string>> information;
    std::vector<uint8_t> contents;
    Ranges ranges;
    std::vector<ParsedPhdr> phdrs;
    std::vector<ParsedShdr> shdrs;
    StrTab strtab;
    uint16_t shstrndx;
    StrTab shnstrtab;
    std::vector<Note> notes;
};
