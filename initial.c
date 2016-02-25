#include "tsk.h"
#include "syscall.h"
#define BUF_LENGTH 10
#define TASK_QUATITY 2  //we have only 2 tasks

int mode;  //shift down mode==1; shift up mode==0
int buf_full_carry;
int keyboard_crisis;
char keyboard_buf[BUF_LENGTH];
char *keyboard_buf_head;
char *keyboard_buf_tail;

//note! struct must be valued when initializing
struct task_struct task[TASK_QUATITY]={{0,0},{0,1}};//task[0] print time,task[1] guess num
f_int syscall_table[]={sys_input,sys_print,sys_utime,sys_sound};

void init_varibles(void)  //initial variables
{
	mode=0;  //shift mode 
	keyboard_buf_head=keyboard_buf;
	keyboard_buf_tail=keyboard_buf;
	buf_full_carry=0;
	keyboard_crisis=0;
}
