char buf[]="Hello,greetings from parent's exec!!!";

void print_Z(char ch,int z)
{
	int int_num=1;
	__asm__ ("int $0x80"
		::"a" (int_num),"b" (ch), "c" (z));
}

void main()
{
	int i=0;
	for (i=0;i<37;i++)
	{
		print_Z(buf[i],i+80);
	}
	for (;;);
}