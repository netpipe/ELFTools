/*
 * nologin   http://www.nologin.org
 * --------------------------------
 *
 * ELF manipulation library
 *
 * skape
 * mmiller@hick.org
 */
#ifndef _MELF_INTERNAL_H
#define _MELF_INTERNAL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/in.h>

ELF_SPEC_HEADER *_melf_listAppend(ELF_SPEC_HEADER_LIST *list, void *header, unsigned long headerLength, void *content, unsigned long contentLength); 
unsigned long _melf_listRemove(ELF_SPEC_HEADER_LIST *list, unsigned long id);
unsigned long _melf_listFlush(ELF_SPEC_HEADER_LIST *list);
void _melf_listDestroyItem(ELF_SPEC_HEADER *header);

/*
 * These methods are used for inline buffer manipulation where one large buffer is being used (read-only access).
 *
 * This method is more efficient, but has problems with memory addresses changing between modifications.
 */
void *_melf_sectionAllocateItem(ELF_SPEC_HEADER *sect, unsigned long itemSize);
void _melf_sectionShrink(ELF_SPEC_HEADER *sect, void *at, unsigned long itemSize);
void _melf_sectionDestroy(ELF_SPEC_HEADER *sect);

/*
 * These methods are used to operate on items that are really stored in a linked list with next/prev pointers appended to the backs of the 
 * structures.
 */
void *_melf_sectionAppendEntryList(ELF_SPEC_HEADER *sect, void *item, unsigned long itemSize);
void _melf_sectionRemoveEntryList(ELF_SPEC_HEADER *sect, void *item);
void *_melf_sectionEnumEntryList(ELF_SPEC_HEADER *sect, unsigned long index);
void _melf_sectionFlushEntryList(ELF_SPEC_HEADER *sect);

void _melf_sectionInitializeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);
void _melf_sectionSynchronizeEntryList(MELF *melf, ELF_SPEC_HEADER *sect);

/*
 * Program header stuff
 */
void _melf_programDestroy(ELF_SPEC_HEADER *sect);

#endif
