#pragma once

#include <stdexcept>
#include <vector>

#include "defs.hpp"
#include "parser.hpp"

template<class EhdrT, class PhdrT, class ShdrT, class ElfXXAddr, class ElfXXHalf, class ElfXXWord, class ElfXXOff, class ElfXXXword>
struct ElfXX {
    virtual ~ElfXX() = default;

    void parse(const std::vector<uint8_t>& buf, const ParsedIdent& ident, ParsedElf& elf) {
        auto ehdr_size = sizeof(EhdrT);

        if (buf.size() < ehdr_size) {
            throw std::runtime_error("file is smaller than ELF file header");
        }

        auto ehdr = EhdrT::from_bytes(byte_range(buf, 0, ehdr_size), ident.endianness);

        elf.shstrndx = ehdr.e_shstrndx;

        parse_ehdr(ehdr, elf);

        parse_phdrs(buf, ident.endianness, ehdr, elf);

        parse_shdrs(buf, ident.endianness, ehdr, elf);
    }

    void parse_ehdr(const EhdrT& ehdr, ParsedElf& elf) {
        push_ehdr_info(ehdr, elf.information);

        add_ehdr_ranges(ehdr, elf.ranges);
    }

    void push_ehdr_info(const EhdrT& ehdr, std::vector<InfoTuple>& information) {
        information.emplace_back("e_type", "Type", type_to_string(ehdr.e_type));

        information.emplace_back(
            "e_machine",
            "Architecture",
            machine_to_string(ehdr.e_machine)
        );

        information.emplace_back("e_entry", "Entrypoint", int_to_hex(ehdr.e_entry));

        information.emplace_back(
            "ph",
            "Program headers",
            std::string("<span id='info_e_phnum'>") + std::to_string(ehdr.e_phnum) + std::string("</span> * ") +
            std::string("<span id='info_e_phentsize'>") + std::to_string(ehdr.e_phentsize) + std::string("</span> @ ") +
            std::string("<span id='info_e_phoff'>") + std::to_string(ehdr.e_phoff) + std::string("</span>")
        );

        information.emplace_back(
            "sh",
            "Section headers",
            std::string("<span id='info_e_shnum'>") + std::to_string(ehdr.e_shnum) + std::string("</span> * ") +
            std::string("<span id='info_e_shentsize'>") + std::to_string(ehdr.e_shentsize) + std::string("</span> @ ") +
            std::string("<span id='info_e_shoff'>") + std::to_string(ehdr.e_shoff) + std::string("</span>")
        );

        if (static_cast<uint32_t>(ehdr.e_flags) != 0) {
            information.emplace_back("e_flags", "Flags", int_to_hex(ehdr.e_flags));
        }
    }

    virtual void add_ehdr_ranges(const EhdrT& ehdr, Ranges& ranges) = 0;

    void parse_phdrs(const std::vector<uint8_t>& buf, uint8_t endianness, const EhdrT& ehdr, ParsedElf& elf) {
        size_t start = ehdr.e_phoff;
        size_t phsize = sizeof(PhdrT);

        for (int i = 0; i < ehdr.e_phnum; ++i) {
            PhdrT phdr = PhdrT::from_bytes(byte_range(buf, start, start + phsize), endianness);
            auto parsed = parse_phdr(phdr);
            auto& ranges = elf.ranges;

            if (parsed.file_offset != 0 && parsed.file_size != 0) {
                ranges.add_range(parsed.file_offset, parsed.file_size, new RangeTypeSegment(i));
            }

            ranges.add_range(start, phsize, new RangeTypeProgramHeader(static_cast<uint32_t>(i)));

            add_phdr_ranges(start, ranges);

            elf.phdrs.emplace_back(parsed);

            start += phsize;
        }
    }

    ParsedPhdr parse_phdr(const PhdrT& phdr) {
        size_t file_offset = phdr.p_offset;
        size_t file_size = phdr.p_filesz;
        size_t vaddr = phdr.p_vaddr;
        size_t memsz = phdr.p_memsz;
        size_t alignment = phdr.p_align;

        return ParsedPhdr{
            phdr.p_type,
            pflags_to_string(phdr.p_flags),
            file_offset,
            file_size,
            vaddr,
            memsz,
            alignment,
        };
    }

    virtual void add_phdr_ranges(size_t start, Ranges& ranges) = 0;

    void parse_shdrs(const std::vector<uint8_t>& buf, uint8_t endianness, const EhdrT& ehdr, ParsedElf& elf) {
        size_t start = ehdr.e_shoff;
        size_t shsize = sizeof(ShdrT);

        for (int i = 0; i < ehdr.e_shnum; ++i) {
            auto shdr = ShdrT::from_bytes(byte_range(buf, start, start + shsize), endianness);
            auto parsed = parse_shdr(buf, shdr);
            auto& ranges = elf.ranges;

            if (parsed.file_offset != 0 && parsed.size != 0 && parsed.shtype != SHT_NOBITS) {
                ranges.add_range(parsed.file_offset, parsed.size, new RangeTypeSection(i));
            }

            ranges.add_range(start, shsize, new RangeTypeSectionHeader(static_cast<uint32_t>(i)));

            add_shdr_ranges(start, ranges);

            elf.shdrs.emplace_back(parsed);

            start += shsize;
        }
    }

    ParsedShdr parse_shdr(const std::vector<uint8_t>& buf, const ShdrT& shdr) {
        auto name = shdr.sh_name;
        auto addr = shdr.sh_addr;
        auto file_offset = shdr.sh_offset;
        auto size = shdr.sh_size;
        auto link = shdr.sh_link;
        auto info = shdr.sh_info;
        auto addralign = shdr.sh_addralign;
        auto entsize = shdr.sh_entsize;

        return ParsedShdr {
            name,
            shdr.sh_type,
            shdr.sh_flags,
            addr,
            file_offset,
            size,
            link,
            info,
            addralign,
            entsize,
        };
    }

    virtual void add_shdr_ranges(size_t start, Ranges& ranges) = 0;
};