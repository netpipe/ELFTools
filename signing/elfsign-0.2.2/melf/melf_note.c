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

Elf32_Nhdr *_melf_noteAllocate(ELF_SPEC_HEADER *note, const char *name, const char *desc, unsigned long descLength);
void _melf_noteShrink(ELF_SPEC_HEADER *note, Elf32_Nhdr *shrinkAt);

ELF_SPEC_HEADER *melf_noteCreate(MELF *melf, const char *name, unsigned long createProgramHeader)
{
	ELF_SPEC_HEADER *note =	melf_sectionAdd(melf);

	if (note)
	{
		melf_sectionSetType(melf, note, SHT_NOTE);
		melf_sectionSetName(melf, note, (name) ? name : ".note");
		melf_sectionSetFlags(melf, note, SHF_ALLOC);
		melf_sectionSetAddress(melf, note, 0x20000000);
		melf_sectionSetEntrySize(melf, note, sizeof(Elf32_Nhdr));
	}

	if (createProgramHeader)
	{
		ELF_SPEC_HEADER *prog = melf_programAdd(melf);

		if (prog)
		{
			melf_programSetType(melf, prog, PT_NOTE);
			melf_programSetFlags(melf, prog, PF_R);
			melf_programSetVirtualAddress(melf, prog, melf_sectionGetAddress(melf, note));
			melf_programSetPhysicalAddress(melf, prog, melf_sectionGetAddress(melf, note));
		}
	}

	return note;
}

void melf_noteInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned char *current = (unsigned char *)section->content;

	for (current = (unsigned char *)section->content;
	     current != (unsigned char *)(section->content) + section->contentLength;
		  current += sizeof(Elf32_Nhdr) + ((Elf32_Nhdr *)current)->n_namesz + ((Elf32_Nhdr *)current)->n_descsz)
	{
		Elf32_Nhdr *currentNote     = (Elf32_Nhdr *)current;
		unsigned long currentLength = sizeof(Elf32_Nhdr) + currentNote->n_namesz + currentNote->n_descsz;

		_melf_sectionAppendEntryList(section, current, currentLength);
	}
}

void melf_noteSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long newContentLength = 0, index = 0, offset = 0;
	unsigned char *base;
	Elf32_Nhdr *current;

	while ((current = melf_noteEnum(melf, section, index++)))
		newContentLength += sizeof(Elf32_Nhdr) + current->n_namesz + current->n_descsz;

	if ((!newContentLength) ||
	    (!(base = (unsigned char *)malloc(newContentLength))))
		return;

	index  = 0;
	offset = 0;
	
	while ((current = melf_noteEnum(melf, section, index++)))
	{
		unsigned long currentLength = sizeof(Elf32_Nhdr) + current->n_namesz + current->n_descsz;

		memcpy(base + offset, current, currentLength);

		offset += currentLength;
	}

	section->content       = base;
	section->contentLength = newContentLength;
}

Elf32_Nhdr *melf_noteAdd(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Word type, const char *name, unsigned char *desc, unsigned long descLength)
{
	Elf32_Nhdr *res = NULL;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
	{
		unsigned long entryLength = sizeof(Elf32_Nhdr);
		unsigned long nameLength  = strlen(name ? name : "") + 1, padDescLength = descLength;
		unsigned char *entry;
	
		if (nameLength % 4 != 0)
			nameLength += 4 - (nameLength %4);
		if (padDescLength % 4 != 0)
			padDescLength += 4 - (padDescLength %4);
	
		entryLength += nameLength + padDescLength;

		if ((entry = (unsigned char *)_melf_sectionAppendEntryList(note, NULL, entryLength)))
		{
			res = (Elf32_Nhdr *)entry;

			res->n_namesz = nameLength;
			res->n_descsz = padDescLength;

			if (name)
				memcpy(entry + sizeof(Elf32_Nhdr), name, strlen(name));
			if (desc)
				memcpy(entry + sizeof(Elf32_Nhdr) + nameLength, desc, descLength);
		}
	}
	else
		res = _melf_noteAllocate(note, name, desc, descLength);	

	if (res)
		res->n_type = type;

	return res;
}

Elf32_Nhdr *melf_noteEnum(MELF *melf, ELF_SPEC_HEADER *note, unsigned long index)
{
	Elf32_Nhdr *res = NULL;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		res = (Elf32_Nhdr *)_melf_sectionEnumEntryList(note, index);
	else
	{
		Elf32_Nhdr *base = (Elf32_Nhdr *)note->content;
		unsigned char *curr = NULL;
		unsigned long cindex = 0;

		for (curr = (unsigned char *)base, cindex = 0;
				curr != (unsigned char *)(base) + note->contentLength;
				curr += sizeof(Elf32_Nhdr) + ((Elf32_Nhdr *)curr)->n_namesz + ((Elf32_Nhdr *)curr)->n_descsz, cindex++)
		{
			if (cindex == index)
			{
				res = (Elf32_Nhdr *)curr;

				break;
			}
		}
	}

	return res;
}

unsigned long melf_noteRemove(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Nhdr *item)
{
	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		_melf_sectionRemoveEntryList(note, item);
	else
	{
		if (!item)
			return 0;
	
		_melf_noteShrink(note, item);
	}

	return 1;
}

unsigned long melf_noteGetType(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Nhdr *item)
{
	return (item) ? item->n_type : 0;
}

const char *melf_noteGetName(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Nhdr *item)
{
	return (item && item->n_namesz) ? (unsigned char *)item + sizeof(Elf32_Nhdr) : NULL;
}

unsigned char *melf_noteGetDesc(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Nhdr *item)
{
	return (item && item->n_descsz) ? (unsigned char *)item + sizeof(Elf32_Nhdr) + item->n_namesz : NULL;
}

Elf32_Nhdr *_melf_noteAllocate(ELF_SPEC_HEADER *note, const char *name, const char *desc, unsigned long descLength)
{
	unsigned long nameLength = strlen(name ? name : "");
	unsigned long newLength = note->contentLength + sizeof(Elf32_Nhdr);
	Elf32_Nhdr *base        = (Elf32_Nhdr *)note->content;

	if (nameLength % 4 != 0)
		nameLength += 4 - (nameLength %4);
	if (descLength % 4 != 0)
		descLength += 4 - (descLength %4);

	// Add padded sizes
	newLength += nameLength + descLength;

	if (!note->content)
		base = (Elf32_Nhdr *)malloc(newLength);
	else
		base = (Elf32_Nhdr *)realloc(note->content, newLength);

	if (!base)
		return NULL;

	note->content = (void *)base;
 	base          = (Elf32_Nhdr *)((char *)base + note->contentLength);
	
	memset(base, 0, newLength - note->contentLength);

	note->contentLength    = newLength;

	base->n_namesz = nameLength;
	base->n_descsz = descLength;

	if (name)
		strncpy((unsigned char *)base + sizeof(Elf32_Nhdr), name, nameLength);
	if (desc)
		memcpy((unsigned char *)base + sizeof(Elf32_Nhdr) + nameLength, desc, descLength);

	return base;
}

void _melf_noteShrink(ELF_SPEC_HEADER *note, Elf32_Nhdr *shrinkAt)
{
	Elf32_Nhdr *base = (Elf32_Nhdr *)note->content;
	unsigned long newLength = note->contentLength - (sizeof(Elf32_Nhdr) + shrinkAt->n_namesz + shrinkAt->n_descsz);
	unsigned long shrinkLength = sizeof(Elf32_Nhdr) + shrinkAt->n_namesz + shrinkAt->n_descsz;
	unsigned long baseOffset   = 0;

	if (!base || !note->contentLength)
		return;

	if (newLength)
		note->content = (void *)malloc(newLength);
	else // Else we got rid of all the note's
	{
		free(base);

		note->content       = NULL;
		note->contentLength = 0;

		return;
	}

	memset(note->content, 0, newLength);

	if (shrinkAt - base)
		memcpy(note->content, base, (baseOffset = (unsigned char *)(shrinkAt) - (unsigned char *)(base)));
	if ((unsigned char *)(shrinkAt) + shrinkLength - (unsigned char *)(base) != note->contentLength)
		memcpy(note->content + baseOffset, 
					(unsigned char *)(shrinkAt) + shrinkLength, 
					(unsigned char *)(base) + note->contentLength - ((unsigned char *)(shrinkAt) + shrinkLength));

	free(base);

	note->contentLength = newLength;
}
