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

ELF_SPEC_HEADER *melf_sectionAdd(MELF *melf)
{
	ELF_SPEC_HEADER *section = _melf_listAppend(&melf->sections, NULL, 0, 
			NULL, 0);
	Elf32_Half numSections = melf_elfGetSectionHeaderCount(melf);

	if (section)
		melf_elfSetSectionHeaderCount(melf, numSections + 1);

	return section;
}

void _melf_sectionDestroy(ELF_SPEC_HEADER *sect)
{
	if (sect->content)
		free(sect->content);

	free(sect);
}

ELF_SPEC_HEADER *melf_sectionGetEnum(MELF *melf)
{
	return melf->sections.head;
}

ELF_SPEC_HEADER *melf_sectionEnumNext(MELF *melf, ELF_SPEC_HEADER *en)
{
	return (en) ? en->next : NULL;
}

ELF_SPEC_HEADER *melf_sectionEnumPrev(MELF *melf, ELF_SPEC_HEADER *en)
{
	return (en) ? en->prev : NULL;
}

ELF_SPEC_HEADER *melf_sectionFindType(MELF *melf, unsigned long type)
{
	ELF_SPEC_HEADER *curr = NULL;

	for (curr = melf_sectionGetEnum(melf);
			curr;
			curr = melf_sectionEnumNext(melf, curr))
	{
		if (curr->spec.section.sh_type == type)
			break;
	}

	return curr;
}

ELF_SPEC_HEADER *melf_sectionFindIndex(MELF *melf, unsigned long index)
{
	ELF_SPEC_HEADER *curr = NULL;

	for (curr = melf_sectionGetEnum(melf);
			curr;
			curr = melf_sectionEnumNext(melf, curr))
	{
		if (curr->index == index)
			break;
	}

	return curr;
}

ELF_SPEC_HEADER *melf_sectionFindName(MELF *melf, const char *name)
{
	ELF_SPEC_HEADER *curr = NULL;

	for (curr = melf_sectionGetEnum(melf);
			curr;
			curr = melf_sectionEnumNext(melf, curr))
	{
		const char *sec = melf_sectionGetName(melf, curr);		

		if ((sec) && (!strcmp(name, sec)))
			break;
	}

	return curr;
}

void melf_sectionSetStringTableHeader(MELF *melf, ELF_SPEC_HEADER *section, ELF_SPEC_HEADER *stringTable)
{
	section->spec.section.sh_link = melf_sectionGetIndex(melf, stringTable);
}

ELF_SPEC_HEADER *melf_sectionGetStringTableHeader(MELF *melf, ELF_SPEC_HEADER *section)
{
	ELF_SPEC_HEADER *ret = NULL;

	if (section->spec.section.sh_link > melf->sections.length)
		return NULL;

	ret = melf_sectionFindIndex(melf, section->spec.section.sh_link);

	if (ret->spec.section.sh_type != SHT_STRTAB)
		return NULL;

	return ret;
}

void melf_sectionSetContent(MELF *melf, ELF_SPEC_HEADER *section, void *content, unsigned long contentLength)
{
	if (section->content)
		free(section->content);

	section->contentLength = contentLength;

	if (content && contentLength)
	{
		if ((section->content = (void *)malloc(contentLength)))
			memcpy(section->content, content, contentLength);
	}
	else
		section->content = NULL;

	section->spec.section.sh_size = section->contentLength;

	/*
	 * If the melf context has write access, initialize the content list for this section
	 */
	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		_melf_sectionInitializeEntryList(melf, section);
}

unsigned long melf_sectionInsertContent(MELF *melf, ELF_SPEC_HEADER *section, unsigned long offset, void *content, unsigned long contentLength)
{
	void *newContent;

	if (offset > section->contentLength) 
		offset = section->contentLength;

	if (!(newContent = malloc(section->contentLength + contentLength)))
		return 0;

	// Initialize our new content buffer with the inserted data.
	memcpy(newContent, section->content, offset);
	memcpy((char *)newContent + offset, content, contentLength);
	memcpy((char *)newContent + offset + contentLength, (char *)section->content + offset, section->contentLength - offset);

	// Recalculate offsets for sections that might care.
	melf_relocRecalculateOffsets(melf, section, offset, contentLength);
	melf_symbolTableRecalculateOffsets(melf, section, offset, contentLength);

	/*
	 * If the melf context has write access, initialize the content list for this section
	 */
	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		_melf_sectionInitializeEntryList(melf, section);

	return 1;
}

void *melf_sectionGetContent(MELF *melf, ELF_SPEC_HEADER *section)
{
	// If the person has write access, synchronize to buffer
	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		_melf_sectionSynchronizeEntryList(melf, section);

	return (section) ? section->content : NULL;
}

void melf_sectionSetName(MELF *melf, ELF_SPEC_HEADER *section, const char *name)
{
	ELF_SPEC_HEADER *stringSection = melf_sectionFindIndex(melf, melf->header.e_shstrndx);
	unsigned long index = 0;

	// If no string table exists or one cannot be found...
	if (!stringSection || melf_sectionGetType(melf, stringSection) != SHT_STRTAB)
	{
		stringSection = melf_stringTableCreate(melf, NULL);

		if (stringSection)
		{
			melf_elfSetStringTableIndex(melf, melf_sectionGetIndex(melf, stringSection));

			melf_sectionSetName(melf, stringSection, ".shstrtab");
		}
	}

	// Now add the string to the string table.
	if ((index = melf_stringTableSetString(melf, stringSection, name)) != -1)
		section->spec.section.sh_name = index;
}

const char *melf_sectionGetName(MELF *melf, ELF_SPEC_HEADER *section)
{
	ELF_SPEC_HEADER *stringSection = melf_sectionFindIndex(melf, melf->header.e_shstrndx);

	return (stringSection) ? melf_stringTableGetString(melf, stringSection, section->spec.section.sh_name) : "";
}

void melf_sectionSetType(MELF *melf, ELF_SPEC_HEADER *section, unsigned long type)
{
	if (section)
		section->spec.section.sh_type = type;
}

unsigned long melf_sectionGetType(MELF *melf, ELF_SPEC_HEADER *section)
{
	return (section) ? section->spec.section.sh_type : SHT_NULL;
}

void melf_sectionSetOffset(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Off offset)
{
	if (section)
		section->spec.section.sh_offset = offset;
}

Elf32_Off melf_sectionGetOffset(MELF *melf, ELF_SPEC_HEADER *section)
{
	return (section) ? section->spec.section.sh_offset : 0;
}

void melf_sectionSetSize(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Half size)
{
	if (section)
		section->spec.section.sh_size = size;
}

Elf32_Half melf_sectionGetSize(MELF *melf, ELF_SPEC_HEADER *section)
{
	return (section) ? section->spec.section.sh_size : 0;
}

void melf_sectionSetAddress(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Addr addr)
{
	if (section)
		section->spec.section.sh_addr = addr;
}

Elf32_Addr melf_sectionGetAddress(MELF *melf, ELF_SPEC_HEADER *section)
{
	return (section) ? section->spec.section.sh_addr : 0;
}

void melf_sectionSetLink(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Word link)
{
	if (section)
		section->spec.section.sh_link = link;
}

Elf32_Word melf_sectionGetLink(MELF *melf, ELF_SPEC_HEADER *section)
{
	return (section) ? section->spec.section.sh_link : 0;
}

void melf_sectionSetInfo(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Word info)
{
	if (section)
		section->spec.section.sh_info = info;
}

Elf32_Word melf_sectionGetInfo(MELF *melf, ELF_SPEC_HEADER *section)
{
	return (section) ? section->spec.section.sh_info : 0;
}

void melf_sectionSetFlags(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Word flags)
{
	if (section)
		section->spec.section.sh_flags = flags;
}

Elf32_Word melf_sectionGetFlags(MELF *melf, ELF_SPEC_HEADER *section)
{
	return (section) ? section->spec.section.sh_flags : 0;
}

void melf_sectionSetEntrySize(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Word entsize)
{
	if (section)
		section->spec.section.sh_entsize = entsize;
}

Elf32_Word melf_sectionGetEntrySize(MELF *melf, ELF_SPEC_HEADER *section)
{
	return (section) ? section->spec.section.sh_entsize : 0;
}

unsigned long melf_sectionGetIndex(MELF *melf, ELF_SPEC_HEADER *section)
{
	return (section) ? section->index : -1;	
}

unsigned long melf_sectionRemove(MELF *melf, unsigned long id)
{
	Elf32_Half numSections = melf_elfGetSectionHeaderCount(melf);
	unsigned long removed = _melf_listRemove(&melf->sections, id);

	if (removed)
		melf_elfSetSectionHeaderCount(melf, numSections - removed);

	return removed;
}

void *_melf_sectionAllocateItem(ELF_SPEC_HEADER *sect, unsigned long itemSize)
{
	unsigned long newLength = sect->contentLength + itemSize;
	unsigned char *base     = NULL;

	if (!sect->content)
		base = (unsigned char *)malloc(newLength);
	else
		base = (unsigned char *)realloc(sect->content, newLength);

	if (!base)
		return NULL;

	sect->content        = (void *)base;
 	base                += sect->contentLength;
	sect->contentLength  = newLength;

	memset(base, 0, itemSize);

	return base;
}

void _melf_sectionShrink(ELF_SPEC_HEADER *sect, void *at, unsigned long itemSize)
{
	unsigned long postElements = 0;
	unsigned long preElements  = 0;
	unsigned long newLength    = sect->contentLength - itemSize, elements = 0;
	unsigned char *shrinkAt    = (unsigned char *)at;
	unsigned char *base        = (unsigned char *)sect->content;

	if (!base || !sect->contentLength)
		return;

	elements = sect->contentLength / itemSize;

	if (newLength)
		sect->content = (void *)malloc(newLength);
	else // Else we got rid of all the symbols
	{
		free(base);

		sect->content       = NULL;
		sect->contentLength = 0;

		return;
	}
	
	// From here on out the number of elements will always be greater than one.

	preElements  = (shrinkAt - base) / itemSize;
	postElements = elements - preElements - 1;

	if (preElements)
		memcpy(sect->content, base, preElements * itemSize);
	if (postElements)
		memcpy(sect->content + (preElements * itemSize), shrinkAt + itemSize, postElements * itemSize);

	free(base);

	sect->contentLength = newLength;
}

ELF_SECTION_ENTRY *_melf_sectionGetEntryListPointer(void *ptr)
{
	return (ptr) ? (ELF_SECTION_ENTRY *)((unsigned char *)ptr - sizeof(ELF_SECTION_ENTRY)) : NULL;
}

void *_melf_sectionGetEntryValuePointer(void *ptr)
{
	return (ptr) ? (void *)((unsigned char *)ptr + sizeof(ELF_SECTION_ENTRY)) : NULL;
}

void *_melf_sectionAppendEntryList(ELF_SPEC_HEADER *sect, void *item, unsigned long itemSize)
{
	ELF_SECTION_ENTRY *e = (ELF_SECTION_ENTRY *)malloc(itemSize + sizeof(ELF_SECTION_ENTRY));
	void *value          = _melf_sectionGetEntryValuePointer(e);

	if (!e)
		return NULL;

	if (item)
		memcpy(value, item, itemSize);
	else
		memset(value, 0, itemSize);

	e->next  = NULL;
	e->prev  = NULL;

	if (sect->contentListTail)
	{
		ELF_SECTION_ENTRY *tail = _melf_sectionGetEntryListPointer(sect->contentListTail);

		tail->next = value;
		e->prev    = _melf_sectionGetEntryValuePointer(tail);
	}
	else
		sect->contentListHead = value;

	sect->contentListTail = value;
	sect->contentListSize++;

	return value;
}

void _melf_sectionRemoveEntryList(ELF_SPEC_HEADER *sect, void *item)
{
	ELF_SECTION_ENTRY *e = _melf_sectionGetEntryListPointer(item), *next, *prev;

	if (!e)
		return;

	next = _melf_sectionGetEntryListPointer(e->next);
	prev = _melf_sectionGetEntryListPointer(e->prev);

	if (prev)
		prev->next = _melf_sectionGetEntryValuePointer(next);
	if (next)
		next->prev = _melf_sectionGetEntryValuePointer(prev);
	if (item == sect->contentListHead)
		sect->contentListHead = _melf_sectionGetEntryValuePointer(next);
	if (item == sect->contentListTail)
		sect->contentListTail = _melf_sectionGetEntryValuePointer(prev);

	sect->contentListSize--;

	free(e);
}

void *_melf_sectionEnumEntryList(ELF_SPEC_HEADER *sect, unsigned long index)
{
	unsigned long curr = 0;
	void *res          = sect->contentListHead;

	while (curr < index && res)
	{
		ELF_SECTION_ENTRY *e = _melf_sectionGetEntryListPointer(res);

		if (e)
			res = e->next;

		curr++;
	}
		
	return res;
}

void _melf_sectionFlushEntryList(ELF_SPEC_HEADER *sect)
{
	ELF_SECTION_ENTRY *e = _melf_sectionGetEntryListPointer(sect->contentListHead), *next;

	while (e)
	{
		next = e->next;

		free(e);

		e = _melf_sectionGetEntryListPointer(next);
	}

	sect->contentListSize = 0;
}

void _melf_sectionInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *sect)
{
	if (sect->contentListHead)
		_melf_sectionFlushEntryList(sect->contentListHead);

	sect->contentListHead = NULL;
	sect->contentListTail = NULL;

	switch (sect->spec.section.sh_type)
	{
		case SHT_REL:
			melf_relocInitializeRelEntryList(melf, sect);
			break;
		case SHT_RELA:
			melf_relocInitializeRelaEntryList(melf, sect);
			break;
		case SHT_SYMTAB:
		case SHT_DYNSYM:
			melf_symbolTableInitializeEntryList(melf, sect);
			break;
		case SHT_DYNAMIC:
			melf_dynamicInitializeEntryList(melf, sect);
			break;
		case SHT_NOTE:
			melf_noteInitializeEntryList(melf, sect);
			break;
		case SHT_RES:
			melf_resInitializeEntryList(melf, sect);
			break;
		case SHT_STRTAB:
			melf_stringTableInitializeEntryList(melf, sect);
			break;
		default:
			break;
	}
}

void _melf_sectionSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *sect)
{
	switch (sect->spec.section.sh_type)
	{
		case SHT_REL:
		case SHT_RELA:
		case SHT_SYMTAB:
		case SHT_DYNSYM:
		case SHT_DYNAMIC:
		case SHT_NOTE:
		case SHT_RES:
		case SHT_STRTAB:
			if (sect->content)
				free(sect->content);

			sect->content       = NULL;
			sect->contentLength = 0;
			break;
		default:
			break;
	}

	switch (sect->spec.section.sh_type)
	{
		case SHT_REL:
			melf_relocSynchronizeRelEntryList(melf, sect);
			break;
		case SHT_RELA:
			melf_relocSynchronizeRelaEntryList(melf, sect);
			break;
		case SHT_SYMTAB:
		case SHT_DYNSYM:
			melf_symbolTableSynchronizeEntryList(melf, sect);
			break;
		case SHT_DYNAMIC:
			melf_dynamicSynchronizeEntryList(melf, sect);
			break;
		case SHT_NOTE:
			melf_noteSynchronizeEntryList(melf, sect);
			break;
		case SHT_RES:
			melf_resSynchronizeEntryList(melf, sect);
			break;
		case SHT_STRTAB:
			melf_stringTableSynchronizeEntryList(melf, sect);
		default:
			break;
	}
}
