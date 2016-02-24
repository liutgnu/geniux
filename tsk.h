/*we need to define a simple process*/
struct task_struct{
long state;	//-1 can't run,0 can run
long pid;	//0 TSS_0,1 TSS_1
/*i guess that's all we need so far*/
};
