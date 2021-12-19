#include <stdlib.h>
#include <stdio.h>

#include "melf.h"

int main(int argc, char **argv)
{
	ELF_SPEC_HEADER *curr, *sym;
	MELF *melf = melf_new();

	if (!melf)
		return 0;

	fprintf(stdout, "creating new elf img called 'blank'...\n");

	melf_elfSetType(melf, ET_EXEC);
	melf_elfSetMachine(melf, EM_386);

	if ((curr = melf_sectionAdd(melf)))
	{
		melf_sectionSetName(melf, curr, "test-section");
		melf_sectionSetType(melf, curr, SHT_NOBITS);
	}
	
	if ((curr = melf_sectionAdd(melf)))
	{
		melf_sectionSetName(melf, curr, "bobby");
		melf_sectionSetType(melf, curr, SHT_NOBITS);
	}

	// Add a dynamic section
	
	if ((curr = melf_dynamicCreate(melf)))
	{
		ELF_SPEC_HEADER *dynstr = melf_sectionGetStringTableHeader(melf, curr);
		unsigned long index = 0;

		// Add DT_NEEDED for libc.
		if (dynstr)
			index = melf_stringTableSetString(melf, dynstr, "/lib/libc.so.6");

		melf_dynamicAddTag(melf, curr, DT_NEEDED, index);
	}

	// Add a note section

	if ((curr = melf_noteCreate(melf, ".note", 1)))
	{
		melf_noteAdd(melf, curr, 1, "hi", "testing", 8);
		melf_noteAdd(melf, curr, 0, "NAME", "DESC", 5);
		melf_noteAdd(melf, curr, 1, "JANE", "DESCZ", 5);
	}

	// Add a symbol table
	
	if ((curr = sym = melf_symbolTableCreate(melf, ".symtab")))
	{
		Elf32_Sym *sym = melf_symbolTableAddSymbol(melf, curr, "tester");
		sym = melf_symbolTableAddSymbol(melf, curr, "shutup");

		melf_symbolSetType(melf, curr, sym, STT_OBJECT);
		melf_symbolSetBinding(melf, curr, sym, STB_GLOBAL);
	}

	// Reloc table
	if ((curr = melf_relocTableCreate(melf, ".rel", 0)))
	{
		Elf32_Rel *rel = melf_relocTableAddRel(melf, curr);
		melf_relocRelInitialize(melf, curr, rel, 0x41, 1, 1);
		
		rel = melf_relocTableAddRel(melf, curr);
		melf_relocRelInitialize(melf, curr, rel, 0x4411, 2, 2);
		
		rel = melf_relocTableAddRel(melf, curr);
		melf_relocRelInitialize(melf, curr, rel, 0x4411, 8, 0);

		melf_relocTableRemoveRel(melf, curr, melf_relocTableEnumRel(melf, curr, 1));

		melf_relocTableSetSymbolTableHeader(melf, curr, sym);
	}

	// Reloc addend table
	if ((curr = melf_relocTableCreate(melf, ".rela", 1)))
	{
		Elf32_Rela *rela = melf_relocTableAddRela(melf, curr);

		melf_relocRelaInitialize(melf, curr, rela, 0x41, 1, 1, 0xffff);

		rela = melf_relocTableAddRela(melf, curr);
		melf_relocRelaInitialize(melf, curr, rela, 0x4441, 1, 2, 0xfffe);
		
		rela = melf_relocTableAddRela(melf, curr);
		melf_relocRelaInitialize(melf, curr, rela, 0x4444, 7, 3, 0xfffe);

		melf_relocTableRemoveRela(melf, curr, melf_relocTableEnumRela(melf, curr, 1));

		melf_relocTableSetSymbolTableHeader(melf, curr, sym);
	}

	melf_save(melf, "blank");

	melf_destroy(melf);

	return 1;
}
