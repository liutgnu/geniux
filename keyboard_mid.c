#include "tsk.h"
#include "varible.h"
extern void hangup();

/*
*normal buf structure
*head--tail
*insert,increase tail
*delete,increase head
*/

unsigned char keyboard_buf_insert(char char_in)	//full return 0xfe,ok return 0,crisis return 0xff
{
//keyboard buf full
	if (buf_full_carry==1 && keyboard_buf_tail==keyboard_buf_head)
	{
		return 0xfe;
	}

//do we meet crisis?
/*
*here i use a varity to distiguish crisis or none crisis.
*i think i needn't mask all interrups,though cli does do a better job.
*if i mask all interrups,i believe system will be slower.
*i would come back later.
*/
	if (keyboard_crisis==1)
	{
		return 0xff;
	}

//nice!
	keyboard_crisis=1;
	*keyboard_buf_tail=char_in;
	keyboard_buf_tail++;
	if (keyboard_buf_tail==keyboard_buf+BUF_LENGTH) 
	{
		keyboard_buf_tail=keyboard_buf_tail-BUF_LENGTH;
		buf_full_carry=1;
	}
	keyboard_crisis=0;
	return 0;
}

unsigned char keyboard_buf_delete()   //empty return 0xfe,ok return char,crisis return 0xff
{
//keyboard buf empty
	if (buf_full_carry==0 && keyboard_buf_head==keyboard_buf_tail)
	{
		return 0xfe;
	}

//do we meet crisis?
	if (keyboard_crisis==1)
	{
		return 0xff;
	}

//nice!
	keyboard_crisis=1;
	char char_out=*keyboard_buf_head;
	keyboard_buf_head++;
	if (keyboard_buf_head==keyboard_buf+BUF_LENGTH)
	{
		keyboard_buf_head=keyboard_buf_head-BUF_LENGTH;
		buf_full_carry=0;
	}
	keyboard_crisis=0;
	return char_out;
}

/*
*this is a previous version, here i have found a bug.
*when run back to hangup(), char keyboard_mid() would get a VOID.
*so we need a loop here.

char keyboard_mid()  //to applications
{
	char mid_tmp=keyboard_buf_delete();
*
*here i didn't deal with buf empty and crisis seperatedly,
*since our OS have no SIGNAL so far.
*
	if (mid_tmp!=0xfe && mid_tmp!=0xff) //ok, chars in buf 
		return mid_tmp;
	else hangup();
}
*/

char keyboard_mid()
{
	unsigned char mid_tmp;
	while (1)
	{
		mid_tmp=keyboard_buf_delete();
		if (mid_tmp==0xfe || mid_tmp==0xff)
			hangup();
		else
			break;
	}
	return mid_tmp;
}
