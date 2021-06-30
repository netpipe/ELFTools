#include "include/elf64.hpp"


std::string Elf64Ehdr::describe() {
    return "file header";
}

Elf64Ehdr Elf64Ehdr::from_le_bytes(const std::vector<uint8_t>& buf) {
    return Elf64Ehdr{
        to_array<16>(byte_range(buf, 0, 16)),
        Elf64Half::from_le_bytes(byte_range(buf, 16, 18)),
        Elf64Half::from_le_bytes(byte_range(buf, 18, 20)),
        Elf64Word::from_le_bytes(byte_range(buf, 20, 24)),
        Elf64Addr::from_le_bytes(byte_range(buf, 24, 32)),
        Elf64Off:: from_le_bytes(byte_range(buf, 32, 40)),
        Elf64Off:: from_le_bytes(byte_range(buf, 40, 48)),
        Elf64Word::from_le_bytes(byte_range(buf, 48, 52)),
        Elf64Half::from_le_bytes(byte_range(buf, 52, 54)),
        Elf64Half::from_le_bytes(byte_range(buf, 54, 56)),
        Elf64Half::from_le_bytes(byte_range(buf, 56, 58)),
        Elf64Half::from_le_bytes(byte_range(buf, 58, 60)),
        Elf64Half::from_le_bytes(byte_range(buf, 60, 62)),
        Elf64Half::from_le_bytes(byte_range(buf, 62, 64)),
    };
}

Elf64Ehdr Elf64Ehdr::from_be_bytes(const std::vector<uint8_t>& buf) {
    return Elf64Ehdr{
        to_array<16>(byte_range(buf, 0, 16)),
        Elf64Half::from_be_bytes(byte_range(buf, 16, 18)),
        Elf64Half::from_be_bytes(byte_range(buf, 18, 20)),
        Elf64Word::from_be_bytes(byte_range(buf, 20, 24)),
        Elf64Addr::from_be_bytes(byte_range(buf, 24, 32)),
        Elf64Off:: from_be_bytes(byte_range(buf, 32, 40)),
        Elf64Off:: from_be_bytes(byte_range(buf, 40, 48)),
        Elf64Word::from_be_bytes(byte_range(buf, 48, 52)),
        Elf64Half::from_be_bytes(byte_range(buf, 52, 54)),
        Elf64Half::from_be_bytes(byte_range(buf, 54, 56)),
        Elf64Half::from_be_bytes(byte_range(buf, 56, 58)),
        Elf64Half::from_be_bytes(byte_range(buf, 58, 60)),
        Elf64Half::from_be_bytes(byte_range(buf, 60, 62)),
        Elf64Half::from_be_bytes(byte_range(buf, 62, 64)),
    };
}

Elf64Ehdr Elf64Ehdr::from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness) {
    if (endianness == ELF_DATA2LSB) {
        return from_le_bytes(buf);
    }
    return from_be_bytes(buf);
}

std::string Elf64Phdr::describe() {
    return "program header";
}

Elf64Phdr Elf64Phdr::from_le_bytes(const std::vector<uint8_t>& buf) {
    return Elf64Phdr{
        Elf64Word:: from_le_bytes(byte_range(buf, 0, 4)),
        Elf64Word:: from_le_bytes(byte_range(buf, 4, 8)),
        Elf64Off::  from_le_bytes(byte_range(buf, 8, 16)),
        Elf64Addr:: from_le_bytes(byte_range(buf, 16, 24)),
        Elf64Addr:: from_le_bytes(byte_range(buf, 24, 32)),
        Elf64Xword::from_le_bytes(byte_range(buf, 32, 40)),
        Elf64Xword::from_le_bytes(byte_range(buf, 40, 48)),
        Elf64Xword::from_le_bytes(byte_range(buf, 48, 56)),
    };
}

Elf64Phdr Elf64Phdr::from_be_bytes(const std::vector<uint8_t>& buf) {
    return Elf64Phdr{
        Elf64Word:: from_be_bytes(byte_range(buf, 0, 4)),
        Elf64Word:: from_be_bytes(byte_range(buf, 4, 8)),
        Elf64Off::  from_be_bytes(byte_range(buf, 8, 16)),
        Elf64Addr:: from_be_bytes(byte_range(buf, 16, 24)),
        Elf64Addr:: from_be_bytes(byte_range(buf, 24, 32)),
        Elf64Xword::from_be_bytes(byte_range(buf, 32, 40)),
        Elf64Xword::from_be_bytes(byte_range(buf, 40, 48)),
        Elf64Xword::from_be_bytes(byte_range(buf, 48, 56)),
    };
}

Elf64Phdr Elf64Phdr::from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness) {
    if (endianness == ELF_DATA2LSB) {
        return from_le_bytes(buf);
    }
    return from_be_bytes(buf);
}

std::string Elf64Shdr::describe() {
    return "section header";
}

Elf64Shdr Elf64Shdr::from_le_bytes(const std::vector<uint8_t>& buf) {
    return Elf64Shdr{
        Elf64Word:: from_le_bytes(byte_range(buf, 0, 4)),
        Elf64Word:: from_le_bytes(byte_range(buf, 4, 8)),
        Elf64Xword::from_le_bytes(byte_range(buf, 8, 16)),
        Elf64Addr:: from_le_bytes(byte_range(buf, 16, 24)),
        Elf64Off::  from_le_bytes(byte_range(buf, 24, 32)),
        Elf64Xword::from_le_bytes(byte_range(buf, 32, 40)),
        Elf64Word:: from_le_bytes(byte_range(buf, 40, 44)),
        Elf64Word:: from_le_bytes(byte_range(buf, 44, 48)),
        Elf64Xword::from_le_bytes(byte_range(buf, 48, 56)),
        Elf64Xword::from_le_bytes(byte_range(buf, 56, 64)),
    };
}

Elf64Shdr Elf64Shdr::from_be_bytes(const std::vector<uint8_t>& buf) {
    return Elf64Shdr{
        Elf64Word:: from_be_bytes(byte_range(buf, 0, 4)),
        Elf64Word:: from_be_bytes(byte_range(buf, 4, 8)),
        Elf64Xword::from_be_bytes(byte_range(buf, 8, 16)),
        Elf64Addr:: from_be_bytes(byte_range(buf, 16, 24)),
        Elf64Off::  from_be_bytes(byte_range(buf, 24, 32)),
        Elf64Xword::from_be_bytes(byte_range(buf, 32, 40)),
        Elf64Word:: from_be_bytes(byte_range(buf, 40, 44)),
        Elf64Word:: from_be_bytes(byte_range(buf, 44, 48)),
        Elf64Xword::from_be_bytes(byte_range(buf, 48, 56)),
        Elf64Xword::from_be_bytes(byte_range(buf, 56, 64)),
    };
}

Elf64Shdr Elf64Shdr::from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness) {
    if (endianness == ELF_DATA2LSB) {
        return from_le_bytes(buf);
    }
    return from_be_bytes(buf);
}

void Elf64::add_ehdr_ranges(const Elf64Ehdr& ehdr, Ranges& ranges) {
    ranges.add_range(0, static_cast<uint32_t>(ehdr.e_ehsize), new RangeTypeFileHeader());
    ranges.add_range(16, 2, new RangeTypeHeaderField("e_type"));
    ranges.add_range(18, 2, new RangeTypeHeaderField("e_machine"));
    ranges.add_range(20, 4, new RangeTypeHeaderField("e_version"));
    ranges.add_range(24, 8, new RangeTypeHeaderField("e_entry"));
    ranges.add_range(32, 8, new RangeTypeHeaderField("e_phoff"));
    ranges.add_range(40, 8, new RangeTypeHeaderField("e_shoff"));
    ranges.add_range(48, 4, new RangeTypeHeaderField("e_flags"));
    ranges.add_range(52, 2, new RangeTypeHeaderField("e_ehsize"));
    ranges.add_range(54, 2, new RangeTypeHeaderField("e_phentsize"));
    ranges.add_range(56, 2, new RangeTypeHeaderField("e_phnum"));
    ranges.add_range(58, 2, new RangeTypeHeaderField("e_shentsize"));
    ranges.add_range(60, 2, new RangeTypeHeaderField("e_shnum"));
    ranges.add_range(62, 2, new RangeTypeHeaderField("e_shstrndx"));
}

void Elf64::add_phdr_ranges(size_t start, Ranges& ranges) {
    ranges.add_range(start +  0, 4, new RangeTypePhdrField("p_type"));
    ranges.add_range(start +  4, 4, new RangeTypePhdrField("p_flags"));
    ranges.add_range(start +  8, 8, new RangeTypePhdrField("p_offset"));
    ranges.add_range(start + 16, 8, new RangeTypePhdrField("p_vaddr"));
    ranges.add_range(start + 24, 8, new RangeTypePhdrField("p_paddr"));
    ranges.add_range(start + 32, 8, new RangeTypePhdrField("p_filesz"));
    ranges.add_range(start + 40, 8, new RangeTypePhdrField("p_memsz"));
    ranges.add_range(start + 48, 8, new RangeTypePhdrField("p_align"));
}

void Elf64::add_shdr_ranges(size_t start, Ranges& ranges) {
    ranges.add_range(start +  0, 4, new RangeTypeShdrField("sh_name"));
    ranges.add_range(start +  4, 4, new RangeTypeShdrField("sh_type"));
    ranges.add_range(start +  8, 8, new RangeTypeShdrField("sh_flags"));
    ranges.add_range(start + 16, 8, new RangeTypeShdrField("sh_addr"));
    ranges.add_range(start + 24, 8, new RangeTypeShdrField("sh_offset"));
    ranges.add_range(start + 32, 8, new RangeTypeShdrField("sh_size"));
    ranges.add_range(start + 40, 4, new RangeTypeShdrField("sh_link"));
    ranges.add_range(start + 44, 4, new RangeTypeShdrField("sh_info"));
    ranges.add_range(start + 48, 8, new RangeTypeShdrField("sh_addralign"));
    ranges.add_range(start + 56, 8, new RangeTypeShdrField("sh_entsize"));
}

