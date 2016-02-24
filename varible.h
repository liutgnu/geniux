/*
*for use of keyboard
*/
#define BUF_LENGTH 10
#define TASK_QUATITY 2  //we have only 2 tasks

extern int mode;  //shift down mode==1; shift up mode==0
extern int buf_full_carry;
extern int keyboard_crisis;
extern char keyboard_buf[BUF_LENGTH];
extern char *keyboard_buf_head;
extern char *keyboard_buf_tail;
extern long current;
extern long jiffies;

/*
*for use of task[TASK_QUATITY]
*/
extern struct task_struct task[];
typedef void (*f_void)();  //for use of void do_self(int sc);

