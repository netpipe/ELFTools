#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <utils.hpp>

#include "elfxx.hpp"


using Elf32Addr = ser_integral_t<uint32_t>;
using Elf32Half = ser_integral_t<uint16_t>;
using Elf32Off = ser_integral_t<uint32_t>;
using Elf32Word = ser_integral_t<uint32_t>;


struct Elf32Ehdr {
    static std::string describe();
    static Elf32Ehdr from_le_bytes(const std::vector<uint8_t>& buf);
    static Elf32Ehdr from_be_bytes(const std::vector<uint8_t>& buf);
    static Elf32Ehdr from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness);

    std::array<uint8_t, 16> e_ident;
    Elf32Half e_type;
    Elf32Half e_machine;
    Elf32Word e_version;
    Elf32Addr e_entry;
    Elf32Off e_phoff;
    Elf32Off e_shoff;
    Elf32Word e_flags;
    Elf32Half e_ehsize;
    Elf32Half e_phentsize;
    Elf32Half e_phnum;
    Elf32Half e_shentsize;
    Elf32Half e_shnum;
    Elf32Half e_shstrndx;
};


struct Elf32Phdr {
    static std::string describe();
    static Elf32Phdr from_le_bytes(const std::vector<uint8_t>& buf);
    static Elf32Phdr from_be_bytes(const std::vector<uint8_t>& buf);
    static Elf32Phdr from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness);

    Elf32Word p_type;
    Elf32Off p_offset;
    Elf32Addr p_vaddr;
    Elf32Addr p_paddr;
    Elf32Word p_filesz;
    Elf32Word p_memsz;
    Elf32Word p_flags;
    Elf32Word p_align;
};


struct Elf32Shdr {
    static std::string describe();
    static Elf32Shdr from_le_bytes(const std::vector<uint8_t>& buf);
    static Elf32Shdr from_be_bytes(const std::vector<uint8_t>& buf);
    static Elf32Shdr from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness);

    Elf32Word sh_name;
    Elf32Word sh_type;
    Elf32Word sh_flags;
    Elf32Addr sh_addr;
    Elf32Off sh_offset;
    Elf32Word sh_size;
    Elf32Word sh_link;
    Elf32Word sh_info;
    Elf32Word sh_addralign;
    Elf32Word sh_entsize;
};


struct Elf32 : ElfXX<Elf32Ehdr, Elf32Phdr, Elf32Shdr, Elf32Addr, Elf32Half, Elf32Word, Elf32Off, Elf32Word> {
    void add_ehdr_ranges(const Elf32Ehdr& ehdr, Ranges& ranges);
    void add_phdr_ranges(size_t start, Ranges& ranges);
    void add_shdr_ranges(size_t start, Ranges& ranges);
};
