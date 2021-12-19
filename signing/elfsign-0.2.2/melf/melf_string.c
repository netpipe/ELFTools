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

ELF_SPEC_HEADER *melf_stringTableCreate(MELF *melf, const char *name)
{
	ELF_SPEC_HEADER *sect = melf_sectionAdd(melf);

	if (!sect)
		return NULL;

	if (name)
		melf_sectionSetName(melf, sect, name);

	melf_sectionSetType(melf, sect, SHT_STRTAB);
#ifdef SHF_STRINGS
	melf_sectionSetFlags(melf, sect, SHF_ALLOC | SHF_STRINGS);
#else
	melf_sectionSetFlags(melf, sect, SHF_ALLOC);
#endif

	// Set initial blank string.
	sect->content            = (void *)malloc(1);
	sect->contentLength      = 1;
	*((char *)sect->content) = 0;

	return sect;
}

void melf_stringTableInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long currentOffset = 0, currentLength = 0;
	char *content = (char *)section->content;

	for (currentOffset = 0;
	     currentOffset < section->contentLength;
	     currentOffset += currentLength)
	{
		char *current = content + currentOffset;

		// Walk until a NULL is found 
		for (currentLength = 0;
		     currentOffset + currentLength < section->contentLength && current[currentLength];
		     currentLength++);

		// Increment to include the null terminator in the copy
		currentLength++;

		// If we didn't exceed our bounds, add the entry
		if (currentOffset + currentLength <= section->contentLength)
			_melf_sectionAppendEntryList(section, current, currentLength);
	}
}

void melf_stringTableSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *section)
{
	unsigned long newContentLength = 0, index = 0, offset = 0;
	char *entry, *newContent = NULL;

	// Calculate the size of the raw string table
	while ((entry = _melf_sectionEnumEntryList(section, index++)))
		newContentLength += strlen(entry) + 1;

	// Allocate the raw buffer for the string table
	if ((!newContentLength) ||
	    (!(newContent = (char *)malloc(newContentLength))))
		return;

	index = 0;

	// Populate the raw content buffer
	while ((entry = _melf_sectionEnumEntryList(section, index++)))
	{
		unsigned long entryLength = strlen(entry) + 1;

		memcpy(newContent + offset, entry, entryLength);

		offset += entryLength;
	}

	// Update the section's content
	section->content       = newContent;
	section->contentLength = newContentLength;
}

/*
 * Appends a string if it does not exist in the string table
 *
 * requires: write access
 */
unsigned long melf_stringTableSetString(MELF *melf, ELF_SPEC_HEADER *stringTable, const char *name)
{
	unsigned long nameLength = (name) ? strlen(name) + 1 : 0;
	unsigned long index = -1;

	if (!MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		return 0;

	// Check to see if the name already exists
	index = melf_stringTableGetStringIndex(melf, stringTable, name);

	// If the index is zero and the first byte of the string is non-null, 
	// insert the string into the table
	if ((index == -1) && (name[0]))
	{
		_melf_sectionAppendEntryList(stringTable, (char *)name, nameLength);

		// After the string is added, get its relative index
		index = melf_stringTableGetStringIndex(melf, stringTable, name);
	}

	return index;
}

/*
 * Get the index of the supplied string
 */
unsigned long melf_stringTableGetStringIndex(MELF *melf, 
		ELF_SPEC_HEADER *stringTable, const char *name)
{
	unsigned long index = -1, nameLength = strlen(name) + 1;

	// If we have write access, we must do the lookup against the list,
	// otherwise we can reference it more quickly in the raw buffer.
	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
	{
		unsigned long currentOffset = 0, currentLength = 0;
		unsigned long listIndex = 0;
		char *current;

		while ((current = _melf_sectionEnumEntryList(stringTable, listIndex++)))
		{
			currentLength = strlen(current) + 1;

			// If this entry matches the one being looked for, we win.
			if ((currentLength == nameLength) &&
			    (!memcmp(current, name, nameLength)))
			{
				index = currentOffset;
				break;
			}

			// Otherwise, increment the offset and continue on.
			currentOffset += currentLength;
		}
	}
	else
	{
		unsigned long currentOffset = 0, currentLength = 0;
		char *current = (char *)stringTable->content;

		for (currentOffset = 0;
		     currentOffset < stringTable->contentLength;
		     currentOffset += currentLength)
		{
			// Calculate the length of the current string
			for (currentLength = 0;
			     currentOffset + currentLength < stringTable->contentLength && current[currentLength];
			     currentLength++);

			currentLength++;

			// Overflow?
			if (currentOffset + currentLength > stringTable->contentLength)
				break;

			// Does this match the requested string?
			if ((nameLength == currentLength) && 
			    (!memcmp(current, name, nameLength)))
			{
				// If so, set the index to the current offset.
				index = currentOffset;
				break;
			}

			// If not, go on to the next string.
			current += currentLength;
		}
	}

	return index;
}

const char *melf_stringTableGetString(MELF *melf, ELF_SPEC_HEADER *stringTable, unsigned long index)
{
	const char *string = NULL;

	if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
	{
		unsigned long currentOffset = 0, currentLength = 0, listIndex = 0;
		char *current;

		// Enumerate the list until we reach the index supplied, or until we 
		// reach the end.
		while ((!string) &&
		       (current = (char *)_melf_sectionEnumEntryList(stringTable, listIndex++)))
		{
			currentLength = strlen(current) + 1;

			// If the currentOffset is less than the index, continue
			if ((currentOffset < index) ||
			    (currentOffset + currentLength < index))
			{
				currentOffset += currentLength;
				continue;
			}

			// In the event that the index is really somewhere inside the string, 
			// add the difference.
			currentOffset += index - currentOffset;

			string = current;
		}
		
	}
	else
	{
		if (stringTable && stringTable->contentLength > index)
			string = (char *)(stringTable->content) + index;
	}

	return string;
}
