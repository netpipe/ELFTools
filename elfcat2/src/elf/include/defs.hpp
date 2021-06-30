#pragma once

#include <cstdint>
#include <string>

constexpr uint8_t ELF_EI_MAG0 = 0;
constexpr uint8_t ELF_EI_MAG1 = 1;
constexpr uint8_t ELF_EI_MAG2 = 2;
constexpr uint8_t ELF_EI_MAG3 = 3;
constexpr uint8_t ELF_EI_CLASS = 4;
constexpr uint8_t ELF_EI_DATA = 5;
constexpr uint8_t ELF_EI_VERSION = 6;
constexpr uint8_t ELF_EI_OSABI = 7;
constexpr uint8_t ELF_EI_ABIVERSION = 8;
constexpr uint8_t ELF_EI_PAD = 9;
constexpr uint8_t ELF_EI_NIDENT = 16;

constexpr uint8_t ELF_CLASS32 = 1;
constexpr uint8_t ELF_CLASS64 = 2;

constexpr uint8_t ELF_DATA2LSB = 1;
constexpr uint8_t ELF_DATA2MSB = 2;

constexpr uint8_t ELF_EV_CURRENT = 1;

constexpr uint8_t ELF_OSABI_SYSV = 0;
constexpr uint8_t ELF_OSABI_HPUX = 1;

constexpr uint16_t ELF_ET_NONE = 0;
constexpr uint16_t ELF_ET_REL = 1;
constexpr uint16_t ELF_ET_EXEC = 2;
constexpr uint16_t ELF_ET_DYN = 3;
constexpr uint16_t ELF_ET_CORE = 4;
constexpr uint16_t ELF_ET_LOOS = 0xfe00;
constexpr uint16_t ELF_ET_HIOS = 0xfeff;
constexpr uint16_t ELF_ET_LOPROC = 0xff00;
constexpr uint16_t ELF_ET_HIPROC = 0xffff;

constexpr uint32_t PT_NULL = 0;
constexpr uint32_t PT_LOAD = 1;
constexpr uint32_t PT_DYNAMIC = 2;
constexpr uint32_t PT_INTERP = 3;
constexpr uint32_t PT_NOTE = 4;
constexpr uint32_t PT_SHLIB = 5;
constexpr uint32_t PT_PHDR = 6;
constexpr uint32_t PT_TLS = 7;
constexpr uint32_t PT_LOOS = 0x60000000;
constexpr uint32_t PT_GNU_EH_FRAME = 0x6474e550;
constexpr uint32_t PT_GNU_STACK = 0x6474e551;
constexpr uint32_t PT_GNU_RELRO = 0x6474e552;
constexpr uint32_t PT_HIOS = 0x6fffffff;
constexpr uint32_t PT_LOPROC = 0x70000000;
constexpr uint32_t PT_HIPROC = 0x7fffffff;

constexpr uint32_t PF_X = 0b001;
constexpr uint32_t PF_W = 0b010;
constexpr uint32_t PF_R = 0b100;
constexpr uint32_t PF_MASKOS = 0x00ff0000;
constexpr uint32_t PF_MASKPROC = 0xff000000;

constexpr uint32_t NT_GNU_BUILD_ID = 0x3;

constexpr uint16_t SHN_UNDEF = 0;

constexpr uint32_t SHT_NULL = 0;
constexpr uint32_t SHT_PROGBITS = 1;
constexpr uint32_t SHT_SYMTAB = 2;
constexpr uint32_t SHT_STRTAB = 3;
constexpr uint32_t SHT_RELA = 4;
constexpr uint32_t SHT_HASH = 5;
constexpr uint32_t SHT_DYNAMIC = 6;
constexpr uint32_t SHT_NOTE = 7;
constexpr uint32_t SHT_NOBITS = 8;
constexpr uint32_t SHT_REL = 9;
constexpr uint32_t SHT_SHLIB = 10;
constexpr uint32_t SHT_DYNSYM = 11;
constexpr uint32_t SHT_INIT_ARRAY = 14;
constexpr uint32_t SHT_FINI_ARRAY = 15;
constexpr uint32_t SHT_LOOS = 0x60000000;
constexpr uint32_t SHT_GNU_HASH = 0x6ffffff6;
constexpr uint32_t SHT_VER_NEED = 0x6ffffffe;
constexpr uint32_t SHT_HIOS = 0x6fffffff;
constexpr uint32_t SHT_LOPROC = 0x70000000;
constexpr uint32_t SHT_HIPROC = 0x7fffffff;

constexpr uint64_t SHF_WRITE = 0b001;
constexpr uint64_t SHF_ALLOC = 0b010;
constexpr uint64_t SHF_EXECINSTR = 0b100;
constexpr uint64_t SHF_MASKOS = 0x0f000000;
constexpr uint64_t SHF_MASKPROC = 0xf0000000;


std::string type_to_string(uint16_t e_type);
std::string abi_to_string(uint8_t abi);
std::string machine_to_string(uint16_t e_machine);
std::string ptype_to_string(uint32_t ptype);
std::string pflags_to_string(uint32_t flags);
std::string shtype_to_string(uint32_t shtype);
std::string shflags_to_string(uint64_t flags);
