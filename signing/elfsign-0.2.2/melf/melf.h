/*
 * ELF manipulation library
 *
 * skape
 * mmiller@hick.org
 */
/**
 * @defgroup melf The Manipulate ELF library.
 *
 * The manipulate ELF library.
 */
/**
 * @addtogroup melf
 *
 * @{
 */
/**
 * @example new.c
 * Generates a new ELF binary with a few different types of sections.
 */
#ifndef _MELF_H
#define _MELF_H

#ifdef MELF_COMP
	#include "stdelf.h"
#else
	#include <stdelf.h>
#endif

#define IS_OVERFLOW(melf, base, addr) ((melf)->imageSize < ((unsigned long)(addr) - (unsigned long)(base)))

/**
 * Opening the ELF object for reading.
 */
#define MELF_ACCESS_READ            (1 << 0)
/**
 * Opening the ELF object for writing.
 */
#define MELF_ACCESS_WRITE           (1 << 1)
/**
 * Opening the ELF object for reading and writing.
 */
#define MELF_ACCESS_ALL             (MELF_ACCESS_READ | MELF_ACCESS_WRITE)

/**
 * Checks to see if the context has been opened readonly
 */
#define MELF_HAS_ACCESS(ctx, flag)  ((ctx)->access & (flag))

/**
 * The ELF header type enumeration includes section and program.
 */
enum ElfHeaderType
{
	ELF_HEADER_TYPE_SECTION = 0,
	ELF_HEADER_TYPE_PROGRAM = 1,
};

/**
 * Wrappers both section and program headers.
 *
 * @short ELF Specific header.
 */
typedef struct _elf_spec_header {

	/**
	 * The index associated with the given header
	 */
	unsigned long           index;
	/**
	 * The unique identifier for the section
	 */
	unsigned long           identifier;
	/**
	 * Type of header
	 */
	enum ElfHeaderType      type;

	/**
	 * The union for either section or program header.
	 */
	union
	{

		/**
		 * The section header data.
		 */
		Elf32_Shdr section;
		/**
		 * The program header data.
		 */
		Elf32_Phdr program;

	} spec;

	/**
	 * The content held in the section, if any.
	 */
	void                    *content;
	/**
	 * The number of bytes of content.
	 */
	unsigned long           contentLength;

	/**
	 * A pointer to the first entry in the content list.
	 */
	void                    *contentListHead;
	/**
	 * A pointer to the last entry in the content list.
	 */
	void                    *contentListTail;
	/**
	 * The number of entries in the list.
	 */
	unsigned long           contentListSize;

	/**
	 * The previous specific header in the list.
	 */
	struct _elf_spec_header *prev;
	/**
	 * The next specific header in the list.
	 */
	struct _elf_spec_header *next;

} ELF_SPEC_HEADER;

/**
 * A list that contains one or more specific headers.
 *
 * @short Specific header list.
 */
typedef struct _elf_spec_header_list {

	/**
	 * The first header in the list.
	 */
	ELF_SPEC_HEADER *head;
	/**
	 * The last header in the list.
	 */
	ELF_SPEC_HEADER *tail;

	/**
	 * The current unique identifier sequence pool.
	 */
	unsigned long   seq;
	/**
	 * The number of items in the list.
	 */
	unsigned long   length;

} ELF_SPEC_HEADER_LIST;

typedef struct _elf_section_entry {

	void *next;
	void *prev;

} ELF_SECTION_ENTRY;

/**
 * The melf context used to hold the path to an image as well as the elf, program and section headers.
 *
 * @short MELF context.
 */
typedef struct _melf {

	/**
	 * The path to the ELF image being operated on.
	 */
	char                 image[1024];
	/**
	 * The size, in bytes, of the image.
	 */
	unsigned long        imageSize;

	/**
	 * The access the image was opened with
	 */
	int                  access;

	/**
	 * The ELF header for the current ELF image.
	 */
	Elf32_Ehdr           header;

	/**
	 * The list of sections
	 */
	ELF_SPEC_HEADER_LIST sections;

	/**
	 * The list of program sections.
	 */
	ELF_SPEC_HEADER_LIST programs;

} MELF;

/**
 * Creates a blank MELF context.
 *
 * @return On success, a newly allocated MELF context is returned, otherwise NULL is returned.
 */
MELF *melf_new();
/**
 * Translates an ELF image into a MELF context which can then be editted.
 *
 * The access parameter can be one of the following:
 *
 * @li MELF_ACCESS_READ
 * 	Read access.
 * @li MELF_ACCESS_WRITE
 * 	Write access.
 * @li MELF_ACCESS_ALL
 * 	Read and Write access.
 *
 * @param  image  [in] The path to the image to edit.
 * @param  access [in] What type of access to open the image with.
 * @return On success, a newly allocated MELF context is returned, otherwise NULL is returned.
 */
MELF *melf_open(const char *image, int access);
/**
 * Destroys a MELF context, deallocating all memory associated with it.
 *
 * @param  melf [in] The context to destroy.
 */
void melf_destroy(MELF *melf);

/**
 * Synchronizes the section and program headers as far as alignment
 * and offsetting is concerned.  This is an integral part of the saving
 * process and can be used to make the in-memory version of the library
 * appear as it would on disk if it were to be saved.
 *
 * @param  melf [in] The melf context.
 * @return The size, in bytes, of the image.
 */
unsigned long melf_synchronize(MELF *melf);
/**
 * Saves a MELF context in ELF format to the specified path.
 *
 * @param  melf [in] The context to translate from.
 * @param  path [in] The path to the image to write to.
 * @return On success, 1 is returned.
 */
unsigned long melf_save(MELF *melf, const char *path);

/**
 * Returns the version of the melf library.
 * 
 * @return The version of the melf library.
 */
unsigned long melf_version();

/**
 * @}
 */

// Elf header functions

/**
 * @defgroup elfheader ELF Header
 * @ingroup  melf
 *
 * @{
 */

/**
 * Sets the raw ELF header
 *
 * @param  melf   [in] The melf context
 * @param  header [in] The raw ELF header to copy
 */
void melf_elfSetRaw(MELF *melf, Elf32_Ehdr *header);
/**
 * Gets the raw ELF header for direct access
 *
 * @param  melf   [in] The melf context
 * @return The raw ELF header associated with the context
 */
Elf32_Ehdr *melf_elfGetRaw(MELF *melf);

/**
 * Sets the type of the binary
 *
 * The type can be one of the following:
 *
 * @li ET_NONE 
 * 		No type.
 * @li ET_REL
 * 		Relocateable object file.
 * @li ET_EXEC
 * 		Executable.
 * @li ET_DYN
 * 		Shared object.
 * @li ET_CORE
 * 		Core.
 *
 * @param  melf [in] The melf context.
 * @param  type [in] The type to use.
 */
void melf_elfSetType(MELF *melf, Elf32_Half type);
/**
 * Gets the type of binary associated with the context.
 *
 * @param  melf [in] The melf context.
 * @return The type of binary.
 */
Elf32_Half melf_elfGetType(MELF *melf);

/**
 * Sets the machine the binary is designed for (e.g. EM_386)
 *
 * @param  melf    [in] The melf context.
 * @param  machine [in] The type of machine.
 */
void melf_elfSetMachine(MELF *melf, Elf32_Half machine);
/**
 * Get the machine the binary is desgined for.
 *
 * @param  melf [in] The melf context.
 * @return The type of machine.
 */
Elf32_Half melf_elfGetMachine(MELF *melf);

/**
 * Sets the ELF version. (e.g. EV_CURRENT)
 *
 * @param  melf    [in] The melf context.
 * @param  version [in] The version.
 */
void melf_elfSetVersion(MELF *melf, Elf32_Word version);
/**
 * Gets the ELF version.
 *
 * @param  melf [in] The melf context.
 * @return The ELF version.
 */
Elf32_Word melf_elfGetVersion(MELF *melf);

/**
 * Sets the entry point virtual address.
 *
 * @param  melf  [in] The melf context.
 * @param  entry [in] The entry point virtual address.
 */
void melf_elfSetEntry(MELF *melf, Elf32_Addr entry);
/**
 * Gets the entry point virtual address.
 *
 * @param  melf [in] The melf context.
 * @return The entry point virtual address.
 */
Elf32_Addr melf_elfGetEntry(MELF *melf);

/**
 * Sets the program header file offset.
 *
 * @param  melf   [in] The melf context.
 * @param  offset [in] The file offset for the program header.
 */
void melf_elfSetProgramHeaderOffset(MELF *melf, Elf32_Off offset);
/**
 * Gets the program header offset.
 *
 * @param  melf [in] The melf context.
 * @return The offset of the program header table.
 */
Elf32_Off melf_elfGetProgramHeaderOffset(MELF *melf);

/**
 * Sets the section header offset.
 *
 * @param  melf   [in] The melf context.
 * @param  offset [in] The offset of the section header table.
 */
void melf_elfSetSectionHeaderOffset(MELF *melf, Elf32_Off offset);
/**
 * Gets the section header offset.
 *
 * @param  melf [in] The melf context.
 * @return The offset of the section header table.
 */
Elf32_Off melf_elfGetSectionHeaderOffset(MELF *melf);

/**
 * Sets the number of program headers.
 *
 * @param  melf  [in] The melf context.
 * @param  count [in] The number of program headers.
 */
void melf_elfSetProgramHeaderCount(MELF *melf, Elf32_Half count);
/**
 * Gets the number of program headers.
 *
 * @param  melf [in] The melf context.
 * @return The number of program headers.
 */
Elf32_Half melf_elfGetProgramHeaderCount(MELF *melf);

/**
 * Sets the size of program header entries
 *
 * @param  melf  [in] The melf context.
 * @param  size  [in] The size of program header entries.
 */
void melf_elfSetSectionHeaderEntrySize(MELF *melf, Elf32_Half size);
/**
 * Gets the size of program header entries.
 *
 * @param  melf [in] The melf context.
 * @return The size of program header entries.
 */
Elf32_Half melf_elfGetSectionEntrySize(MELF *melf);
/**
 * Sets the size of program header entries
 *
 * @param  melf  [in] The melf context.
 * @param  size  [in] The size of program header entries.
 */
void melf_elfSetProgramHeaderEntrySize(MELF *melf, Elf32_Half size);
/**
 * Gets the size of program header entries.
 *
 * @param  melf [in] The melf context.
 * @return The size of program header entries.
 */
Elf32_Half melf_elfGetProgramEntrySize(MELF *melf);

/**
 * Sets the number of section headers.
 *
 * @param  melf  [in] The melf context.
 * @param  count [in] The number of section headers.
 */
void melf_elfSetSectionHeaderCount(MELF *melf, Elf32_Half count);
/**
 * Gets the number of section headers.
 *
 * @param  melf [in] The melf context.
 * @return The number of section headers.
 */
Elf32_Half melf_elfGetSectionHeaderCount(MELF *melf);

/**
 * Sets the index of the string table for section headers.
 *
 * @param  melf  [in] The melf context.
 * @param  index [in] The index to the string table header for section headers.
 */
void melf_elfSetStringTableIndex(MELF *melf, Elf32_Half index);
/**
 * Gets the index of the string table for section headers.
 *
 * @param  melf [in] The melf context.
 * @return The index to the string table header for section headers.
 */
Elf32_Half melf_elfGetStringTableIndex(MELF *melf);

/**
 * @}
 */

// Section functions

/**
 * @defgroup sections Section Headers
 * @ingroup  melf
 *
 * @{
 */

/**
 * Adds a new section.
 *
 * @param  melf [in] The melf context.
 * @return On success, a new blank section is returned.
 */
ELF_SPEC_HEADER *melf_sectionAdd(MELF *melf);
/**
 * Gets an enumerator for the section header table.
 *
 * @param  melf [in] The melf context.
 * @return On success, a pointer to the first entry in the section enumeration is returned, otherwise NULL is returned.
 */
ELF_SPEC_HEADER *melf_sectionGetEnum(MELF *melf);
/**
 * Gets the next entry in the section header enumeration.
 *
 * @param  melf [in] The melf context.
 * @param  en   [in] The current enumeration pointer.
 * @return The next header in the enumeration.
 */
ELF_SPEC_HEADER *melf_sectionEnumNext(MELF *melf, ELF_SPEC_HEADER *en);
/**
 * Gets the previous entry in the section header enumeration.
 *
 * @param  melf [in] The melf context.
 * @param  en   [in] The current enumeration pointer.
 * @return The previous header in the enumeration.
 */
ELF_SPEC_HEADER *melf_sectionEnumPrev(MELF *melf, ELF_SPEC_HEADER *en);

/**
 * Finds the first section header of a given type (e.g. SHT_NOTE)
 *
 * @param  melf [in] The melf context.
 * @param  type [in] The type to search for.
 * @return If a matching section is found a valid pointer will be returned, otherwise NULL is returned.
 */
ELF_SPEC_HEADER *melf_sectionFindType(MELF *melf, unsigned long type);
/**
 * Finds a section that is at the given index.
 *
 * @param  melf  [in] The melf context.
 * @param  index [in] The index to find a section at.
 * @return If a header exists at the provided index a valid pointer will be returned, otherwise NULL is returned.
 */
ELF_SPEC_HEADER *melf_sectionFindIndex(MELF *melf, unsigned long index);
/**
 * Finds the first section with a given name.
 *
 * @param  melf [in] The melf context.
 * @param  name [in] The name of the section to search for.
 * @return If a header exists with the provided name a valid pointer will be returned, otherwise NULL is returned.
 */
ELF_SPEC_HEADER *melf_sectionFindName(MELF *melf, const char *name);

/**
 * Sets the arbitrary content for the given section.
 *
 * @param  melf          [in] The melf context.
 * @param  section       [in] The section to operate on.
 * @param  content       [in] The raw content to set.  This parameter can be NULL.
 * @param  contentLength [in] The length of the raw content.  0 for no content.
 */
void melf_sectionSetContent(MELF *melf, ELF_SPEC_HEADER *section, void *content, unsigned long contentLength);
/**
 * Gets the raw content for a given section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to retrieve the content of.
 * @return If the specified section is valid and has content, a valid pointer is returned.  Otherwise, NULL is returned.
 */
void *melf_sectionGetContent(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Inserts new content into existing content for the given section.
 *
 * @param  melf          [in] The melf context.
 * @param  section       [in] The section to insert content into.
 * @param  offset        [in] The offset at which to insert the content.
 * @param  content       [in] The content to insert.
 * @param  contentLength [in] The length of the content being inserted.
 * @return On success, 1 is returned.  Otherwise, zero is returned.
 */
unsigned long melf_sectionInsertContent(MELF *melf, ELF_SPEC_HEADER *section, unsigned long offset, void *content, unsigned long contentLength);

/**
 * Sets the name of a section.  If a string table does not exist for section headers then one 
 * is created, otherwise the existing one is used.
 *
 * @param  melf [in] The melf context.
 * @param  section [in] The section to operate on.
 * @param  name [in] The name to set the section to.
 */
void melf_sectionSetName(MELF *melf, ELF_SPEC_HEADER *section, const char *name);
/**
 * Gets the name of the section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to get the name of.
 * @return If the section has a name a valid pointer is returned.  Otherwise, NULL is returned.
 */
const char *melf_sectionGetName(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Sets the specified section to a given type. (e.g. SHT_SYMTAB, SHT_STRTAB)
 *
 * The type can be one of the following standard types:
 *
 * @li SHT_NULL
 * 		No type
 * @li SHT_PROGBITS
 * 		Program data.
 * @li SHT_SYMTAB
 * 		Symbol table.
 * @li SHT_STRTAB
 * 		String table.
 * @li SHT_RELA
 * 		Relocation entries with addends.
 * @li SHT_HASH
 * 		Symbol hash table.
 * @li SHT_DYNAMIC
 * 		Dynamic linking information.
 * @li SHT_NOTE
 * 		Notes
 * @li SHT_NOBITS
 * 		Program space with no data.
 * @li SHT_REL
 * 		Relocation entries with no addends.
 * @li SHT_DYNSYM
 * 		Dynamic linker symbol table.
 *
 * There are more acceptable types, but these are the standard set.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @param  type    [in] The type to set the section to.
 */
void melf_sectionSetType(MELF *melf, ELF_SPEC_HEADER *section, unsigned long type);
/**
 * Gets the section type of a given section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to get the type of.
 * @return The section type.
 */
unsigned long melf_sectionGetType(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Set the section's file offset.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @param  offset  [in] The offset to update to.
 */
void melf_sectionSetOffset(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Off offset);
/**
 * Get the section's file offset.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @return The offset currently set on the section.
 */
Elf32_Off melf_sectionGetOffset(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Set the section's expressed content size.  Note that this does not affect the
 * size of the content that MELF thinks the section has, it merely updates
 * the section header's size value.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @param  size    [in] The expressed size to update to.
 */
void melf_sectionSetSize(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Half size);
/**
 * Get the section's expressed content size.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @return The section's expressed content size.
 */
Elf32_Half melf_sectionGetSize(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Sets the virtual address associated with the section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @param  addr    [in] The virtual address.
 */
void melf_sectionSetAddress(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Addr addr);
/**
 * Gets the virtual address associated with a section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @return The virtual address associated with the section.
 */
Elf32_Addr melf_sectionGetAddress(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Sets the link section associated with the section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @param  link    [in] The index of the section that is to be associated.
 */
void melf_sectionSetLink(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Word link);
/**
 * Gets the link section associated with the section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 */
Elf32_Word melf_sectionGetLink(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Sets the info section associated with the section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @param  info    [in] The index of the section that is to be associated.
 */
void melf_sectionSetInfo(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Word info);
/**
 * Gets the info section associated with the section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 */
Elf32_Word melf_sectionGetInfo(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Set the flags on a given section.
 *
 * Flags can be one or more of the following:
 *
 * @li SHF_WRITE
 * 		Writable.
 * @li SHF_ALLOC
 * 		Occupies memory during execution.
 * @li SHF_EXECINSTR
 * 		Contains executable code.
 * @li SHF_MERGE
 * 		Might be merged.
 * @li SHF_STRINGS
 * 		Contains strings.
 * @li SHF_INFO_LINK
 * 		sh_info contains section index.
 * @li SHF_GROUP
 * 		Section is a member of a group.
 * @li SHF_TLS
 * 		Thread local storage.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to set the flags on.
 * @param  flags   [in] The flags to set.
 */
void melf_sectionSetFlags(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Word flags);
/**
 * Get the flags of a given section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to get the flags of.
 */
Elf32_Word melf_sectionGetFlags(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Set the size of an entry in the content for the section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @param  entsize [in] The size of a entry.
 */
void melf_sectionSetEntrySize(MELF *melf, ELF_SPEC_HEADER *section, Elf32_Word entsize);
/**
 * Get the size of an entry in the content for the section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @return The size of an entry.
 */
Elf32_Word melf_sectionGetEntrySize(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Get the index of a given section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @return The index of the section.
 */
unsigned long melf_sectionGetIndex(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Set the string table to associate with the section.  This is used for things like symbol tables
 * which tend to have their own string tables for symbol names.
 *
 * @param  melf        [in] The melf context.
 * @param  section     [in] The section to operate on.
 * @param  stringTable [in] The string table section to associate with.
 */
void melf_sectionSetStringTableHeader(MELF *melf, ELF_SPEC_HEADER *section, ELF_SPEC_HEADER *stringTable);
/**
 * Get the string table associated with a section.
 *
 * @param  melf    [in] The melf context.
 * @param  section [in] The section to operate on.
 * @return The string table section associated with the supplied section, if any.
 */
ELF_SPEC_HEADER *melf_sectionGetStringTableHeader(MELF *melf, ELF_SPEC_HEADER *section);

/**
 * Remove a given section by its identifier.
 *
 * @param  melf [in] The melf context.
 * @param  id   [in] The identifier of the section to remove.
 * @return 1 if successful.
 */
unsigned long melf_sectionRemove(MELF *melf, unsigned long id);

/**
 * @}
 */

/**
 * @defgroup programs Program Headers
 * @ingroup  melf
 *
 * @{
 */

/**
 * Adds a new program header.
 *
 * @param  melf [in] The melf context.
 * @return On success, a new blank program  headeris returned.
 */
ELF_SPEC_HEADER *melf_programAdd(MELF *melf);
/**
 * Gets an enumerator for the program header table.
 *
 * @param  melf [in] The melf context.
 * @return On success, a pointer to the first entry in the program enumeration is returned, otherwise NULL is returned.
 */
ELF_SPEC_HEADER *melf_programGetEnum(MELF *melf);
/**
 * Gets the next entry in the program header enumeration.
 *
 * @param  melf [in] The melf context.
 * @param  en   [in] The current enumeration pointer.
 * @return The next header in the enumeration.
 */
ELF_SPEC_HEADER *melf_programEnumNext(MELF *melf, ELF_SPEC_HEADER *en);
/**
 * Gets the previous entry in the program header enumeration.
 *
 * @param  melf [in] The melf context.
 * @param  en   [in] The current enumeration pointer.
 * @return The previous header in the enumeration.
 */
ELF_SPEC_HEADER *melf_programEnumPrev(MELF *melf, ELF_SPEC_HEADER *en);

/**
 * Set the program header type.
 *
 * type can be one of the following:
 *
 * @li PT_LOAD
 * 		Loadable segment.
 * @li PT_DYNAMIC
 * 		Dynamic linking information.
 * @li PT_INTERP
 * 		Program interpreter.
 * @li PT_NOTE
 * 		Auxiliary information.
 * @li PT_PHDR
 * 		Program header table.
 * @li PT_TLS
 * 		Thread-local storage.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @param  type    [in] The type to set it to.
 */
void melf_programSetType(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Word type);
/**
 * Gets the type of a program header.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @return The type of a program header.
 */
Elf32_Word melf_programGetType(MELF *melf, ELF_SPEC_HEADER *program);

/**
 * Sets the virtual address associated with a program header.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @param  addr    [in] The virtual address to set to.
 */
void melf_programSetVirtualAddress(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Addr addr);
/**
 * Gets the virtual address associated with a program header.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @return The virtual address.
 */
Elf32_Addr melf_programGetVirtualAddress(MELF *melf, ELF_SPEC_HEADER *program);

/**
 * Sets the physical address associated with a program header.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @param  addr    [in] The physical address to set to.
 */
void melf_programSetPhysicalAddress(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Addr addr);
/**
 * Gets the physical address associated with a program header.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @return The physical address.
 */
Elf32_Addr melf_programGetPhysicalAddress(MELF *melf, ELF_SPEC_HEADER *program);

/**
 * Sets the virtual size of the program segment.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @param  size    [in] The virtual size.
 */
void melf_programSetVirtualSize(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Word size);
/**
 * Gets the virtual size of the program segment.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @return The virtual size of the program segment.
 */
Elf32_Word melf_programGetVirtualSize(MELF *melf, ELF_SPEC_HEADER *program);

/**
 * Sets the physical size of the program segment.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @param  size    [in] The physical size of the program segment.
 */
void melf_programSetPhysicalSize(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Word size);
/**
 * Gets the physical size of the program segment.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @return The physical size of the program segment.
 */
Elf32_Word melf_programGetPhysicalSize(MELF *melf, ELF_SPEC_HEADER *program);

/**
 * Sets the flags on the program header.
 *
 * flags can be one or more of the following:
 *
 * @li PF_R
 * 		Readable.
 * @li PF_W
 * 		Writable.
 * @li PF_X
 * 		Executable.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @param  flags   [in] The flags to set.
 */
void melf_programSetFlags(MELF *melf, ELF_SPEC_HEADER *program, Elf32_Word flags);
/**
 * Gets the flags associated with a program header.
 *
 * @param  melf    [in] The melf context.
 * @param  program [in] The program header to operate on.
 * @return The program header flags.
 */
Elf32_Word melf_programGetFlags(MELF *melf, ELF_SPEC_HEADER *program);

/**
 * Removes a program header of a given identifier.
 *
 * @param  melf [in] The melf context.
 * @param  id   [in] The identifier of the program header to be removed.
 * @return 1 on success.
 */
unsigned long melf_programRemove(MELF *melf, unsigned long id);

/**
 * @}
 */

/**
 * @defgroup sect_strings String Table
 * @ingroup  sections
 *
 * @{
 */

/**
 * Creates a new string table section with the specified name.  If the name 
 * is NULL, the name for the new section will not be set.
 *
 * @param  melf [in] The melf context.
 * @param  name [in] The name of the string table.  This can be NULL.
 * @return On success an initialized string table section header is returned.  Otherwise, NULL is returned.
 */
ELF_SPEC_HEADER *melf_stringTableCreate(MELF *melf, const char *name);

/**
 * Initializes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_stringTableInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);
/**
 * Synchronizes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_stringTableSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);

/**
 * Sets a string in the string table.  If the name exists in the string table its index will be 
 * re-used.  Otherwise, it is appended to the end.  If the operation fails, -1 is returned.
 *
 * @param  melf        [in] The melf context.
 * @param  stringTable [in] The string table to operate on.
 * @param  name        [in] The name to set.
 * @return The index to the name in the string table content, or -1 on failure.
 */
unsigned long melf_stringTableSetString(MELF *melf, ELF_SPEC_HEADER *stringTable, const char *name);
/**
 * Gets the index of a given string in the string table.  If the index cannot be found, -1 will be
 * returned.
 *
 * @param  melf        [in] The melf context.
 * @param  stringTable [in] The string table to operate on.
 * @param  name        [in] The name of the index being searched for.
 * @return The index to the name of the string table content, or -1 on failure.
 */
unsigned long melf_stringTableGetStringIndex(MELF *melf, ELF_SPEC_HEADER *stringTable, const char *name);
/**
 * Gets the string value at the specified index in the string table.
 *
 * @param  melf        [in] The melf context.
 * @param  stringTable [in] The string table to operate on.
 * @param  index       [in] The index to get the string at.
 * @return A pointer to a valid string on success, otherwise NULL.
 */
const char *melf_stringTableGetString(MELF *melf, ELF_SPEC_HEADER *stringTable, unsigned long index);

/**
 * @}
 */

/**
 * @defgroup sect_dynamic Dynamic
 * @ingroup  sections
 *
 * @{
 */

/**
 * Creates and initialize a dynamic section header.
 *
 * @param  melf [in] The melf context.
 * @return On success, an initialized dynamic section header is returned.  Otherwise, NULL is returned.
 */
ELF_SPEC_HEADER *melf_dynamicCreate(MELF *melf);

/**
 * Initializes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_dynamicInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);
/**
 * Synchronizes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_dynamicSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);


/**
 * Adds a tag to the dynamic section.
 *
 * tag can be one of the following:
 *
 * @li DT_NULL
 * 		Blank entry.
 * @li DT_NEEDED
 * 		Name of a required dynamic library.
 * @li DT_PLTRELSZ
 * 		Size in bytes of the PLT relocs.
 * @li DT_PLTGOT
 * 		Processor defined value.
 * @li DT_HASH
 * 		Address of the symbol table hash.
 * @li DT_STRTAB
 * 		Address of the string table.
 * @li DT_SYMTAB
 * 		Address of the symbol table.
 * @li DT_RELA
 * 		Address of the rela relocs.
 * @li DT_RELASZ
 * 		Size of the rela relocs.
 * @li DT_RELAENT
 * 		Size of a rela entry.
 * @li DT_STRSZ
 * 		Size of the sring table.
 * @li DT_SYMENT
 * 		Size of a symbol entry.
 * @li DT_INIT
 * 		Address of init function.
 * @li DT_FINI
 * 		Address of fini function.
 * @li DT_SONAME
 * 		Name of shared object.
 * @li DT_REL
 * 		Address of the rel relocs.
 * @li DT_RELSZ
 * 		Size of the rel relocs.
 * @li DT_RELENT
 * 		Size of a rel entry.
 * @li DT_PLTREL
 * 		Type of reloc in PLT.
 * @li DT_TEXTREL
 * 		Reloc might modify .text.
 * @li DT_JMPREL
 * 		Address of PLT relocs.
 * @li DT_RUNPATH
 * 		Library search path.
 * @li DT_FLAGS
 * 		Flags for object being loaded.
 * @li DT_ENCODING
 * 		Start of encoding range.
 *
 * There are a few more tags that are not specified here.
 *
 * @param  melf    [in] The melf context.
 * @param  dynamic [in] The dynamic section to operate on.
 * @param  tag     [in] The tag to set.
 * @param  val     [in] The arbitrary value associated with the tag.
 * @return On succes, a new dynamic entry structure is returned.  Otherwise, NULL is returned.
 */
Elf32_Dyn *melf_dynamicAddTag(MELF *melf, ELF_SPEC_HEADER *dynamic, Elf32_Sword tag, Elf32_Word val);
/**
 * Removes all instances of a specified tag.
 *
 * @param  melf    [in] The melf context.
 * @param  dynamic [in] The dynamic section to operate on.
 * @param  tag     [in] The tag to remove.
 * @return The number of instances removed.
 */
unsigned long melf_dynamicRemoveTag(MELF *melf, ELF_SPEC_HEADER *dynamic, Elf32_Sword tag);
/**
 * Removes an entry at a specific index.  Indexes start at 0.
 *
 * @param  melf    [in] The melf context.
 * @param  dynamic [in] The dynamic section to operate on.
 * @param  index   [in] The index to remove.
 * @return 1 if successful.
 */
unsigned long melf_dynamicRemoveIndex(MELF *melf, ELF_SPEC_HEADER *dynamic, unsigned long index);
/**
 * Gets the first instance of a specified tag.
 *
 * @param  melf    [in] The melf context.
 * @param  dynamic [in] The dynamic section to operate on.
 * @param  tag     [in] The tag to get.
 * @return If an instance of the given tag is found a valid pointer is returned.  Otherwise, NULL is returned.
 */
Elf32_Dyn *melf_dynamicGetTag(MELF *melf, ELF_SPEC_HEADER *dynamic, Elf32_Sword tag);
/**
 * Gets the dynamic entry at a given index.  Indexes start at 0.
 *
 * @param  melf    [in] The melf context.
 * @param  dynamic [in] The dynamic section to operate on.
 * @param  index   [in] The index to get the dynamic entry of.
 * @return A valid pointer to a dynamic entry if the index is valid, otherwise NULL.
 */
Elf32_Dyn *melf_dynamicGetIndex(MELF *melf, ELF_SPEC_HEADER *dynamic, unsigned long index);
/**
 * Attempts to update a given tag if one exists, otherwise a new one is added.
 *
 * @param  melf    [in] The melf context.
 * @param  dynamic [in] The dynamic section to operate on.
 * @param  tag     [in] The tag to set.
 * @param  val     [in] The arbitrary value of a given tag.
 * @return 1 on success.
 */
unsigned long melf_dynamicSetTag(MELF *melf, ELF_SPEC_HEADER *dynamic, Elf32_Sword tag, Elf32_Word val);
/**
 * Sets the value of an entry at a specified index.
 *
 * @param  melf    [in] The melf context.
 * @param  dynamic [in] The dynamic section to operate on.
 * @param  index   [in] The index to operate on.
 * @param  val     [in] The arbitrary value.
 * @return 1 on success.
 */
unsigned long melf_dynamicSetIndex(MELF *melf, ELF_SPEC_HEADER *dynamic, unsigned long index, Elf32_Word val);

/**
 * @}
 */

/**
 * @defgroup sect_note Note
 * @ingroup  sections
 *
 * @{
 */

/**
 * Creates a note section with the provided name, optionally creating a program header for the note section.
 *
 * @param  melf                [in] The melf context.
 * @param  name                [in] The name of the section.  This can be NULL.
 * @param  createProgramHeader [in] 1 if a PT_NOTE program header is to be made, 0 if not.
 * @return On success, an initialized note section header is returned.  Otherwise, NULL is returned.
 */
ELF_SPEC_HEADER *melf_noteCreate(MELF *melf, const char *name, unsigned long createProgramHeader);

/**
 * Initializes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_noteInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);
/**
 * Synchronizes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_noteSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);

/**
 * Adds an entry to the note section.
 *
 * type can be one of the following for object files:
 *
 * @li NT_VERSION
 * 		Contains a version string.
 *
 * type can be one of the following for core files:
 *
 * @li NT_PRSTATUS
 * 		Contains a copy of prstatus struct.
 * @li NT_FPREGSET
 * 		Contains copy of fpregset struct.
 * @li NT_PRPSINFO
 * 		Contains copy of prpsinfo struct.
 * @li NT_PRXREG
 * 		Contains copy of prxregset struct.
 * @li NT_PLATFORM
 * 		String from sysinfo(SI_PLATFORM).
 * @li NT_AUXV
 * 		Contains copy of auxv array.
 * @li NT_GWINDOWS
 * 		Contains copy of gwindows struct.
 * @li NT_PSTATUS
 * 		Contains copy of pstatus struct.
 * @li NT_PSINFO
 * 		Contains copy of psinfo struct.
 * @li NT_PRCRED
 * 		Contains copy of prcred struct.
 * @li NT_UTSNAME
 * 		Contains copy of utsname struct.
 * @li NT_LWPSTATUS
 * 		Contains copy of lwpstatus struct.
 * @li NT_LWPSINFO
 * 		Contains copy of lwpinfo struct.
 * @li NT_PRFPXREG
 * 		Contains copy of fprxregset struct
 *
 * @param  melf       [in] The melf context.
 * @param  note       [in] The note section to operate on.
 * @param  type       [in] The type of note.
 * @param  name       [in] The name associated with the type.
 * @param  desc       [in] The desc associated with the type.
 * @param  descLength [in] The length of the desc.
 */
Elf32_Nhdr *melf_noteAdd(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Word type, const char *name, unsigned char *desc, unsigned long descLength);
/**
 * Enumerates the note entries at a given index.  Indexes start at 0.
 *
 * @param  melf  [in] The melf context.
 * @param  note  [in] The note section to operate on.
 * @param  index [in] The index to enumerate at.
 * @return If the index is valid, a valid pointer to a note entry is returned.  Otherwise, NULL is returned.
 */
Elf32_Nhdr *melf_noteEnum(MELF *melf, ELF_SPEC_HEADER *note, unsigned long index);
/**
 * Removes a note entry.
 *
 * @param  melf [in] The melf context.
 * @param  note [in] The note section to operate on.
 * @param  item [in] The entry to remove.
 * @return 1 on success.
 */
unsigned long melf_noteRemove(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Nhdr *item);

/**
 * Gets the type associated with a given note entry.
 *
 * @param  melf [in] The melf context.
 * @param  note [in] The note section to operate on.
 * @param  item [in] The entry to get the type of.
 * @return The type.
 */
unsigned long melf_noteGetType(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Nhdr *item);
/**
 *
 * @param  melf [in] The melf context.
 * @param  note [in] The note section to operate on.
 * @param  item [in] The entry to get the name of.
 * @return The name.
 */
const char *melf_noteGetName(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Nhdr *item);
/**
 *
 * @param  melf [in] The melf context.
 * @param  note [in] The note section to operate on.
 * @param  item [in] The entry to get the desc of.
 * @return The desc.
 */
unsigned char *melf_noteGetDesc(MELF *melf, ELF_SPEC_HEADER *note, Elf32_Nhdr *item);

/**
 * @}
 */

/**
 * @defgroup sect_symboltable Symbol Table
 * @ingroup  sections
 *
 * @{
 */

/**
 * Creates a symbol table with the specified name.
 *
 * @param  melf [in] The melf context.
 * @param  name [in] The name of the symbol table.  This can be NULL.
 * @return On success, an initialized symbol table section is returned.  Otherwise, NULL is returned.
 */
ELF_SPEC_HEADER *melf_symbolTableCreate(MELF *melf, const char *name);

/**
 * Initializes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_symbolTableInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);
/**
 * Synchronizes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_symbolTableSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);

/**
 * Adds a new symbol with the given name to the symbol table.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  name     [in] The name of the symbol.  This can be NULL.
 */
Elf32_Sym *melf_symbolTableAddSymbol(MELF *melf, ELF_SPEC_HEADER *symTable, const char *name);
/**
 * Enumerates the symbol table at the given index.  Indexes start at 0.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  index    [in] The index to enumerate at.
 * @return If the index is valid, a valid pointer to a symbol will be returned.  Otherwise, NULL is returned.
 */
Elf32_Sym *melf_symbolTableEnum(MELF *melf, ELF_SPEC_HEADER *symTable, unsigned long index);
/**
 * Removes a given symbol.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  symbol   [in] The symbol to remove.
 * @return 1 on success.
 */
unsigned long melf_symbolTableRemoveSymbol(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *symbol);

/**
 * Adjusts offsets of symbols in a related section.
 *
 * @param  melf           [in] The melf context.
 * @param  relatedSection [in] The section that has been modified and is potentially related to some symbol tables.
 * @param  offset         [in] The offset at which new content was inserted.
 * @param  contentLength  [in] The length of the content that was inserted.
 * @return On success, the number of symbols adjusted is returned.  Otherwise, 0 is returned if none were adjusted.
 */
unsigned long melf_symbolTableRecalculateOffsets(MELF *melf, ELF_SPEC_HEADER *relatedSection, unsigned long offset, unsigned long contentLength);

/**
 * @}
 */

/**
 * @defgroup sect_symbols Symbols
 * @ingroup  sect_symboltable
 *
 * @{
 */

/**
 * Sets the name of a given symbol.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @param  name     [in] The name to set it to.
 */
void melf_symbolSetName(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym, const char *name);
/**
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @return THe name of the symbol.
 */
const char *melf_symbolGetName(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym);

/**
 * Set the value of the sybmol.  This is typically the virtual address at which it is found in memory.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @param  value    [in] The arbitrary value.
 */
void melf_symbolSetValue(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym, Elf32_Addr value);
/**
 * Gets the value associated with the symbol.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @return The value of the symbol.
 */
Elf32_Addr melf_symbolGetValue(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym);

/**
 * Sets the size of the symbol in virtual memory.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @param  size     [in] The size of the symbol in virtual memory.
 */
void melf_symbolSetSize(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym, Elf32_Word size);
/**
 * Gets the size of the symbol in virtual memory.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @return The size of the symbol in virtual memory.
 */
Elf32_Word melf_symbolGetSize(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym);

/**
 * Sets the binding information of the symbol.
 *
 * binding can be one of the following:
 *
 * @li STB_LOCAL
 * 		Local symbol.
 * @li STB_GLOBAL
 * 		Global symbol.
 * @li STB_WEAK
 * 		Weak symbol.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @param  binding  [in] The symbol binding information.
 */
void melf_symbolSetBinding(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym, unsigned char binding);
/**
 * Gets the symbol binding information.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @return Binding information.
 */
unsigned char melf_symbolGetBinding(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym);

/**
 * Sets the symbol type.
 *
 * type can be one of the following:
 *
 * @li STT_NOTYPE
 * 		No type.
 * @li STT_OBJECT
 * 		Symbol is a data object.
 * @li STT_FUNC
 * 		Symbol is a function.
 * @li STT_SECTION
 * 		Symbol associated with a section.
 * @li STT_FILE
 * 		Symbol's name is a file name.
 * @li STT_COMMON
 * 		Symbol is a common data object.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @param  type     [in] The type of the symbol.
 */
void melf_symbolSetType(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym, unsigned char type);
/**
 * Gets the type of the symbol.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @return The symbol type.
 */
unsigned char melf_symbolGetType(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym);

/**
 * Sets the section index of the symbol.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @param  shndx    [in] The section index.
 */
void melf_symbolSetSectionIndex(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym, Elf32_Half shndx);
/**
 * Gets the section index of the symbol.
 *
 * @param  melf     [in] The melf context.
 * @param  symTable [in] The symbol table to operate on.
 * @param  sym      [in] The symbol to operate on.
 * @return The section index.
 */
Elf32_Half melf_symbolGetSectionIndex(MELF *melf, ELF_SPEC_HEADER *symTable, Elf32_Sym *sym);

/**
 * @}
 */

/**
 * @defgroup elfres ELF Resources
 * @ingroup  melf
 *
 * @{
 */

/**
 * Enumeration of resource types
 *
 * @short Elf resource types
 */
typedef enum _Elf32_ResType {
	/**
	 * Unknown resource
	 */
	ELF_RES_TYPE_UNKNOWN = 0,
	/**
	 * String resource
	 */
	ELF_RES_TYPE_STRING  = 1,
	/**
	 * Binary resource
	 */
	ELF_RES_TYPE_BINARY  = 2,
} Elf32_ResType;

/**
 * Elf resource object for an individual resource.
 *
 * @short Elf resource
 */
typedef struct _Elf32_Res {

	// Stored on disk:
	struct _Elf32_Res_Header {

		/**
		 * The length of the resource, including the header.
		 */
		unsigned long length;
		/**
		 * The type of the resource.
		 */
		Elf32_ResType type;
		/**
		 * The resource identifier.
		 */
		unsigned long identifier;
	} header;

	/**
	 * The resource data
	 */
	void          *data;	

	// Not stored on disk, internal use only:
	/**
	 * The index of the resource
	 */
	unsigned long index;
	/**
	 * The offset of the resource
	 */
	unsigned long offset;

} Elf32_Res;

/**
 * Opens an existing resource section, if one exists.
 *
 * @param  melf [in] The melf context.
 * @return On success, the corresponding section is returned, otherwise NULL.
 */
ELF_SPEC_HEADER *melf_resOpen(MELF *melf);
/**
 * Creates a new resource section, or opens an existing one if one exists.
 *
 * @param  melf [in] The melf context.
 * @return On success, the corresponding section is returned, otherwise NULL.
 */
ELF_SPEC_HEADER *melf_resCreate(MELF *melf);

/**
 * Initializes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_resInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);
/**
 * Synchronizes the entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_resSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);

/**
 * Enumerates the resources in a given resource section.
 *
 * @param  melf     [in]  The melf context.
 * @param  resTable [in]  The resource table section.
 * @param  index    [in]  The current enumeration index.
 * @param  res      [out] The buffer to hold the resource.
 * @return If there is a resource at the given index, 1 is returned, otherwise 0 is returned.
 */
unsigned char melf_resEnum(MELF *melf, ELF_SPEC_HEADER *resTable, unsigned long index, Elf32_Res *res);
/**
 * Gets a resource entry by a specified type.
 *
 * @param  melf     [in]  The melf context.
 * @param  resTable [in]  The resource table section.
 * @param  type     [in]  The type to search for.
 * @param  res      [out] The buffer to hold the resource.
 * @return On success, 1 is returned, otherwise 0 is returned.
 */
unsigned char melf_resGetType(MELF *melf, ELF_SPEC_HEADER *resTable, Elf32_ResType type, Elf32_Res *res);
/**
 * Gets a resource entry by a specified identifier.
 *
 * @param  melf     [in]  The melf context.
 * @param  resTable [in]  The resource table section.
 * @param  id       [in]  The identifier to search for.
 * @param  res      [out] The buffer to hold the resource.
 * @return On success, 1 is returned, otherwise 0 is returned.
 */
unsigned char melf_resGetId(MELF *melf, ELF_SPEC_HEADER *resTable, unsigned long id, Elf32_Res *res);
/**
 * Adds a new resource to the resource table.  Resource identifiers must be unique.
 *
 * @param  melf     [in]  The melf context.
 * @param  resTable [in]  The resource table section.
 * @param  type     [in]  The resource type.
 * @param  length   [in]  The length of the data buffer passed in.
 * @param  id       [in]  The identifier to associate this resource with.
 * @param  data     [in]  The arbitrary data.
 * @return On success, 1 is returned, otherwise 0 is returned.
 */
unsigned char melf_resAdd(MELF *melf, ELF_SPEC_HEADER *resTable, Elf32_ResType type, unsigned long length, unsigned long id, void *data);
/**
 * Updates a resource to the resource table.  Resource identifiers must be unique.
 * If the resource does not exist it is created.
 *
 * @param  melf     [in]  The melf context.
 * @param  resTable [in]  The resource table section.
 * @param  type     [in]  The resource type.
 * @param  length   [in]  The length of the data buffer passed in.
 * @param  id       [in]  The identifier to associate this resource with.
 * @param  data     [in]  The arbitrary data.
 * @return On success, 1 is returned, otherwise 0 is returned.
 */
unsigned char melf_resUpdate(MELF *melf, ELF_SPEC_HEADER *resTable, Elf32_ResType type, unsigned long length, unsigned long id, void *data);
/**
 * Removes a resource to the resource table.  Resource identifiers must be unique.
 *
 * @param  melf     [in]  The melf context.
 * @param  resTable [in]  The resource table section.
 * @param  res      [in]  The resource to remove.
 * @return On success, 1 is returned, otherwise 0 is returned.
 */
unsigned char melf_resRemove(MELF *melf, ELF_SPEC_HEADER *resTable, Elf32_Res *res);
/**
 * Closes a resource table.  This currently has no operation.
 *
 * @param  melf     [in]  The melf context.
 * @param  resTable [in]  The resource table section.
 */
void melf_resClose(MELF *melf, ELF_SPEC_HEADER *resTable);

/**
 * @}
 */

/**
 * @defgroup elfrelocs Relocateables
 * @ingroup  sections
 *
 * @{
 */

/**
 * Creates a relocateable section in the binary with a given name.  If addends is set to 1
 * the section will contain Elf32_Rela's, if it is set to zero, it will contain Elf32_Rel's.
 *
 * @param  melf       [in] The melf context.
 * @param  name       [in] The name of the section.
 * @param  hasAddends [in] 1 if the section will have addends, 0 if not.
 * @return On success, an valid ELF section pointer is returned.  Otherwise, NULL is returned.
 */
ELF_SPEC_HEADER *melf_relocTableCreate(MELF *melf, const char *name, unsigned char hasAddends);

/**
 * Initializes the rel entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_relocInitializeRelEntryList(MELF *melf, ELF_SPEC_HEADER *sect);
/**
 * Synchronizess the rel entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_relocSynchronizeRelEntryList(MELF *melf, ELF_SPEC_HEADER *sect);
/**
 * Initializes the rela entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_relocInitializeRelaEntryList(MELF *melf, ELF_SPEC_HEADER *sect);
/**
 * Synchronizess the rel entry list for the given section.  It is not recommended that this function be called directly.
 *
 * @param  melf [in] The melf context.
 * @param  sect [in] The section that is being operated on.
 */
void melf_relocSynchronizeRelaEntryList(MELF *melf, ELF_SPEC_HEADER *sect);

/**
 * Set the symbol table header associated with the relocateables in this section.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  symTable   [in] The symbol table to be associated.
 */
void melf_relocTableSetSymbolTableHeader(MELF *melf, ELF_SPEC_HEADER *relocTable, ELF_SPEC_HEADER *symTable);

/**
 * Adds an uninitialized relocateable entry to the table.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @return On success, a valid pointer to a reloc entry is returned.  Otherwise, NULL is returned.
 */
Elf32_Rel *melf_relocTableAddRel(MELF *melf, ELF_SPEC_HEADER *relocTable);
/**
 * Enumerates the reloc table at the given index.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  index      [in] The index at which to enumerate.
 * @return On success, a valid pointer to a reloc entry is returned.  Otherwise, NULL is returned.
 */
Elf32_Rel *melf_relocTableEnumRel(MELF *melf, ELF_SPEC_HEADER *relocTable, unsigned long index);
/**
 * Removes a reloc entry from the table.  All entries that match the reloc entry passed in
 * will be removed from the table.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rel        [in] The reloc entry to remove.
 * @return On success a non-zero value is returned to indicate the number of entries removed.  Otherwise, zero is returned.
 */
unsigned long melf_relocTableRemoveRel(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel);

/**
 * Adds an uninitialized reloc entry (with addend) to the table.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @return On success, a valid pointer to the reloc (addend) entry is returned.  Otherwise, NULL is returned.
 */
Elf32_Rela *melf_relocTableAddRela(MELF *melf, ELF_SPEC_HEADER *relocTable);
/**
 * Enumerates the reloc table at the given index.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  index      [in] The index at which to enumerate.
 * @return On success, a valid pointer to a reloc (addend) entry is returned.  Otherwise, NULL is returned.
 */
Elf32_Rela *melf_relocTableEnumRela(MELF *melf, ELF_SPEC_HEADER *relocTable, unsigned long index);
/**
 * Removes a reloc (addend) entry from the table.  All entries taht match the reloc entry passed in
 * will be removed from the table.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rela       [in] The reloc (addend) entry to be removed.
 * @return On success a non-zero value is returned to indicate the number of entries removed.  Otherwise, zero is returned.
 */
unsigned long melf_relocTableRemoveRela(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela);
/**
 * Adjusts offsets of relocateables in a related section.
 *
 * @param  melf           [in] The melf context.
 * @param  relatedSection [in] The section that has been modified and is potentially related to some reloc tables.
 * @param  offset         [in] The offset at which new content was inserted.
 * @param  contentLength  [in] The length of the content that was inserted.
 * @return On success, the number of relocs adjusted is returned.  Otherwise, 0 is returned if none were adjusted.
 */
unsigned long melf_relocRecalculateOffsets(MELF *melf, ELF_SPEC_HEADER *relatedSection, unsigned long offset, unsigned long contentLength);


/**
 * Initializes a reloc entry to a given set of values.
 *
 * @param  melf        [in] The melf context.
 * @param  relocTable  [in] The reloc section being operated on.
 * @param  rel         [in] The reloc entry to be initialized.
 * @param  offset      [in] The offset to which the reloc points.
 * @param  symbolIndex [in] The index of the symbol to which the reloc entry refers, if any.
 * @param  type        [in] The type of reloc.
 */
void melf_relocRelInitialize(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel, Elf32_Addr offset, unsigned char symbolIndex, unsigned char type);

/**
 * Sets the offset of a given reloc entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rel        [in] The reloc entry being operated on.
 * @param  offset     [in] The offset to which the reloc points.
 */
void melf_relocRelSetOffset(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel, Elf32_Addr offset);
/**
 * Gets the offset of a given reloc entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rel        [in] The reloc entry being operated on.
 * @return The offset of the reloc entry. 
 */
Elf32_Addr melf_relocRelGetOffset(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel);

/**
 * Sets the symbol index associated with the reloc entry.
 *
 * @param  melf        [in] The melf context.
 * @param  relocTable  [in] The reloc section being operated on.
 * @param  rel         [in] The reloc entry being operated on.
 * @param  symbolIndex [in] The index of the symbol to which the reloc refers.
 */
void melf_relocRelSetSymbol(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel, unsigned char symbolIndex);
/**
 * Gets the symbol index associated with the reloc entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rel        [in] The reloc entry being operated on.
 * @return The symbol index associated with the reloc entry.
 */
unsigned char melf_relocRelGetSymbol(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel);

/**
 * Sets the type of the reloc entry.
 *
 * type can be one of the following for 386:
 *
 * @li R_386_NONE
 * 	No reloc
 * @li R_386_32
 * 	Direct 32 bit
 * @li R_386_PC32
 * 	PC relative 32 bit
 * @li R_386_GOT32
 * 	32 bit GOT entry
 * @li R_386_PLT32
 * 	32 bit PLT address
 * @li R_386_COPY
 * 	Copy of symbol at runtime
 * @li R_386_GLOB_DAT
 * 	Create GOT entry
 * @li R_386_JMP_SLOT
 * 	Create PLT entry
 * @li R_386_RELATIVE
 * 	Adjust by program base
 * @li R_386_GOTOFF
 * 	32 bit offset to GOT
 * @li R_386_GOTPC
 * 	32 bit PC relative offset to GOT
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rel        [in] The reloc entry being operated on.
 * @param  type       [in] The reloc entry's type.
 */
void melf_relocRelSetType(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel, unsigned char type);
/**
 * Gets the type of the reloc entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rel        [in] The reloc entry being operated on.
 * @return The type of the reloc entry.
 */
unsigned char melf_relocRelGetType(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rel *rel);

/**
 * Initializes a reloc (addend) entry to a given set of values.
 *
 * @param  melf        [in] The melf context.
 * @param  relocTable  [in] The reloc section being operated on.
 * @param  rela        [in] The reloc (addend) entry to be initialized.
 * @param  offset      [in] The offset to which the reloc points.
 * @param  symbolIndex [in] The index of the symbol to which the reloc entry refers, if any.
 * @param  type        [in] The type of reloc.
 * @param  addend      [in] The addend associated with the reloc entry.
 */
void melf_relocRelaInitialize(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, Elf32_Addr offset, unsigned char symbolIndex, unsigned char type, Elf32_Sword addend);

/**
 * Sets the offset associated with the reloc (addend) entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rela       [in] The reloc (addend) entry.
 * @param  offset     [in] The offset.
 */
void melf_relocRelaSetOffset(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, Elf32_Addr offset);
/**
 * Gets the offset associated with the reloc (addend) entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rela       [in] The reloc (addend) entry.
 * @return The offset of the reloc (addend) entry.
 */
Elf32_Addr melf_relocRelaGetOffset(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela);

/**
 * Sets the symbol index associated with the reloc (addend) entry.
 *
 * @param  melf        [in] The melf context.
 * @param  relocTable  [in] The reloc section being operated on.
 * @param  rela        [in] The reloc (addend) entry.
 * @param  symbolIndex [in] The index of the symbol being associated.
 */
void melf_relocRelaSetSymbol(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, unsigned char symbolIndex);
/**
 * Gets the symbol index associated with the reloc (addend) entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rela       [in] The reloc (addend) entry.
 * @return The index of the associated symbol.
 */
unsigned char melf_relocRelaGetSymbol(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela);

/**
 * Sets the type of the reloc (addend) entry.
 * 
 * type can be one of the following for 386:
 *
 * @li R_386_NONE
 * 	No reloc
 * @li R_386_32
 * 	Direct 32 bit
 * @li R_386_PC32
 * 	PC relative 32 bit
 * @li R_386_GOT32
 * 	32 bit GOT entry
 * @li R_386_PLT32
 * 	32 bit PLT address
 * @li R_386_COPY
 * 	Copy of symbol at runtime
 * @li R_386_GLOB_DAT
 * 	Create GOT entry
 * @li R_386_JMP_SLOT
 * 	Create PLT entry
 * @li R_386_RELATIVE
 * 	Adjust by program base
 * @li R_386_GOTOFF
 * 	32 bit offset to GOT
 * @li R_386_GOTPC
 * 	32 bit PC relative offset to GOT
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rela       [in] The reloc (addend) entry.
 * @param  type       [in] The type of the entry.
 */
void melf_relocRelaSetType(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, unsigned char type);
/**
 * Gets the type of the reloc (addend) entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rela       [in] The reloc (addend) entry.
 * @return The type of the entry.
 */
unsigned char melf_relocRelaGetType(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela);

/**
 * Sets the addend of the reloc (addend) entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rela       [in] The reloc (addend) entry.
 * @param  addend     [in] The reloc entry addend.
 */
void melf_relocRelaSetAddend(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela, Elf32_Sword addend);
/**
 * Gets the addend of the reloc (addend) entry.
 *
 * @param  melf       [in] The melf context.
 * @param  relocTable [in] The reloc section being operated on.
 * @param  rela       [in] The reloc (addend) entry.
 * @return The reloc entry addend.
 */
Elf32_Sword melf_relocRelaGetAddend(MELF *melf, ELF_SPEC_HEADER *relocTable, Elf32_Rela *rela);

/**
 * @}
 */

#ifdef MELF_COMP
#include "melf_internal.h"
#endif

#endif
