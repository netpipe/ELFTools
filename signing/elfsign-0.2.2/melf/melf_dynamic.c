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

Elf32_Dyn *_melf_dynamicAllocateDyn(ELF_SPEC_HEADER *dynamic);
void _melf_dynamicShrink(ELF_SPEC_HEADER *dynamic, Elf32_Dyn *shrinkAt);

ELF_SPEC_HEADER *melf_dynamicCreate(MELF *melf)
{
	ELF_SPEC_HEADER *dyn = melf_sectionAdd(melf), *dynstr;
	ELF_SPEC_HEADER *prog = melf_programAdd(melf);

	if (!dyn)
		return NULL;

	melf_sectionSetName(melf, dyn, ".dynamic");
	melf_sectionSetType(melf, dyn, SHT_DYNAMIC);
	melf_sectionSetAddress(melf, dyn, 0x10000000);
	melf_sectionSetFlags(melf, dyn, SHF_ALLOC);
	melf_sectionSetEntrySize(melf, dyn, sizeof(Elf32_Dyn));

	if ((dynstr = melf_stringTableCreate(melf, ".dynstr")))
		melf_sectionSetStringTableHeader(melf, dyn, dynstr);

	melf_programSetType(melf, prog, PT_DYNAMIC);
	melf_programSetVirtualAddress(melf, prog, melf_sectionGetAddress(melf, dyn));
	melf_programSetPhysicalAddress(melf, prog, melf_sectionGetAddress(melf, dyn));
	melf_programSetFlags(melf, prog, PF_R);
	melf_programSetVirtualSize(melf, prog, sizeof(Elf32_Dyn) * 2);
	melf_programSetPhysicalSize(melf, prog, sizeof(Elf32_Dyn) * 2);

	// create mandatory entries
	melf_dynamicAddTag(melf, dyn, DT_STRTAB, melf_sectionGetIndex(melf, dynstr));

	return dyn;
}

void melf_dynamicInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long elements = section->contentLength / sizeof(Elf32_Dyn);
	unsigned long index    = 0;
	Elf32_Dyn *table       = (Elf32_Dyn *)section->content;

	for (index = 0; index < elements; index++)
		_melf_sectionAppendEntryList(section, table + index, sizeof(Elf32_Dyn));
}

void melf_dynamicSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long index = 0, newContentLength = (section->contentListSize + 1) * sizeof(Elf32_Dyn);
	Elf32_Dyn *table, *curr;
	int realOffset = 0;

	// If the list already contains a DT_NULL, we shall not allocate an extra entry.
	if (melf_dynamicGetTag(melf, section, DT_NULL))
		newContentLength -= sizeof(Elf32_Dyn);

	if ((!section->contentListSize) ||
		 (!(table = (Elf32_Dyn *)malloc(newContentLength))))
		return;

	for (index = 0, realOffset = 0;
	     index < section->contentListSize;
	     index++)
	{
		if (!(curr = melf_dynamicGetIndex(melf, section, index)))
			continue;

		if (curr->d_tag == DT_NULL)
			continue;

		memcpy(table + realOffset++, curr, sizeof(Elf32_Dyn));
	}

	// Finally, set the last entry to DT_NULL
	curr             = table + realOffset;
	curr->d_tag      = DT_NULL;
	curr->d_un.d_val = 0;

	section->content       = table;
	section->contentLength = newContentLength;

}

Elf32_Dyn *melf_dynamicAddTag(MELF *melf, ELF_SPEC_HEADER *dynamic, Elf32_Sword tag, Elf32_Word val)
{
	Elf32_Dyn tmp, *dyn = NULL;

	memset(&tmp, 0, sizeof(tmp));

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		dyn = (Elf32_Dyn *)_melf_sectionAppendEntryList(dynamic, &tmp, sizeof(Elf32_Dyn));
	else
		dyn = _melf_dynamicAllocateDyn(dynamic);

	if (dyn)
	{
		dyn->d_tag       = tag;
		dyn->d_un.d_val  = val;
	}

	return dyn;
}

unsigned long melf_dynamicRemoveTag(MELF *melf, ELF_SPEC_HEADER *dynamic, Elf32_Sword tag)
{
	unsigned long index = 0, match = 0;
	Elf32_Dyn    *curr;

	while ((curr = melf_dynamicGetIndex(melf, dynamic, index)))
	{
		if (curr->d_tag == tag)
			match += melf_dynamicRemoveIndex(melf, dynamic, index), index = 0;
		else
			index++;
	}

	return match;
}

unsigned long melf_dynamicRemoveIndex(MELF *melf, ELF_SPEC_HEADER *dynamic, unsigned long index)
{
	unsigned long cindex = 0, match = 0;
	Elf32_Dyn    *curr;

	if (!MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		return 0;

	while ((curr = melf_dynamicGetIndex(melf, dynamic, cindex++)))
	{
		if (index == cindex)
		{
			_melf_sectionRemoveEntryList(dynamic, curr);

			match = 1;

			break;
		}
	}

	return match;
}

Elf32_Dyn *melf_dynamicGetTag(MELF *melf, ELF_SPEC_HEADER *dynamic, Elf32_Sword tag)
{
	unsigned long cindex = 0;
	Elf32_Dyn    *curr;

	while ((curr = melf_dynamicGetIndex(melf, dynamic, cindex++)))
	{
		if (curr->d_tag == tag)
			break;
	}

	return curr;
}

Elf32_Dyn *melf_dynamicGetIndex(MELF *melf, ELF_SPEC_HEADER *dynamic, unsigned long index)
{
	Elf32_Dyn *res = NULL;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		res = (Elf32_Dyn *)_melf_sectionEnumEntryList(dynamic, index);
	else
	{
		unsigned long elements = dynamic->contentLength / sizeof(Elf32_Dyn);
		Elf32_Dyn     *base = (Elf32_Dyn *)dynamic->content;
	
		if (index < elements)
			res = base + index;
	}
	
	return res;
}

unsigned long melf_dynamicSetTag(MELF *melf, ELF_SPEC_HEADER *dynamic, Elf32_Sword tag, Elf32_Word val)
{
	Elf32_Dyn *match = melf_dynamicGetTag(melf, dynamic, tag);

	if (match)
		match->d_un.d_val = val;
	else
		match = melf_dynamicAddTag(melf, dynamic, tag, val);

	return 1;
}

unsigned long melf_dynamicSetIndex(MELF *melf, ELF_SPEC_HEADER *dynamic, unsigned long index, Elf32_Word val)
{
	Elf32_Dyn *entry = NULL;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		entry = (Elf32_Dyn *)_melf_sectionEnumEntryList(dynamic, index);
	else
	{
		unsigned long elements = dynamic->contentLength / sizeof(Elf32_Dyn);
		Elf32_Dyn     *base = (Elf32_Dyn *)dynamic->content;
	
		if (index < elements)
			entry = base + index;
	}

	if (entry)
		entry->d_un.d_val = val;

	return (entry) ? 1 : 0;
}

Elf32_Dyn *_melf_dynamicAllocateDyn(ELF_SPEC_HEADER *dynamic)
{
	unsigned long newLength = dynamic->contentLength + sizeof(Elf32_Dyn);
	Elf32_Dyn     *base     = NULL, *initial = NULL, *ret = NULL;

	if (!(initial = (Elf32_Dyn *)dynamic->content))
		base = (Elf32_Dyn *)malloc(newLength);
	else
		base = (Elf32_Dyn *)realloc(dynamic->content, newLength);

	if (!base)
		return NULL;

	dynamic->content        = (void *)base;
 	base                    = (Elf32_Dyn *)((char *)base + dynamic->contentLength);
	dynamic->contentLength  = newLength;

	base->d_tag      = DT_NULL;
	base->d_un.d_val = 0;

	if (!initial)
	{
		_melf_dynamicAllocateDyn(dynamic);

		ret = (Elf32_Dyn *)dynamic->content;
	}
	else
		ret = (Elf32_Dyn *)(((unsigned char *)dynamic->content + dynamic->contentLength)) - 2;

	if (ret < (Elf32_Dyn *)dynamic->content)
		return NULL;

	return ret;
}

void _melf_dynamicShrink(ELF_SPEC_HEADER *dynamic, Elf32_Dyn *shrinkAt)
{
	Elf32_Dyn *base = (Elf32_Dyn *)dynamic->content;
	unsigned long newLength = dynamic->contentLength - sizeof(Elf32_Dyn), elements = 0;
	unsigned long preElements  = 0;
	unsigned long postElements = 0;

	if (!base || !dynamic->contentLength)
		return;

	elements = dynamic->contentLength / sizeof(Elf32_Dyn);

	if (newLength)
		dynamic->content = (void *)malloc(newLength);
	else // Else we got rid of all the dynamic's
	{
		free(base);

		dynamic->content       = NULL;
		dynamic->contentLength = 0;

		return;
	}
	
	// From here on out the number of elements will always be greater than one.

	preElements  = shrinkAt - base;
	postElements = elements - preElements - 1;

	if (preElements)
		memcpy(dynamic->content, base, preElements * sizeof(Elf32_Dyn));
	if (postElements)
		memcpy(dynamic->content + (preElements * sizeof(Elf32_Dyn)), shrinkAt + 1, postElements * sizeof(Elf32_Dyn));

	free(base);

	dynamic->contentLength = newLength;
}
