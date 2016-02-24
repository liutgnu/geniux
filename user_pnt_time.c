#include "sys_interface.h"

void task0()
{
	int pre_time=-1;
	int lst_time=0;
	print_XY('s',3,0);
	while (1)
	{
		lst_time=time_s();
		if (lst_time!=pre_time)
		{
			print_XY((char)((lst_time%1000)/100+48),0,0);
			print_XY((char)((lst_time%100)/10+48),1,0);
			print_XY((char)((lst_time%10)/1+48),2,0);
			pre_time=lst_time;
		}
	}
}
