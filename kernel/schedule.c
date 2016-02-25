#define TSS(n) ((n<<4)+32)
#define TASK_QUATITY 64
/*
#define switch_to(n)  {\
  struct {long a,b;} m_tmp;\
  if (n==current->pid)\
    __asm__("jmp 1f\n\t");\
  else\
  {\
    current=p_tasks[n];\
    __asm__ ("movl %%ebx,%1\n\t"\
	     "ljmp *%0\n\t"\
	     "1:"\
	     ::"m" (*&m_tmp.a),"m" (*&m_tmp.b),"b" (TSS(n)));	\
  }\
}
*/
#include <kernel/tsk.h>

extern struct task_struct * current;
extern struct task_struct * p_tasks[];

void schedule(int cpl)
{
  int tmp=(current->pid)+1;
  if (cpl==0)
    return;
  else if (cpl==3)
  {
    while(p_tasks[tmp]==(struct task_struct *)0 || p_tasks[tmp]->state==-1)
      tmp=(tmp+1)%TASK_QUATITY;
    // switch_to(tmp);
    struct {long a,b;} m_tmp;
    if (tmp==current->pid)
      __asm__("jmp 1f\n\t");			
    else
    {						
      current=p_tasks[tmp];			
      __asm__ ("movl %%ebx,%1\n\t"		
	       "ljmp *%0\n\t"			
	       "1:"						
	       ::"m" (*&m_tmp.a),"m" (*&m_tmp.b),"b" (TSS(tmp)));	
    }
  }
  else
    return;
}
