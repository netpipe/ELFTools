#include <map>
#include <string>
#include <sstream>

#include "include/defs.hpp"


std::string type_to_string(uint16_t e_type) {
    static const std::map<uint16_t, std::string> type_mapping{
        {ELF_ET_NONE, "None (NONE)"},
        {ELF_ET_REL, "Relocatable object file (REL)"},
        {ELF_ET_EXEC, "Executable file (EXEC)"},
        {ELF_ET_DYN, "Shared object file (DYN)"},
        {ELF_ET_CORE, "Core file (CORE)"},
        {ELF_ET_LOOS | ELF_ET_HIOS, "Environment-specific use"},
        {ELF_ET_LOPROC | ELF_ET_HIPROC, "Processor-specific use"},
    };
    
    auto iterator = type_mapping.find(e_type);
    if (iterator != type_mapping.end()) {
        return iterator->second;
    }
    
    return std::string("Unknown ") + std::to_string(e_type);
}

std::string abi_to_string(uint8_t abi) {
    static const std::map<uint8_t, std::string> abi_mapping {
{        ELF_OSABI_SYSV, "SysV"},
{        ELF_OSABI_HPUX, "HP-UX"},
{        2, "NetBSD"},
{        3, "Linux"},
{        4, "Hurd"},
{        6, "Solaris"},
{        7, "AIX"},
{        8, "IRIX"},
{        9, "FreeBSD"},
{        10, "Tru64"},
{        11, "Modesto"},
{        12, "OpenBSD"},
{        13, "OpenVMS"},
{        255, "Standalone"},
    };
    
    auto iterator = abi_mapping.find(abi);
    if (iterator != abi_mapping.end()) {
        return iterator->second;
    }

    return std::string("Unknown ") + std::to_string(abi);
}

std::string machine_to_string(uint16_t e_machine) {
    static const std::map<uint16_t, std::string> machine_mapping {
{        0, "None"},
{        1, "AT&T WE 32100"},
{        2, "SPARC"},
{        3, "x86"},
{        4, "Motorolla 68000"},
{        5, "Motorolla 88000"},
{        6, "Intel MCU"},
{        7, "Intel 80860"},
{        8, "MIPS"},
{        9, "IBM System/370"},
{        10, "MIPS RS3000 little-endian"},
{        14, "HP PA-RISC"},
{        19, "Intel 80960"},
{        20, "PowerPC"},
{        21, "PowerPC 64-bit"},
{        22, "S390"},
{        40, "ARM Aarch32"},
{        50, "Itanium IA-64"},
{        62, "x86-64"},
{        183, "ARM Aarch64"},
{        190, "CUDA"},
{        224, "AMDGPU"},
{        243, "RISC-V"},
    };

    auto iterator = machine_mapping.find(e_machine);
    if (iterator != machine_mapping.end()) {
        return iterator->second;
    }

    return std::string("Unknown ") + std::to_string(e_machine);
}

std::string ptype_to_string(uint32_t ptype) {
    static const std::map<uint32_t, std::string> type_mapping {
{        PT_NULL, "NULL"},
{        PT_LOAD, "LOAD"},
{        PT_DYNAMIC, "DYNAMIC"},
{        PT_INTERP, "INTERP"},
{        PT_NOTE, "NOTE"},
{        PT_SHLIB, "SHLIB"},
{        PT_PHDR, "PHDR"},
{        PT_TLS, "TLS"},
{        PT_LOOS, "LOOS"},
{        PT_GNU_EH_FRAME, "GNU_EH_FRAME (OS-specific)"},
{        PT_GNU_STACK, "GNU_STACK (OS-specific)"},
{        PT_GNU_RELRO, "GNU_RELRO (OS-specific)"},
{        PT_HIOS, "HIOS"},
{        PT_LOPROC, "LOPROC"},
{        PT_HIPROC, "HIPROC"},
    };

    auto iterator = type_mapping.find(ptype);
    if (iterator != type_mapping.end()) {
        return iterator->second;
    }

    return std::string("Unknown ") + std::to_string(ptype);
}

std::string pflags_to_string(uint32_t flags) {
    std::stringstream s;

    if (flags & PF_R) {
        s << 'R';
    }

    if (flags & PF_W) {
        s << 'W';
    }

    if (flags & PF_X) {
        s << 'X';
    }

    return s.str();
}

std::string shtype_to_string(uint32_t shtype) {
    static const std::map<uint32_t, std::string> shtype_mapping {
{        SHT_NULL, "NULL"},
{        SHT_PROGBITS, "PROGBITS"},
{        SHT_SYMTAB, "SYMTAB"},
{        SHT_STRTAB, "STRTAB"},
{        SHT_RELA, "RELA"},
{        SHT_HASH, "HASH"},
{        SHT_DYNAMIC, "DYNAMIC"},
{        SHT_NOTE, "NOTE"},
{        SHT_NOBITS, "NOBITS"},
{        SHT_REL, "REL"},
{        SHT_SHLIB, "SHLIB"},
{        SHT_INIT_ARRAY, "INIT_ARRAY"},
{        SHT_FINI_ARRAY, "FINI_ARRAY"},
{        SHT_DYNSYM, "DYNSYM"},
{        SHT_LOOS, "LOOS"},
{        SHT_GNU_HASH, "GNU_HASH (OS-specific)"},
{        SHT_VER_NEED, "VER_NEED (OS-specific)"},
{        SHT_HIOS, "HIOS"},
{        SHT_LOPROC, "LOPROC"},
{        SHT_HIPROC, "HIPROC"},
    };

    auto iterator = shtype_mapping.find(shtype);
    if (iterator != shtype_mapping.end()) {
        return iterator->second;
    }

    return std::string("Unknown ") + std::to_string(shtype);
}

std::string shflags_to_string(uint64_t flags) {
    std::stringstream s;

    if (flags & SHF_WRITE) {
        s << 'W';
    }

    if (flags & SHF_ALLOC) {
        s << 'A';
    }

    if (flags & SHF_EXECINSTR) {
        s << 'X';
    }

    if (s.tellp() == std::streampos(0)) {
        s << '0';
    }

    return s.str();
}
