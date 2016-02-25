struct tss_struct{
  unsigned long back_link;
  unsigned long esp0;
  unsigned long ss0;
  unsigned long esp1;
  unsigned long ss1;
  unsigned long esp2;
  unsigned long ss2;
  unsigned long cr3;
  unsigned long eip;
  unsigned long eflags;
  unsigned long eax;
  unsigned long ecx;
  unsigned long edx;
  unsigned long ebx;
  unsigned long esp;
  unsigned long ebp;
  unsigned long esi;
  unsigned long edi;
  unsigned long es;
  unsigned long cs;
  unsigned long ss;
  unsigned long ds;
  unsigned long fs;
  unsigned long gs;
  unsigned long ldt;
  unsigned long trace_bitmap;
};

struct gldt_struct{  //1 item of *dt
  unsigned short int seg_limit;
  unsigned short int B_0_15;
  unsigned short int B_16_23;
  unsigned short int B_24_31;
};

struct task_struct{
  long state;  //-1 can't run, 0 can run
  long pid;
  struct task_struct * p_parent;
  struct task_struct * p_child;
  struct gldt_struct ldt[3];
  struct tss_struct tss;
};
