#include "tsk.h"
#include "varible.h"
//don't change include order!!!

extern unsigned char keyboard_buf_insert();

/*
*we don't need unchar ascii, so i leave them to 0x00.
*keyboard function is limitted, i would modify it later
*/

char key_map[]={
	"\x00" "\x00"				/* s0-esc */
	"1234567890-="
	"\x00" "\x00"				/* bs-tab */
	"qwertyuiop[]"
	"\x00" "\x00"				/* enter-ctrl */
	"asdfghjkl;\'"
	"`" "\x00"				/* para-lshift */
	"\\zxcvbnm,./"
	"\x00" "\x00" "\x00" "\x20"		/* 36-space */
	"\x00" "\x00" "\x00" "\x00"		/* 3a-3d */
	"\x00" "\x00" "\x00" "\x00"		/* 3e-41 */
	"\x00" "\x00" "\x00" "\x00"		/* 42-45 */
	"\x00" "\x00" "\x00" "\x00"		/* 46-49 */
	"\x00" "\x00" "\x00" "\x00"		/* 4a-4d */
	"\x00" "\x00" "\x00" "\x00"		/* 4e-51 */
	"\x00" "\x00" "\x00" "\x00"		/* 52-55 */
	"\x00" "\x00" "\x00" "\x00"		/* 56-59 */
	"\x00" "\x00" "\x00" "\x00"		/* 5a-5d */
	"\x00" "\x00" "\x00" 			/* 5e-60 */
};

char shift_map[]={
	"\x00" "\x00"				/* s0-esc */
	"!@#$%^&*()_+"
	"\x00" "\x00"				/* bs-tab */
	"QWERTYUIOP{}"
	"\x00" "\x00"				/* enter-ctrl */
	"ASDFGHJKL:\""
	"~" "\x00"				/* para-lshift */
	"|ZXCVBNM<>\?"
	"\x00" "\x00" "\x00" "\x20"		/* 36-space */
	"\x00" "\x00" "\x00" "\x00"		/* 3a-3d */
	"\x00" "\x00" "\x00" "\x00"		/* 3e-41 */
	"\x00" "\x00" "\x00" "\x00"		/* 42-45 */
	"\x00" "\x00" "\x00" "\x00"		/* 46-49 */
	"\x00" "\x00" "\x00" "\x00"		/* 4a-4d */
	"\x00" "\x00" "\x00" "\x00"		/* 4e-51 */
	"\x00" "\x00" "\x00" "\x00"		/* 52-55 */
	"\x00" "\x00" "\x00" "\x00"		/* 56-59 */
	"\x00" "\x00" "\x00" "\x00"		/* 5a-5d */
	"\x00" "\x00" "\x00" 			/* 5e-60 */
};

void do_self(int sc)  //inspired by linux-0.96, though easy, but i thought for a while.
{
	char ch;
	if (mode==0) ch=key_map[sc];
	else if (mode==1) ch=shift_map[sc];
/*else return;  //for future extension   */
	if (keyboard_buf_insert(ch)!=0) return;  //i will not handle error right now
}

void shift(int sc)  //useless parameter, just to be unified
{
	if (mode==1) return;
	else mode=1;
}

void unshift(int sc)
{
	if (mode==0) return;
	else mode=0;
}

void none(int sc)
{
	return;
}

f_void key_table[]={
/*we shoule not deal with control characters.those should be none*/
none,do_self,do_self,do_self,    //00-03
do_self,do_self,do_self,do_self, //04-07
do_self,do_self,do_self,do_self, //08-0b
do_self,do_self,do_self,do_self, //0c-0f
do_self,do_self,do_self,do_self, //10-13
do_self,do_self,do_self,do_self, //14-17
do_self,do_self,do_self,do_self, //18-1b
do_self,none,do_self,do_self,    //1c-1f
do_self,do_self,do_self,do_self, //20-23
do_self,do_self,do_self,do_self, //24-27
do_self,do_self,shift,do_self,   //28-2b
do_self,do_self,do_self,do_self, //2c-2f
do_self,do_self,do_self,do_self, //30-33
do_self,none,shift,do_self,      //34-37
none,do_self,none,none,          //38-3b
none,none,none,none,             //3c-3f
none,none,none,none,             //40-43
none,none,none,none,             //44-47
none,none,do_self,none,          //48-4b
none,none,do_self,none,          //4c-4f
none,none,none,none,             //50-53  
none,none,do_self,none,          //54-57
none,none,none,none,             //58-5b
none,none,none,none,             //5c-5f
none,none,none,none,             //60-63
none,none,none,none,             //64-67
none,none,none,none,             //68-6b
none,none,none,none,             //6c-6f
none,none,none,none,             //70-73
none,none,none,none,             //74-77
none,none,none,none,             //78-7b
none,none,none,none,             //7c-7f
none,none,none,none,             //80-83
none,none,none,none,             //84-87
none,none,none,none,             //88-8b
none,none,none,none,             //8c-8f
none,none,none,none,             //90-93
none,none,none,none,             //94-97
none,none,none,none,             //98-9b
none,none,none,none,             //9c-9f
none,none,none,none,             //a0-a3
none,none,none,none,             //a4-a7
none,none,unshift,none,          //a8-ab
none,none,none,none,             //ac-af
none,none,none,none,             //b0-b3
none,none,unshift,none,          //b4-b7
none,none,none,none,             //b8-bb
none,none,none,none,             //bc-bf
none,none,none,none,             //c0-c3
none,none,none,none,             //c4-c7
none,none,none,none,             //c8-cb
none,none,none,none,             //cc-cf
none,none,none,none,             //d0-d3
none,none,none,none,             //d4-d7
none,none,none,none,             //d8-db
none,none,none,none,             //dc-df
none,none,none,none,             //e0-e3
none,none,none,none,             //e4-e7
none,none,none,none,             //e8-eb
none,none,none,none,             //ec-ef
none,none,none,none,             //f0-f3
none,none,none,none,             //f4-f7
none,none,none,none,             //f8-fb
none,none,none,none,             //fc-ff
};
