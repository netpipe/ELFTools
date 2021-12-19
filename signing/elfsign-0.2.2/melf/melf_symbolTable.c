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

ELF_SPEC_HEADER *melf_symbolTableCreate(MELF *melf, const char *name)
{
	ELF_SPEC_HEADER *sect = melf_sectionAdd(melf);
	ELF_SPEC_HEADER *str;
	Elf32_Sym *first = NULL;

	if (!sect)
		return NULL;

	melf_sectionSetType(melf, sect, SHT_SYMTAB);
	melf_sectionSetFlags(melf, sect, SHF_ALLOC);
	melf_sectionSetEntrySize(melf, sect, sizeof(Elf32_Sym));

	if (name)
		melf_sectionSetName(melf, sect, name);

	if ((str = melf_stringTableCreate(melf, ".strtab")))
		melf_sectionSetStringTableHeader(melf, sect, str);

	// Create the first index of the symbol table.
	// name  => 0
	// value => 0
	// size  => 0
	// info  => 0
	// other => 0
	// shndx => 0
	first = melf_symbolTableAddSymbol(melf, sect, NULL);

	return sect;
}

void melf_symbolTableInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long elements = section->contentLength / sizeof(Elf32_Sym);
	unsigned long index    = 0;
	Elf32_Sym *table       = (Elf32_Sym *)section->content;

	for (index = 0; index < elements; index++)
		_melf_sectionAppendEntryList(section, table + index, sizeof(Elf32_Sym));
}

void melf_symbolTableSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long index = 0, newContentLength = section->contentListSize * sizeof(Elf32_Sym);
	Elf32_Sym *table, *curr;

	// If the list size is zero or malloc fails, return.
	if ((!section->contentListSize) ||
	    (!(table = (Elf32_Sym *)malloc(newContentLength))))
		return;

	for (index = 0;
			index < section->contentListSize;
			index++)
	{
		if (!(curr = melf_symbolTableEnum(melf, section, index)))
			continue;

		memcpy(table + index, curr, sizeof(Elf32_Sym));
	}

	section->content       = table;
	section->contentLength = newContentLength;
}

Elf32_Sym *melf_symbolTableAddSymbol(MELF *melf, ELF_SPEC_HEADER *symTable, const char *name)
{
	Elf32_Sym *res = NULL, tmp;

	// If they don't have write access...
	if (!MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		return NULL;

	memset(&tmp, 0, sizeof(tmp));

	// If the add succeeds, try to set the name for the symbol
	if ((res = _melf_sectionAppendEntryList(symTable, &tmp, sizeof(Elf32_Sym))))
		melf_symbolSetName(melf, symTable, res, name);

	return res;
}

Elf32_Sym *melf_symbolTableEnum(MELF *melf, ELF_SPEC_HEADER *symTable, unsigned long index)
{
	Elf32_Sym *res = NULL;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		res = (Elf32_Sym *)_melf_sectionEnumEntryList(symTable, index);
	else
	{
		unsigned long elements = symTable->contentLength / sizeof(Elf32_Sym);
		Elf32_Sym *table       = (Elf32_Sym *)symTable->content;

		if (index < elements)
			res = table + index;
	}

	return res;
}

unsigned long melf_symbolTableRemoveSymbol(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *symbol)
{
	unsigned long index = 0, match = 0;
	Elf32_Sym *curr;

	if (!symbol)
		return 0;

	// If they don't have write access...
	if (!MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		return 0;

	while ((curr = melf_symbolTableEnum(melf, symTable, index++)))
	{
		if (!memcmp(curr, symbol, sizeof(Elf32_Sym)))
		{
			_melf_sectionRemoveEntryList(symTable, curr);

			match++;
		}
	}

	return match;
}

unsigned long melf_symbolTableRecalculateOffsets(MELF *melf, ELF_SPEC_HEADER *relatedSection, unsigned long offset, unsigned long contentLength)
{
	ELF_SPEC_HEADER *current;
	unsigned long adjusted = 0;
	unsigned long index = 0;
	Elf32_Sym *sym;

	for (current = melf_sectionGetEnum(melf);
			current;
			current = melf_sectionEnumNext(melf, current))
	{
		if (current->spec.section.sh_type != SHT_SYMTAB)
			continue;

		index = 0;

		while ((sym = melf_symbolTableEnum(melf, current, index++)))
		{
			if ((sym->st_shndx != relatedSection->index) ||
				 (sym->st_value <= offset))
				continue;

			sym->st_value += contentLength;
			adjusted++;
		}
	}

	return adjusted;
}
