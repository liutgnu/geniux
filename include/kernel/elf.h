//elf head
struct elf_head_struct {
  unsigned char e_ident[16];
  unsigned short int e_type;
  unsigned short int e_machine;
  unsigned int e_version;
  unsigned int e_entry;  //entry point address
  unsigned int e_phoff;  //start of program headers
  unsigned int e_shoff;  //start of section headers
  unsigned int e_flags;
  unsigned short int e_ehsize;  //size of elf head
  unsigned short int e_phentsize;  //size of program headers
  unsigned short int e_phnum;  //number of program headers
  unsigned short int e_shentsize;  //size of section headers
  unsigned short int e_shnum;  //number of section headers
  unsigned short int e_shstrndx;  //section header string table index
};

//section head:entry,elf_head.e_shoff size,eshentsize*e_shnum
struct section_head_struct{
  unsigned int sh_name;
  unsigned int sh_type;
  unsigned int sh_flags;
  unsigned int sh_addr;
  unsigned int sh_offset;
  unsigned int sh_size;
  unsigned int sh_link;
  unsigned int sh_info;
  unsigned int sh_addralign;
  unsigned int sh_entsize;
};

//program segment head:entry,e_phoff size,e_phentsize*e_phnum
struct program_segment_head_struct{
  unsigned int p_type;
  unsigned int p_offset;
  unsigned int p_vaddr;
  unsigned int p_paddr;
  unsigned int p_filesz;
  unsigned int p_memsz;
  unsigned int p_flags;
  unsigned int p_align;
};
