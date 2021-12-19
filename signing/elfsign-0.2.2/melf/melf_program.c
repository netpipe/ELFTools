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

ELF_SPEC_HEADER *melf_programAdd(MELF *melf)
{
	ELF_SPEC_HEADER *program = _melf_listAppend(&melf->programs, NULL, 0, 
			NULL, 0);
	Elf32_Half numPrograms = melf_elfGetProgramHeaderCount(melf);

	if (program)
		melf_elfSetProgramHeaderCount(melf, numPrograms + 1);

	return program;
}

void _melf_programDestroy(ELF_SPEC_HEADER *program)
{
	if (program->content)
		free(program->content);

	free(program);
}

ELF_SPEC_HEADER *melf_programGetEnum(MELF *melf)
{
	return melf->programs.head;
}

ELF_SPEC_HEADER *melf_programEnumNext(MELF *melf, ELF_SPEC_HEADER *en)
{
	return (en) ? en->next : NULL;
}

ELF_SPEC_HEADER *melf_programEnumPrev(MELF *melf, ELF_SPEC_HEADER *en)
{
	return (en) ? en->prev : NULL;
}

void melf_programSetType(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Word type)
{
	if (program)
		program->spec.program.p_type = type;
}

Elf32_Word melf_programGetType(MELF *melf, ELF_SPEC_HEADER *program)
{
	return (program) ? program->spec.program.p_type : 0;
}

void melf_programSetVirtualAddress(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Addr addr)
{
	if (program)
		program->spec.program.p_vaddr = addr;
}

Elf32_Addr melf_programGetVirtualAddress(MELF *melf, ELF_SPEC_HEADER *program)
{
	return (program) ? program->spec.program.p_vaddr : 0;
}

void melf_programSetPhysicalAddress(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Addr addr)
{
	if (program)
		program->spec.program.p_paddr = addr;
}

Elf32_Addr melf_programGetPhysicalAddress(MELF *melf, ELF_SPEC_HEADER *program)
{
	return (program) ? program->spec.program.p_paddr : 0;
}

void melf_programSetVirtualSize(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Word size)
{
	if (program)
		program->spec.program.p_memsz = size;
}

Elf32_Word melf_programGetVirtualSize(MELF *melf, ELF_SPEC_HEADER *program)
{
	return (program) ? program->spec.program.p_memsz : 0;
}

void melf_programSetPhysicalSize(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Word size)
{
	if (program)
		program->spec.program.p_filesz = size;
}

Elf32_Word melf_programGetPhysicalSize(MELF *melf, ELF_SPEC_HEADER *program)
{
	return (program) ? program->spec.program.p_filesz : 0;
}

void melf_programSetFlags(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Word flags)
{
	if (program)
		program->spec.program.p_flags = flags;
}

Elf32_Word melf_programGetFlags(MELF *melf, ELF_SPEC_HEADER *program)
{
	return (program) ? program->spec.program.p_flags : 0;
}

unsigned long melf_programRemove(MELF *melf, unsigned long id)
{
	Elf32_Half numPrograms = melf_elfGetProgramHeaderCount(melf);
	unsigned long removed = _melf_listRemove(&melf->programs, id);

	if (removed)
		melf_elfSetProgramHeaderCount(melf, numPrograms - removed);

	return removed;
}
