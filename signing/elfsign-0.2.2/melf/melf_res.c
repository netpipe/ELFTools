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

ELF_SPEC_HEADER *melf_resOpen(MELF *melf)
{
	return melf_sectionFindType(melf, SHT_RES);
}

ELF_SPEC_HEADER *melf_resCreate(MELF *melf)
{
	ELF_SPEC_HEADER *res = melf_sectionFindType(melf, SHT_RES);

	if (res)
		return res;

	if (!(res = melf_sectionAdd(melf)))
		return NULL;

	melf_sectionSetName(melf, res, ".res");
	melf_sectionSetType(melf, res, SHT_RES);

	return res;
}

void melf_resInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned char *current;

	for (current = (unsigned char *)section->content;
			current != (unsigned char *)(section->content) + section->contentLength;
			current += ntohl(*(unsigned long *)current))
		_melf_sectionAppendEntryList(section, current, ntohl(*(unsigned long *)current));
}

void melf_resSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long newContentLength = 0, index = 0, offset = 0;
	unsigned char *current;
	unsigned char *base;

	// Calculate the size of the section
	while ((current = (unsigned char *)_melf_sectionEnumEntryList(section, index++)))
		newContentLength += ntohl(*(unsigned long *)current);

	if ((!newContentLength) ||
	    (!(base = (unsigned char *)malloc(newContentLength))))
		return;

	index = 0;

	while ((current = (unsigned char *)_melf_sectionEnumEntryList(section, index++)))
	{
		unsigned long currentLength = ntohl(*(unsigned long *)current);

		memcpy(base + offset, current, currentLength);

		offset += currentLength;
	}

	section->content       = base;
	section->contentLength = newContentLength;
}

unsigned char melf_resEnum(MELF *melf, ELF_SPEC_HEADER *resTable, unsigned long index, Elf32_Res *res)
{
	unsigned char *current;
	unsigned long offset = 0;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
	{
		current = (unsigned char *)_melf_sectionEnumEntryList(resTable, index);

		if (!current)
			return 0;

		// Use the offset as the pointer to the entry to remove
		res->offset = (int)current;
	}
	else
	{
		unsigned long c = 0, length = 0;

		current = (unsigned char *)resTable->content;

		if (!current)
			return 0;

		// While we're not going past our buffer and we haven't reached our index, enumerate
		while (((offset + (sizeof(struct _Elf32_Res_Header))) < resTable->contentLength) && 
					(c < index))
		{
			length  = ntohl(*(unsigned long *)(current+offset));
			offset += length; // Increment offset by the current pointer;
			c++;
		}

		if (((offset + sizeof(struct _Elf32_Res_Header)) >= resTable->contentLength) ||
				(c != index))
			return 0;
		
		res->offset = offset;
	}

	res->index             = index;
	res->header.length     = ntohl(*(unsigned long *)(current+offset)) - sizeof(struct _Elf32_Res_Header);
	res->header.type       = ntohl(*(unsigned long *)(current+offset+sizeof(unsigned long)));
	res->header.identifier = ntohl(*(unsigned long *)(current+offset+sizeof(unsigned long)+sizeof(unsigned long)));

	if (res->header.length)
		res->data = (void *)(current+offset+sizeof(struct _Elf32_Res_Header));
	else
		res->data = NULL;

	return 1;
}

unsigned char melf_resGetType(MELF *melf, ELF_SPEC_HEADER *resTable, Elf32_ResType type, Elf32_Res *res)
{
	unsigned long index = 0;
	unsigned char found = 0;

	while (melf_resEnum(melf, resTable, index++, res))
	{
		if (res->header.type == type)
		{
			found = 1;
			break;
		}
	}

	return found;
}

unsigned char melf_resGetId(MELF *melf, ELF_SPEC_HEADER *resTable, unsigned long id, Elf32_Res *res)
{
	unsigned long index = 0;
	unsigned char found = 0;

	while (melf_resEnum(melf, resTable, index++, res))
	{
		if (res->header.identifier == id)
		{
			found = 1;
			break;
		}
	}

	return found;
}

unsigned char melf_resAdd(MELF *melf, ELF_SPEC_HEADER *resTable, Elf32_ResType type, unsigned long length, unsigned long id, void *data)
{
	unsigned char result = 0;
	Elf32_Res verify;

	// Make sure something with this identifier does not exist.
	if (melf_resGetId(melf, resTable, id, &verify))
		return 0;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
	{
		struct _Elf32_Res_Header *header = NULL;
		unsigned long entryLength = sizeof(struct _Elf32_Res_Header) + length;
		unsigned char *buf;
		unsigned char *payload;

		if (!(buf = (unsigned char *)malloc(entryLength)))
			return 0;

		header  = (struct _Elf32_Res_Header *)(buf);
		payload = buf + sizeof(struct _Elf32_Res_Header);

		header->length     = htonl(length + sizeof(struct _Elf32_Res_Header));
		header->identifier = htonl(id);
		header->type       = htonl(type);

		memcpy(payload, data, length);
	
		if (_melf_sectionAppendEntryList(resTable, buf, entryLength))
			result = 1;

		free(buf);
	}
	else
	{
		struct _Elf32_Res_Header *header = NULL;
		unsigned long originalLength = resTable->contentLength;
		unsigned long newLength      = originalLength + sizeof(struct _Elf32_Res_Header) + length;
		unsigned char *newBuffer     = NULL;
		unsigned char *payload       = NULL;

		do
		{
			if (!(newBuffer = (unsigned char *)realloc(resTable->content, newLength)))
				break;

			header  = (struct _Elf32_Res_Header *)(newBuffer + originalLength);
			payload = newBuffer + originalLength + sizeof(struct _Elf32_Res_Header);

			// Initialize the header
			header->length     = htonl(length + sizeof(struct _Elf32_Res_Header)); // Length includes both header and payload
			header->identifier = htonl(id);
			header->type       = htonl(type);

			// Initialize the payload
			memcpy(payload, data, length);

			// And we're done.
			resTable->content       = newBuffer;
			resTable->contentLength = newLength;

			result = 1;

		} while (0);

	}

	return result;
}

unsigned char melf_resUpdate(MELF *melf, ELF_SPEC_HEADER *resTable, Elf32_ResType type, unsigned long length, unsigned long id, void *data)
{
	Elf32_Res current;

	if (melf_resGetId(melf, resTable, id, &current))
		melf_resRemove(melf, resTable, &current);

	return melf_resAdd(melf, resTable, type, length, id, data);
}

unsigned char melf_resRemove(MELF *melf, ELF_SPEC_HEADER *resTable, Elf32_Res *res)
{
	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		_melf_sectionRemoveEntryList(resTable, (void *)res->offset);
	else
	{
		unsigned char *base        = (unsigned char *)resTable->content;
		unsigned long shrinkLength = res->header.length + sizeof(struct _Elf32_Res_Header);
		unsigned long newLength    = resTable->contentLength - (shrinkLength);
		unsigned long baseOffset   = 0;
		unsigned char *shrink      = base + res->offset;

		if (newLength)
			resTable->content = malloc(newLength);
		else
		{
			free(base);

			resTable->content       = NULL;
			resTable->contentLength = 0;

			return 1;
		}

		memset(resTable->content, 0, newLength);

		// Copy front
		if (shrink - base)
			memcpy(resTable->content, 
								 base, 
								 (baseOffset = shrink - base));
		// Copy back
		if ((shrink + shrinkLength) - base != resTable->contentLength)
			memcpy(resTable->content + baseOffset,
								 shrink + shrinkLength,
								 (base + resTable->contentLength) - (shrink + shrinkLength));

		free(base);

		resTable->contentLength = newLength;
	}

	return 1;
}

void melf_resClose(MELF *melf, ELF_SPEC_HEADER *resTable)
{
}
