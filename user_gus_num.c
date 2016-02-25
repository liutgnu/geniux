#include "sys_interface.h"

int input_arry[4];
int generate_arry[4];

int tri_num()
{
	int tmp=time_ms();
	return ((tmp*tmp*tmp)%10);
}

void get_random_num()
{
	generate_arry[0]=tri_num();
	do 
		{generate_arry[1]=tri_num();} 
	while(generate_arry[1]==generate_arry[0]);
	do
		{generate_arry[2]=tri_num();}
	while(generate_arry[2]==generate_arry[0] || generate_arry[2]==generate_arry[1]);
	do
		{generate_arry[3]=tri_num();}
	while(generate_arry[3]==generate_arry[0] || generate_arry[3]==generate_arry[1] || generate_arry[3]==generate_arry[2]);
}

void input_num()
{
	input_arry[0]=(int)(input()-48);
	input_arry[1]=(int)(input()-48);
	input_arry[2]=(int)(input()-48);
	input_arry[3]=(int)(input()-48);
}

void task1()
{
	int loop_num;
	int i,j;
	int cont_a,cont_b;
	int z=83;  //start from 83. 80+'G'+'O'+'|'
	sound();
	get_random_num();
	for (loop_num=0;loop_num<15;loop_num++)
	{
		cont_a=0;
		cont_b=0;
		print_XY('G',0,1);print_XY('O',1,1);print_XY('|',2,1);
		input_num();
		for (i=0;i<4;i++)
		{
			if (generate_arry[i]==input_arry[i]) 
				cont_a++;
		}
		for (i=0;i<4;i++)
		{	
			for (j=0;j<4;j++)
			{
				if (input_arry[j]==generate_arry[i]) 
					cont_b++;
			}
		}
		cont_b=cont_b-cont_a;
		print_Z(cont_a+48,z);print_Z('A',z+1);print_Z(cont_b+48,z+2);print_Z('B',z+3);print_Z('|',z+4);
		z=z+5;
		if (cont_a==4 && cont_b==0)
		{
			print_Z('O',z);print_Z('K',z+1);print_Z('|',z+2);
			z=z+3;
		}
		else continue;				
	}
	if (loop_num>14)
	{
		print_Z('N',z);print_Z('O',z+1);print_Z('|',z+2);
		z=z+3;
		print_Z(generate_arry[3]+48,z);print_Z(generate_arry[2]+48,z+1);print_Z(generate_arry[1]+48,z+2);print_Z(generate_arry[0]+48,z+3);print_Z('|',z+4);
		z=z+5;
	}
}
