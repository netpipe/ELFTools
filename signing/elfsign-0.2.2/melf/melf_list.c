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

ELF_SPEC_HEADER *_melf_listAppend(ELF_SPEC_HEADER_LIST *list, void *header, unsigned long headerLength, void *content, unsigned long contentLength)
{
	ELF_SPEC_HEADER *curr = (ELF_SPEC_HEADER *)malloc(sizeof(ELF_SPEC_HEADER));

	if (!curr)
		return 0;

	memset(curr, 0, sizeof(ELF_SPEC_HEADER));

	if (list->tail)
	{
		list->tail->next = curr;
		curr->prev       = list->tail;
	}
	else
		list->head = curr;

	curr->identifier = ++list->seq;
	curr->index      = list->length;

	if (header)
		memcpy(&curr->spec, header, headerLength);

	if (content && contentLength)
	{
		if ((curr->content = malloc(curr->contentLength = contentLength)))
			memcpy(curr->content, content, contentLength);
	}

	list->tail  = curr;

	list->length++;

	return curr;
}

unsigned long _melf_listRemove(ELF_SPEC_HEADER_LIST *list, unsigned long id)
{
	ELF_SPEC_HEADER *curr = NULL, *dec = NULL;
	unsigned long success = 0;

	for (curr = list->head; 
			curr; 
			curr = curr->next)
	{
		if (curr->identifier == id)
		{
			// Decrement indexes from this point
			dec = curr->next;

			if (curr->prev)
				curr->prev->next = curr->next;
			if (curr->next)
				curr->next->prev = curr->prev;
			if (curr == list->head)
				list->head = curr->next;
			if (curr == list->tail)
				list->tail = curr->prev;

			_melf_listDestroyItem(curr);

			list->length--;
		}
	}

	for (;
			dec;
			dec = dec->next)
		dec->index--;

	return success;
}

unsigned long _melf_listFlush(ELF_SPEC_HEADER_LIST *list)
{
	ELF_SPEC_HEADER *head = list->head, *next;

	while (head)
	{
		next = head->next;

		_melf_listDestroyItem(head);

		head = next;

		list->length--;
	}

	return 1;
}

void _melf_listDestroyItem(ELF_SPEC_HEADER *header)
{
	switch (header->type)
	{
		case ELF_HEADER_TYPE_SECTION:
			_melf_sectionDestroy(header);
			break;
		case ELF_HEADER_TYPE_PROGRAM:
			_melf_programDestroy(header);
			break;
	}
}

