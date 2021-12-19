/*
 * nologin   http://www.nologin.org
 * --------------------------------
 *
 * ELF manipulation library
 *
 * skape
 * mmiller@hick.org
 */
#include "melf.h"

void melf_elfSetRaw(MELF *melf, Elf32_Ehdr *header)
{
	memcpy(&melf->header, header, sizeof(Elf32_Ehdr));
}

Elf32_Ehdr *melf_elfGetRaw(MELF *melf)
{
	return &melf->header;
}

void melf_elfSetType(MELF *melf, Elf32_Half type)
{
	melf->header.e_type = type;
}

Elf32_Half melf_elfGetType(MELF *melf)
{
	return melf->header.e_type;
}

void melf_elfSetMachine(MELF *melf, Elf32_Half machine)
{
	melf->header.e_machine = machine;
}

Elf32_Half melf_elfGetMachine(MELF *melf)
{
	return melf->header.e_machine;
}

void melf_elfSetVersion(MELF *melf, Elf32_Word version)
{
	melf->header.e_version = version;
}

Elf32_Word melf_elfGetVersion(MELF *melf)
{
	return melf->header.e_version;
}

void melf_elfSetEntry(MELF *melf, Elf32_Addr entry)
{
	melf->header.e_entry = entry;
}

Elf32_Addr melf_elfGetEntry(MELF *melf)
{
	return melf->header.e_entry;
}

void melf_elfSetProgramHeaderOffset(MELF *melf, Elf32_Off offset)
{
	melf->header.e_phoff = offset;
}

Elf32_Off melf_elfGetProgramHeaderOffset(MELF *melf)
{
	return melf->header.e_phoff;
}

void melf_elfSetSectionHeaderOffset(MELF *melf, Elf32_Off offset)
{
	melf->header.e_shoff = offset;
}

Elf32_Off melf_elfGetSectionHeaderOffset(MELF *melf)
{
	return melf->header.e_shoff;
}

void melf_elfSetSectionHeaderEntrySize(MELF *melf, Elf32_Half size)
{
	melf->header.e_shentsize = size;
}

Elf32_Half melf_elfGetSectionEntrySize(MELF *melf)
{
	return melf->header.e_shentsize;
}

void melf_elfSetProgramHeaderEntrySize(MELF *melf, Elf32_Half size)
{
	melf->header.e_phentsize = size;
}

Elf32_Half melf_elfGetProgramEntrySize(MELF *melf)
{
	return melf->header.e_phentsize;
}

void melf_elfSetProgramHeaderCount(MELF *melf, Elf32_Half count)
{
	melf->header.e_phnum = count;
}

Elf32_Half melf_elfGetProgramHeaderCount(MELF *melf)
{
	return melf->header.e_phnum;
}

void melf_elfSetSectionHeaderCount(MELF *melf, Elf32_Half count)
{
	melf->header.e_shnum = count;
}

Elf32_Half melf_elfGetSectionHeaderCount(MELF *melf)
{
	return melf->header.e_shnum;
}

void melf_elfSetStringTableIndex(MELF *melf, Elf32_Half index)
{
	melf->header.e_shstrndx = index;
}

Elf32_Half melf_elfGetStringTableIndex(MELF *melf)
{
	return (melf) ? melf->header.e_shstrndx : -1;
}
