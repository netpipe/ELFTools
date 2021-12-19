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

void _melf_loadSections(MELF *melf, unsigned long base);
void _melf_loadPrograms(MELF *melf, unsigned long base);

void _melf_updatePrograms(MELF *melf, unsigned long elfProgramHeaderSize);

// 0.0.4.0
unsigned long melf_version()
{
	return 0x00000400;
}

MELF *melf_new()
{
	MELF *melf = (MELF *)malloc(sizeof(MELF));

	if (!melf)
		return NULL;

	memset(melf, 0, sizeof(MELF));

	memcpy(melf->header.e_ident, ELFMAG, SELFMAG);

	melf->header.e_type     = ET_NONE;
	melf->header.e_machine  = EM_NONE;
	melf->header.e_ident[4] = ELFCLASS32;
#if defined(ELF_BIG_ENDIAN)
	melf->header.e_ident[5] = ELFDATA2MSB;
#else
	melf->header.e_ident[5] = ELFDATA2LSB;
#endif
	melf->header.e_ident[6] = melf->header.e_version = EV_CURRENT;

	// For new ELF images, initialize to all access
	melf->access = MELF_ACCESS_ALL;

	return melf;
}

MELF *melf_open(const char *image, int access)
{
	MELF *melf = (MELF *)malloc(sizeof(MELF));
	unsigned char success = 0;
	unsigned long base = 0;
	struct stat statbuf;
	int fd = 0;

	if (!melf)
		return NULL;

	memset(melf, 0, sizeof(MELF));

	strncpy(melf->image, image, sizeof(melf->image) - 1);

	melf->access = access;

	do
	{
		// Can we read it?
		if ((fd = open(melf->image, O_RDONLY)) <= 0)
			break;

		// Is the size compatible?
		if ((fstat(fd, &statbuf) != 0) || (statbuf.st_size < sizeof(Elf32_Ehdr)))
			break;

		melf->imageSize = statbuf.st_size;

		// Can we mmap it?
		if (!(base = (unsigned long)mmap(NULL, melf->imageSize, PROT_READ, MAP_PRIVATE, fd, 0)))
			break;

		// Is it a potential elf image?
		if (memcmp((void *)base, ELFMAG, SELFMAG))
			break;

		memcpy(&melf->header, (void *)base, sizeof(Elf32_Ehdr));

		_melf_loadPrograms(melf, base);
		_melf_loadSections(melf, base);

		success = 1;

	} while (0);

	if (base)
		munmap((void *)base, statbuf.st_size);
	if (fd > 0)
		close(fd);
	if (!success)
		melf_destroy(melf), melf = NULL;

	return melf;
}

void melf_destroy(MELF *melf)
{
	_melf_listFlush(&melf->sections);
	_melf_listFlush(&melf->programs);

	free(melf);
}

unsigned long melf_synchronize(MELF *melf)
{
	unsigned long currentOffset = sizeof(Elf32_Ehdr), elfProgramHeaderSize = 0;
	ELF_SPEC_HEADER *curr;

	// First program headers
	melf->header.e_phnum      = melf->programs.length;
	melf->header.e_phentsize  = sizeof(Elf32_Phdr);
	melf->header.e_phoff      = currentOffset;
	currentOffset            += sizeof(Elf32_Phdr) * melf->programs.length;
	elfProgramHeaderSize      = currentOffset;

	// Synchronize the contents from the list to a buffer
	for (curr = melf_sectionGetEnum(melf);
			curr;
			curr = melf_sectionEnumNext(melf, curr))
		_melf_sectionSynchronizeEntryList(melf, curr);

	// Now for sections
	for (curr = melf_sectionGetEnum(melf);
			curr;
			curr = melf_sectionEnumNext(melf, curr))
	{
		unsigned long remainder = 0;

		// Calculate page alignment
		if (curr->spec.section.sh_addralign > 1)
		{
			unsigned long mask = (currentOffset & (curr->spec.section.sh_addralign - 1));

			if (mask)
				remainder = curr->spec.section.sh_addralign - mask;
		}

		currentOffset += remainder;

		curr->spec.section.sh_offset = currentOffset;
		curr->spec.section.sh_size   = curr->contentLength;

		if (curr->spec.section.sh_type != SHT_NOBITS)
			currentOffset += curr->contentLength;
	}
	
	melf->header.e_shnum     = melf->sections.length;
	melf->header.e_shentsize = sizeof(Elf32_Shdr);
	melf->header.e_shoff     = currentOffset;
	
	currentOffset += sizeof(Elf32_Shdr) * melf->sections.length;

	// Update program headers to the right sizes based of section references & offsets.
	_melf_updatePrograms(melf, elfProgramHeaderSize);

	return currentOffset;
}

unsigned long melf_save(MELF *melf, const char *path)
{
	unsigned long fileSize = 0;
	ELF_SPEC_HEADER *curr;
	void *map = NULL;
	int fd = 0;

	// Set the file size and synchronize the sections/program headers for saving
	fileSize = melf_synchronize(melf);

	if (!path)
		path = melf->image;

	do
	{
		unsigned long offset = 0, currSize = fileSize, diff = 4096;
		int mode = (melf->header.e_type == ET_EXEC) ? 0755 : 0644; 
		char null[diff];

		memset(null, 0, diff);

		// Only create the file 755 if it's ET_EXEC.
		if ((fd = open(path, O_RDWR | O_TRUNC | O_CREAT, mode)) <= 0)
			break;

		while (currSize)
		{
			unsigned long amt = (currSize < diff) ? currSize : diff;

			write(fd, null, amt);

			currSize -= amt;
		}

		if ((map = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) <= 0)
			break;

		memcpy(map, &melf->header, sizeof(Elf32_Ehdr));

		offset += sizeof(Elf32_Ehdr);

		// Write program headers
		for (curr = melf_programGetEnum(melf);
				curr;
				curr = melf_programEnumNext(melf, curr))
		{
			memcpy(map + offset, &curr->spec.program, sizeof(Elf32_Phdr));

			offset += sizeof(Elf32_Phdr);
		}

		// Write sections
		for (curr = melf_sectionGetEnum(melf);
				curr;
				curr = melf_sectionEnumNext(melf, curr))
		{
			if (curr->content)
			{
				memcpy(map + curr->spec.section.sh_offset, curr->content, curr->contentLength);

				offset = curr->spec.section.sh_offset + curr->spec.section.sh_size;
			}
		}

		// Write section headers
		for (curr = melf_sectionGetEnum(melf);
				curr;
				curr = melf_sectionEnumNext(melf, curr))
		{
			memcpy(map + offset, &curr->spec.section, sizeof(Elf32_Shdr));

			offset += sizeof(Elf32_Shdr);
		}

		msync(map, fileSize, MS_SYNC);

	} while (0);

	if (map)
		munmap(map, fileSize);
	if (fd > 0)
		close(fd);

	return 1;
}

void _melf_loadSections(MELF *melf, unsigned long base)
{
	ELF_SPEC_HEADER *current;
	Elf32_Shdr *fSections = (Elf32_Shdr *)((base + melf->header.e_shoff));
	unsigned long x = 0;

	do
	{
		// Make sure someone isn't being evil.
		if (IS_OVERFLOW(melf, base, fSections) || IS_OVERFLOW(melf, base, fSections + melf->header.e_shnum))
			break;

		for (; x < melf->header.e_shnum; x++)
		{
			// Make sure the content size is alright.
			if ((IS_OVERFLOW(melf, base, base + fSections[x].sh_offset)) ||
				 (IS_OVERFLOW(melf, base, base + fSections[x].sh_offset + fSections[x].sh_size)))
				continue;

			_melf_listAppend(&melf->sections, 
								 		(void *)((fSections + x)),
										sizeof(Elf32_Shdr),
										(void *)((base + fSections[x].sh_offset)),
										fSections[x].sh_size);
		}

		if (MELF_HAS_ACCESS(melf, MELF_ACCESS_WRITE))
		{
			for (current = melf_sectionGetEnum(melf);
					current;
					current = melf_sectionEnumNext(melf, current))
				_melf_sectionInitializeEntryList(melf, current);
		}

	} while (0);
}

void _melf_loadPrograms(MELF *melf, unsigned long base)
{
	Elf32_Phdr *fPrograms = (Elf32_Phdr *)((base + melf->header.e_phoff));
	unsigned long x = 0;

	do
	{
		if ((IS_OVERFLOW(melf, base, fPrograms)) || 
		    (IS_OVERFLOW(melf, base, fPrograms + melf->header.e_phnum)))
			break;

		for (; x < melf->header.e_phnum; x++)
		{
			_melf_listAppend(&melf->programs, 
								 		(void *)((fPrograms + x)),
										sizeof(Elf32_Shdr),
										NULL,
										0);
		}

	} while (0);
}

void _melf_updatePrograms(MELF *melf, unsigned long elfProgramHeaderSize)
{
	ELF_SPEC_HEADER *section = NULL, *prog = NULL;
	unsigned long diff;

	for (prog = melf_programGetEnum(melf);
			prog;
			prog = melf_programEnumNext(melf, prog))
	{
		unsigned long newSize = 0, origSize = prog->spec.program.p_filesz;

		if (prog->spec.program.p_memsz > prog->spec.program.p_filesz)
			diff = prog->spec.program.p_memsz - prog->spec.program.p_filesz;
		else
			diff = 0;

		// Enumerate sections, calculating the span size of the mapped segment
		for (section = melf_sectionGetEnum(melf);
				section;
				section = melf_sectionEnumNext(melf, section))
		{
			if (section->spec.section.sh_addr >= prog->spec.program.p_vaddr &&
			    (
			     (!prog->spec.program.p_memsz) ||
			     (section->spec.section.sh_addr < prog->spec.program.p_vaddr + prog->spec.program.p_memsz)
				 )
			   )
			{
				ELF_SPEC_HEADER *prev;
				unsigned long addrDiff = 0;

				// Find the previous logical section (in memory) to calculate wastage
				for (prev = melf_sectionGetEnum(melf);
				     prev;
				     prev = melf_sectionEnumNext(melf, prev))
				{
					unsigned long currDiff, end;

					end = prev->spec.section.sh_addr + prev->spec.section.sh_size;

					if (end > section->spec.section.sh_addr)
						continue;

					if (end < prog->spec.program.p_vaddr ||
					    end >= prog->spec.program.p_vaddr + prog->spec.program.p_memsz)
						continue;

					currDiff = section->spec.section.sh_addr - end;

					if ((!addrDiff) ||
						 (currDiff < addrDiff))
						addrDiff = currDiff;
					
				}

				newSize += section->spec.section.sh_size + addrDiff;
				
				if (section->spec.section.sh_addr == prog->spec.program.p_vaddr)
					prog->spec.program.p_offset = section->spec.section.sh_offset;
			}
		}

		// If the offset into the file is zero, we'll need to include the
		// ELF header & the program header tables
		if (!prog->spec.program.p_offset)
			newSize += elfProgramHeaderSize;

		// If no sections were within the range specified by the mapping, assume
		// it's a non-section mapped segment and use the original size.
		if (!newSize)
			newSize = origSize;

		prog->spec.program.p_filesz = newSize;
		prog->spec.program.p_memsz  = newSize;

		if (diff)
			prog->spec.program.p_filesz -= diff;

	}
}
