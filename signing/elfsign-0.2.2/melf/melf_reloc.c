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

ELF_SPEC_HEADER *melf_relocTableCreate(MELF *melf, const char *name, unsigned char hasAddends)
{
	ELF_SPEC_HEADER *sect = melf_sectionAdd(melf);

	if (!sect)
		return NULL;

	melf_sectionSetType(melf, sect, (hasAddends) ? SHT_RELA : SHT_REL);
	melf_sectionSetFlags(melf, sect, SHF_ALLOC);
	melf_sectionSetEntrySize(melf, sect, (hasAddends) ? sizeof(Elf32_Rela) : sizeof(Elf32_Rel));

	if (name)
		melf_sectionSetName(melf, sect, name);

	return sect;
}

void melf_relocInitializeRelEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long elements = section->contentLength / sizeof(Elf32_Rel);
	unsigned long index    = 0;
	Elf32_Rel *table       = (Elf32_Rel *)section->content;

	for (index = 0; index < elements; index++)
		_melf_sectionAppendEntryList(section, table + index, sizeof(Elf32_Rel));
}

void melf_relocSynchronizeRelEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long index = 0, newContentLength = section->contentListSize * sizeof(Elf32_Rel);
	Elf32_Rel *table, *curr;

	// If the list size is zero or malloc fails, return.
	if ((!section->contentListSize) ||
	    (!(table = (Elf32_Rel *)malloc(newContentLength))))
		return;

	for (index = 0;
			index < section->contentListSize;
			index++)
	{
		if (!(curr = melf_relocTableEnumRel(melf, section, index)))
			continue;

		memcpy(table + index, curr, sizeof(Elf32_Rel));
	}

	section->content       = table;
	section->contentLength = newContentLength;
}

void melf_relocInitializeRelaEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long elements = section->contentLength / sizeof(Elf32_Rela);
	unsigned long index    = 0;
	Elf32_Rela *table      = (Elf32_Rela *)section->content;

	for (index = 0; index < elements; index++)
		_melf_sectionAppendEntryList(section, table + index, sizeof(Elf32_Rela));
}

void melf_relocSynchronizeRelaEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long index = 0, newContentLength = section->contentListSize * sizeof(Elf32_Rela);
	Elf32_Rela *table, *curr;

	// If the list size is zero or malloc fails, return.
	if ((!section->contentListSize) ||
	    (!(table = (Elf32_Rela *)malloc(newContentLength))))
		return;

	for (index = 0;
			index < section->contentListSize;
			index++)
	{
		if (!(curr = melf_relocTableEnumRela(melf, section, index)))
			continue;

		memcpy(table + index, curr, sizeof(Elf32_Rela));
	}

	section->content       = table;
	section->contentLength = newContentLength;
}

void melf_relocTableSetSymbolTableHeader(MELF *melf, ELF_SPEC_HEADER *relocTable, ELF_SPEC_HEADER *symTable)
{
	melf_sectionSetLink(melf, relocTable, melf_sectionGetIndex(melf, symTable));
}

Elf32_Rel *melf_relocTableAddRel(MELF *melf, ELF_SPEC_HEADER *relocTable)
{
	Elf32_Rel tmp, *res = NULL;

	memset(&tmp, 0, sizeof(tmp));

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		res = (Elf32_Rel *)_melf_sectionAppendEntryList(relocTable, &tmp, sizeof(Elf32_Rel));
	else
		res = NULL; // If they don't have write access, fail the call.

	return res;
}

Elf32_Rel *melf_relocTableEnumRel(MELF *melf, ELF_SPEC_HEADER *relocTable, unsigned long index)
{
	Elf32_Rel *res = NULL;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		res = (Elf32_Rel *)_melf_sectionEnumEntryList(relocTable, index);
	else
	{
		unsigned long elements = relocTable->contentLength / sizeof(Elf32_Rel);
		Elf32_Rel *table       = (Elf32_Rel *)relocTable->content;
	
		if (index < elements)
			res = table + index;
	}

	return res;
}

unsigned long melf_relocTableRemoveRel(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel)
{
	unsigned long index = 0, match = 0;
	Elf32_Rel *curr;

	if (!rel)
		return 0;

	// If they don't have write access, fail the call.
	if (!MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		return 0;

	// Enumerate the table and find a match, remove it, then keep searching.
	while ((curr = melf_relocTableEnumRel(melf, relocTable, index++)))
	{
		if ((curr->r_offset == rel->r_offset) &&
		    (curr->r_info == rel->r_info))
		{
			_melf_sectionRemoveEntryList(relocTable, curr);

			match++;
		}
	}

	return match;
}

Elf32_Rela *melf_relocTableAddRela(MELF *melf, ELF_SPEC_HEADER *relocTable)
{
	Elf32_Rela tmp, *res = NULL;

	memset(&tmp, 0, sizeof(tmp));

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		res = (Elf32_Rela *)_melf_sectionAppendEntryList(relocTable, &tmp, sizeof(Elf32_Rela));
	else
		res = NULL;

	return res;
}

Elf32_Rela *melf_relocTableEnumRela(MELF *melf, ELF_SPEC_HEADER *relocTable, unsigned long index)
{
	Elf32_Rela *res = NULL;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		res = (Elf32_Rela *)_melf_sectionEnumEntryList(relocTable, index);
	else
	{
		unsigned long elements = relocTable->contentLength / sizeof(Elf32_Rela);
		Elf32_Rela *table      = (Elf32_Rela *)relocTable->content;
	
		if (index < elements)
			res = table + index;
	}
	
	return res;
}

unsigned long melf_relocTableRemoveRela(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela)
{
	unsigned long index = 0, match = 0;
	Elf32_Rela *curr;

	if (!rela)
		return 0;

	// If they don't have write access, fail the call.
	if (!MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		return 0;

	// Enumerate the table and find a match, remove it, then keep searching.
	while ((curr = melf_relocTableEnumRela(melf, relocTable, index++)))
	{
		if ((curr->r_offset == rela->r_offset) &&
		    (curr->r_info == rela->r_info) &&
			 (curr->r_addend == rela->r_addend))
		{
			_melf_sectionRemoveEntryList(relocTable, curr);

			match++;
		}
	}

	return match;
}

unsigned long melf_relocRecalculateOffsets(MELF *melf, ELF_SPEC_HEADER *relatedSection, unsigned long offset, unsigned long contentLength)
{
	ELF_SPEC_HEADER *current;
	unsigned long adjusted = 0;
	unsigned long index = 0;

	for (current = melf_sectionGetEnum(melf);
			current;
			current = melf_sectionEnumNext(melf, current))
	{
		// Is this a section that this (potential) reloc table references?
		if (relatedSection->index != current->spec.section.sh_link)
			continue;

		switch (current->spec.section.sh_type)
		{
			case SHT_REL:
				{
					Elf32_Rel *rel;

					index = 0;

					while ((rel = melf_relocTableEnumRel(melf, current, index++)))
					{
						if (rel->r_offset <= offset)
							continue;

						rel->r_offset += contentLength;
						adjusted++;
					}
				}
				break;
			case SHT_RELA:
				{
					Elf32_Rela *rela;

					index = 0;

					while ((rela = melf_relocTableEnumRela(melf, current, index++)))
					{
						if (rela->r_offset <= offset)
							continue;

						rela->r_offset += contentLength;
						adjusted++;
					}
				}
				break;
			default:
				break;
		}
	}

	return adjusted;
}

void melf_relocRelInitialize(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel, Elf32_Addr offset, unsigned char symbolIndex, unsigned char type)
{
	if (rel)
	{
		rel->r_offset = offset;
		rel->r_info   = (symbolIndex << 8) + type;
	}
}

void melf_relocRelSetOffset(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel, Elf32_Addr offset)
{
	if (rel)
		rel->r_offset = offset;
}

Elf32_Addr melf_relocRelGetOffset(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel)
{
	return (rel) ? rel->r_offset : 0;
}

void melf_relocRelSetSymbol(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel, unsigned char symbolIndex)
{
	if (rel)
		rel->r_info = (symbolIndex << 8) + (rel->r_info & 0xff);	
}

unsigned char melf_relocRelGetSymbol(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel)
{
	return (rel) ? (rel->r_info >> 8) : 0;
}

void melf_relocRelSetType(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel, unsigned char type)
{
	if (rel)
		rel->r_info = (rel->r_info & 0xff00) + type;
}

unsigned char melf_relocRelGetType(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel)
{
	return (rel) ? (rel->r_info & 0xff) : 0;
}

void melf_relocRelaInitialize(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, Elf32_Addr offset, unsigned char symbolIndex, unsigned char type, Elf32_Sword addend)
{
	if (rela)
	{
		rela->r_offset = offset;
		rela->r_info   = (symbolIndex << 8) + type;
		rela->r_addend = addend;
	}
}

void melf_relocRelaSetOffset(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, Elf32_Addr offset)
{
	if (rela)
		rela->r_offset = offset;
}

Elf32_Addr melf_relocRelaGetOffset(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela)
{
	return (rela) ? rela->r_offset : 0;
}

void melf_relocRelaSetSymbol(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, unsigned char symbolIndex)
{
	if (rela)
		rela->r_info = (symbolIndex << 8) + (rela->r_info & 0xff);	
}

unsigned char melf_relocRelaGetSymbol(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela)
{
	return (rela) ? (rela->r_info >> 8) : 0;
}

void melf_relocRelaSetType(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, unsigned char type)
{
	if (rela)
		rela->r_info = (rela->r_info & 0xff00) + type;
}

unsigned char melf_relocRelaGetType(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela)
{
	return (rela) ? (rela->r_info & 0xff) : 0;
}

void melf_relocRelaSetAddend(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, Elf32_Sword addend)
{
	if (rela)
		rela->r_addend = addend;
}

Elf32_Sword melf_relocRelaGetAddend(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela)
{
	return (rela) ? rela->r_addend : 0;
}
