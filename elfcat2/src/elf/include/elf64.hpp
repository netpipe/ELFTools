#pragma once

#include <utils.hpp>

#include <array>
#include <cstdint>
#include <string>

#include "elfxx.hpp"
#include "parser.hpp"


using Elf64Addr = ser_integral_t<uint64_t>;
using Elf64Off = ser_integral_t<uint64_t>;
using Elf64Half = ser_integral_t<uint16_t>;
using Elf64Word = ser_integral_t<uint32_t>;
using Elf64Xword = ser_integral_t<uint64_t>;


struct Elf64Ehdr {
    static std::string describe();
    static Elf64Ehdr from_le_bytes(const std::vector<uint8_t>& buf);
    static Elf64Ehdr from_be_bytes(const std::vector<uint8_t>& buf);
    static Elf64Ehdr from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness);

    std::array<uint8_t, 16> e_ident;
    Elf64Half e_type;
    Elf64Half e_machine;
    Elf64Word e_version;
    Elf64Addr e_entry;
    Elf64Off e_phoff;
    Elf64Off e_shoff;
    Elf64Word e_flags;
    Elf64Half e_ehsize;
    Elf64Half e_phentsize;
    Elf64Half e_phnum;
    Elf64Half e_shentsize;
    Elf64Half e_shnum;
    Elf64Half e_shstrndx;
};

struct Elf64Phdr {
    static std::string describe();
    static Elf64Phdr from_le_bytes(const std::vector<uint8_t>& buf);
    static Elf64Phdr from_be_bytes(const std::vector<uint8_t>& buf);
    static Elf64Phdr from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness);

    Elf64Word p_type;
    Elf64Word p_flags;
    Elf64Off p_offset;
    Elf64Addr p_vaddr;
    Elf64Addr p_paddr;
    Elf64Xword p_filesz;
    Elf64Xword p_memsz;
    Elf64Xword p_align;
};

struct Elf64Shdr {
    static std::string describe();
    static Elf64Shdr from_le_bytes(const std::vector<uint8_t>& buf);
    static Elf64Shdr from_be_bytes(const std::vector<uint8_t>& buf);
    static Elf64Shdr from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness);

    Elf64Word sh_name;
    Elf64Word sh_type;
    Elf64Xword sh_flags;
    Elf64Addr sh_addr;
    Elf64Off sh_offset;
    Elf64Xword sh_size;
    Elf64Word sh_link;
    Elf64Word sh_info;
    Elf64Xword sh_addralign;
    Elf64Xword sh_entsize;
};


struct Elf64 : ElfXX<Elf64Ehdr, Elf64Phdr, Elf64Shdr, Elf64Addr, Elf64Half, Elf64Word, Elf64Off, Elf64Xword> {
    void add_ehdr_ranges(const Elf64Ehdr& ehdr, Ranges& ranges);
    void add_phdr_ranges(size_t start, Ranges& ranges);
    void add_shdr_ranges(size_t start, Ranges& ranges);
};