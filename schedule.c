/*
*in this file, we will deal with the most tricky things,maybe.
*if a program is running in kernel mode, it shouldn't be interrupted
*by timer, though modern OS can handle this perfectly. i have no solution at all.
*so no interrupts in kernel mode!!!
*
*i have to say "disable all interrupts" is very ugly, if there are two user programms
*running, one is counting time. if another is running in kernel mode, a timer 
*interrupt comes, CPU would ignore it. so the time counting would have some loss.
*i don't know how to deal with that.
*
*we check the previous cpl of CS, if cpl==0 (kernel mode) ,then exit; else (user mode),
*get into interrupts'.
*/

/*
*struct task_struct{
*long state;	//-1 can't run,0 can run
*long pid;	//0 TSS_0,1 TSS_1
*}
*/
#include "tsk.h"
#include "varible.h"
#define TSS(n) ((((unsigned long) n<<1)+4)<<3) //depend on tss_n in gdt, if gdt changed, so did TSS(n)

void schedule(int cpl);
void hangup();
void wakeup(int pid);

/*
*in the following program, "1:" can't be replaced as "quit:"
*because if switch_to was called 2 times, there would be 2 "quit"s,
*it's a fatal error in asm.
*
*there must be a struct {long a,b;}, it's strange, but if you don't, ljmp would 
*jump to a god-know place! i debugged for a long time. thanks to linux-0.12, i
*copied from it. like an old saying:"i'm standing on the top of giants' minds!" 
*/
#define switch_to(n) {\
struct {long a,b;} m_tmp; \
__asm__ ("cmpl %%eax,current\n\t" \
	"je 1f\n\t"  \
	"movw  %%bx,%1\n\t" \
	"movl  %%eax,current\n\t" \
	"ljmp  *%0\n\t" \
	"1:"  \
	::"m" (*&m_tmp.a),"m" (*&m_tmp.b),"a" (task[n].pid),"b" (TSS(n))); \
}

void schedule(int cpl)
{
	int next_pid=(current+1)%TASK_QUATITY;  //very simple schedule function
/*here we should check if next task is ready*/
	wakeup(next_pid);
	if (cpl==0) return; //kernel mode
	else if (cpl==3) //user mode
	{
		if (task[next_pid].state==-1) return;
		else switch_to(next_pid);
	}
//for future extension
	else return;
}

void hangup()
{
	task[current].state=-1;
	switch_to((current+1)%TASK_QUATITY);
}

void wakeup(int pid)
{
/*we have only 2 tasks,one is always ready,the other one need to check keyboard_buf*/
	if (pid==0) return;  //show time, always ready to run
	if (pid==1) //guess number
	{
		if (task[pid].state==-1)  //only when unready check; once ready, no need check 
		{
			if (buf_full_carry!=0 || keyboard_buf_head!=keyboard_buf_tail)  //not empty, ready
			task[pid].state=0;
		}
	}
}
