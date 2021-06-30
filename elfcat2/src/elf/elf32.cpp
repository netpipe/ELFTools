#include "include/elf32.hpp"


std::string Elf32Ehdr::describe() {
    return "file header";
}

Elf32Ehdr Elf32Ehdr::from_le_bytes(const std::vector<uint8_t>& buf) {
    return {
        to_array<16>(byte_range(buf, 0, 16)),
        Elf32Half::from_le_bytes(byte_range(buf, 16, 18)),
        Elf32Half::from_le_bytes(byte_range(buf, 18, 20)),
        Elf32Word::from_le_bytes(byte_range(buf, 20, 24)),
        Elf32Addr::from_le_bytes(byte_range(buf, 24, 28)),
        Elf32Off:: from_le_bytes(byte_range(buf, 28, 32)),
        Elf32Off:: from_le_bytes(byte_range(buf, 32, 36)),
        Elf32Word::from_le_bytes(byte_range(buf, 36, 40)),
        Elf32Half::from_le_bytes(byte_range(buf, 40, 42)),
        Elf32Half::from_le_bytes(byte_range(buf, 42, 44)),
        Elf32Half::from_le_bytes(byte_range(buf, 44, 46)),
        Elf32Half::from_le_bytes(byte_range(buf, 46, 48)),
        Elf32Half::from_le_bytes(byte_range(buf, 48, 50)),
        Elf32Half::from_le_bytes(byte_range(buf, 50, 52)),
    };
}

Elf32Ehdr Elf32Ehdr::from_be_bytes(const std::vector<uint8_t>& buf) {
    return Elf32Ehdr{
        to_array<16>(byte_range(buf, 0, 16)),
        Elf32Half::from_be_bytes(byte_range(buf, 16, 18)),
        Elf32Half::from_be_bytes(byte_range(buf, 18, 20)),
        Elf32Word::from_be_bytes(byte_range(buf, 20, 24)),
        Elf32Addr::from_be_bytes(byte_range(buf, 24, 28)),
        Elf32Off:: from_be_bytes(byte_range(buf, 28, 32)),
        Elf32Off:: from_be_bytes(byte_range(buf, 32, 36)),
        Elf32Word::from_be_bytes(byte_range(buf, 36, 40)),
        Elf32Half::from_be_bytes(byte_range(buf, 40, 42)),
        Elf32Half::from_be_bytes(byte_range(buf, 42, 44)),
        Elf32Half::from_be_bytes(byte_range(buf, 44, 46)),
        Elf32Half::from_be_bytes(byte_range(buf, 46, 48)),
        Elf32Half::from_be_bytes(byte_range(buf, 48, 50)),
        Elf32Half::from_be_bytes(byte_range(buf, 50, 52)),
    };
}

Elf32Ehdr Elf32Ehdr::from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness) {
    if (endianness == ELF_DATA2LSB) {
        return from_le_bytes(buf);
    }
    return from_be_bytes(buf);
}

std::string Elf32Phdr::describe() {
    return "program header";
}

Elf32Phdr Elf32Phdr::from_le_bytes(const std::vector<uint8_t>& buf) {
    return Elf32Phdr{
        Elf32Word::from_le_bytes(byte_range(buf, 0, 4)),
        Elf32Off:: from_le_bytes(byte_range(buf, 4, 8)),
        Elf32Addr::from_le_bytes(byte_range(buf, 8, 12)),
        Elf32Addr::from_le_bytes(byte_range(buf, 12, 16)),
        Elf32Word::from_le_bytes(byte_range(buf, 16, 20)),
        Elf32Word::from_le_bytes(byte_range(buf, 20, 24)),
        Elf32Word::from_le_bytes(byte_range(buf, 24, 28)),
        Elf32Word::from_le_bytes(byte_range(buf, 28, 32)),
    };
}

Elf32Phdr Elf32Phdr::from_be_bytes(const std::vector<uint8_t>& buf) {
    return Elf32Phdr{
        Elf32Word::from_be_bytes(byte_range(buf, 0, 4)),
        Elf32Off:: from_be_bytes(byte_range(buf, 4, 8)),
        Elf32Addr::from_be_bytes(byte_range(buf, 8, 12)),
        Elf32Addr::from_be_bytes(byte_range(buf, 12, 16)),
        Elf32Word::from_be_bytes(byte_range(buf, 16, 20)),
        Elf32Word::from_be_bytes(byte_range(buf, 20, 24)),
        Elf32Word::from_be_bytes(byte_range(buf, 24, 28)),
        Elf32Word::from_be_bytes(byte_range(buf, 28, 32)),
    };
}

Elf32Phdr Elf32Phdr::from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness) {
    if (endianness == ELF_DATA2LSB) {
        return from_le_bytes(buf);
    }
    return from_be_bytes(buf);
}

std::string Elf32Shdr::describe() {
    return "section header";
}

Elf32Shdr Elf32Shdr::from_le_bytes(const std::vector<uint8_t>& buf) {
    return  Elf32Shdr{
        Elf32Word::from_le_bytes(byte_range(buf, 0, 4)),
        Elf32Word::from_le_bytes(byte_range(buf, 4, 8)),
        Elf32Word::from_le_bytes(byte_range(buf, 8, 12)),
        Elf32Addr::from_le_bytes(byte_range(buf, 12, 16)),
        Elf32Off:: from_le_bytes(byte_range(buf, 16, 20)),
        Elf32Word::from_le_bytes(byte_range(buf, 20, 24)),
        Elf32Word::from_le_bytes(byte_range(buf, 24, 28)),
        Elf32Word::from_le_bytes(byte_range(buf, 28, 32)),
        Elf32Word::from_le_bytes(byte_range(buf, 32, 36)),
        Elf32Word::from_le_bytes(byte_range(buf, 36, 40)),
    };
}

Elf32Shdr Elf32Shdr::from_be_bytes(const std::vector<uint8_t>& buf) {
    return Elf32Shdr{
        Elf32Word::from_be_bytes(byte_range(buf, 0, 4)),
        Elf32Word::from_be_bytes(byte_range(buf, 4, 8)),
        Elf32Word::from_be_bytes(byte_range(buf, 8, 12)),
        Elf32Addr::from_be_bytes(byte_range(buf, 12, 16)),
        Elf32Off:: from_be_bytes(byte_range(buf, 16, 20)),
        Elf32Word::from_be_bytes(byte_range(buf, 20, 24)),
        Elf32Word::from_be_bytes(byte_range(buf, 24, 28)),
        Elf32Word::from_be_bytes(byte_range(buf, 28, 32)),
        Elf32Word::from_be_bytes(byte_range(buf, 32, 36)),
        Elf32Word::from_be_bytes(byte_range(buf, 36, 40)),
    };
}

Elf32Shdr Elf32Shdr::from_bytes(const std::vector<uint8_t>& buf, uint8_t endianness) {
    if (endianness == ELF_DATA2LSB) {
        return from_le_bytes(buf);
    }
    return from_be_bytes(buf);
}

void Elf32::add_ehdr_ranges(const Elf32Ehdr& ehdr, Ranges& ranges) {
    ranges.add_range(0, static_cast<size_t>(ehdr.e_ehsize), new RangeTypeFileHeader());
    ranges.add_range(16, 2, new RangeTypeHeaderField("e_type"));
    ranges.add_range(18, 2, new RangeTypeHeaderField("e_machine"));
    ranges.add_range(20, 4, new RangeTypeHeaderField("e_version"));
    ranges.add_range(24, 4, new RangeTypeHeaderField("e_entry"));
    ranges.add_range(28, 4, new RangeTypeHeaderField("e_phoff"));
    ranges.add_range(32, 4, new RangeTypeHeaderField("e_shoff"));
    ranges.add_range(36, 4, new RangeTypeHeaderField("e_flags"));
    ranges.add_range(40, 2, new RangeTypeHeaderField("e_ehsize"));
    ranges.add_range(42, 2, new RangeTypeHeaderField("e_phentsize"));
    ranges.add_range(44, 2, new RangeTypeHeaderField("e_phnum"));
    ranges.add_range(46, 2, new RangeTypeHeaderField("e_shentsize"));
    ranges.add_range(48, 2, new RangeTypeHeaderField("e_shnum"));
    ranges.add_range(50, 2, new RangeTypeHeaderField("e_shstrndx"));
}

void Elf32::add_phdr_ranges(size_t start, Ranges& ranges) {
    ranges.add_range(start +  0, 4, new RangeTypePhdrField("p_type"));
    ranges.add_range(start +  4, 4, new RangeTypePhdrField("p_offset"));
    ranges.add_range(start +  8, 4, new RangeTypePhdrField("p_vaddr"));
    ranges.add_range(start + 12, 4, new RangeTypePhdrField("p_paddr"));
    ranges.add_range(start + 16, 4, new RangeTypePhdrField("p_filesz"));
    ranges.add_range(start + 20, 4, new RangeTypePhdrField("p_memsz"));
    ranges.add_range(start + 24, 4, new RangeTypePhdrField("p_flags"));
    ranges.add_range(start + 28, 4, new RangeTypePhdrField("p_align"));
}

void Elf32::add_shdr_ranges(size_t start, Ranges& ranges) {
    ranges.add_range(start +  0, 4, new RangeTypeShdrField("sh_name"));
    ranges.add_range(start +  4, 4, new RangeTypeShdrField("sh_type"));
    ranges.add_range(start +  8, 4, new RangeTypeShdrField("sh_flags"));
    ranges.add_range(start + 12, 4, new RangeTypeShdrField("sh_addr"));
    ranges.add_range(start + 16, 4, new RangeTypeShdrField("sh_offset"));
    ranges.add_range(start + 20, 4, new RangeTypeShdrField("sh_size"));
    ranges.add_range(start + 24, 4, new RangeTypeShdrField("sh_link"));
    ranges.add_range(start + 28, 4, new RangeTypeShdrField("sh_info"));
    ranges.add_range(start + 32, 4, new RangeTypeShdrField("sh_addralign"));
    ranges.add_range(start + 36, 4, new RangeTypeShdrField("sh_entsize"));
}

