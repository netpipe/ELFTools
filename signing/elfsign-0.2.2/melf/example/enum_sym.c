#include <stdlib.h>
#include <stdio.h>

#include "melf.h"

void enum_symtab(MELF *melf, ELF_SPEC_HEADER *curr);

int main(int argc, char **argv)
{
	ELF_SPEC_HEADER *curr;
	MELF *melf = melf_open(argv[1], MELF_ACCESS_ALL);

	if (!melf)
		return 0;

	for (curr = melf_sectionGetEnum(melf);
	     curr;
	     curr = melf_sectionEnumNext(melf, curr))
	{
		if ((curr->spec.section.sh_type == SHT_DYNSYM) ||
		    (curr->spec.section.sh_type == SHT_SYMTAB))
		{
			enum_symtab(melf, curr);
		}
	}

	return 0;
}

void enum_symtab(MELF *melf, ELF_SPEC_HEADER *curr)
{
	Elf32_Sym *sym;
	unsigned long index = 0;

	while ((sym = melf_symbolTableEnum(melf, curr, index++)))
	{
		printf("%s\n", melf_symbolGetName(melf, curr, sym));
	}
}
