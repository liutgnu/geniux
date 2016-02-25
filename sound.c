#include "sound.h"

/*
* memory map
* 0    kernel   60k DMA 64K     sound data                    640k 
* |--------------|-------|----------------------|--------------|.....
*
* disk map
* 0         kernel      64K     sound data                    640k 
* |----------------------|----------------------|--------------|.....
*
*/

/*
*DMA buff: 0xf000 -> 0xffff  4K
*/

/*setup DMA controler*/
void setup_DMA()
{
	long addr=DMA_BUF_START;
	cli();
/*mask DMA channel*/
	delay_outb(4 | DMA_CHANNEL,0x0a);
/*perform a write, reset 0x0c, setting 16-bit reg from low to high*/
	delay_outb(DMA_COMMAND,0x0c);
/*output command type*/
	delay_outb(DMA_COMMAND,0x0b);
/*0-7 bit of addr*/
	delay_outb(addr,0x02);
	addr=addr>>8;
/*8-15 bit of addr*/
	delay_outb(addr,0x02);
	addr=addr>>8;
/*16-19 bit of addr*/
	delay_outb(addr,0x83);
/*0-7 bit of count-1*/
	delay_outb(0xff,0x03);
/*8-15 bit of count-1*/
	delay_outb(0x0f,0x03);
/*unmask DMA channel*/
	delay_outb(0xfb & DMA_CHANNEL,0x0a);
	sti();
}

/*
*we should look inside pcm-WAV form. head of WAV varies from each other,
*i can't figure out why. the only thing i know is "after string 'data'(4B), 
*lies sound data length(4B), then followed by real sound data"
*
*so the simplest way is search string 'data', then save it's address
*/

int process_sound()
{
	unsigned char *Bit8;
	unsigned long *Bit32;
	int sound_length;
	Bit8=(unsigned char *)sound_data_start;
	Bit32=(unsigned long *)sound_data_start;
	while (Bit8 + 4 <= sound_data_start + 0x40)	//assume head less than 0x40
	{
		if (*Bit8 == 'd' && *(Bit8 + 1) == 'a' && *(Bit8 + 2) == 't' && *(Bit8 + 3) == 'a')
		{
			Bit32 =(unsigned long *) (Bit8 + 4);
			break;
		}
		else
			Bit8++;
	}
//what about head larger than 0x40? i hope not
/*
*you can see printk is very important, since our kernel is too weak to have a printk,
*else we can just printk "invalid WAV format" and exit. THAT EASY aha?
*/
	if (Bit8 + 4 > sound_data_start + 0x40)
		return 1;

	sound_length=*Bit32;
	sound_data_start=(unsigned char *)(Bit32 + 4);
	sound_pointer=sound_data_start;
	sound_data_end=sound_data_start + sound_length;
	return 0;
}

/*copy sound data block to DMA buf*/

int renew_buf()
{
/*check first*/
	if (sound_data_start==sound_data_end)
		return 2;	//wait! havn't initialized yet!

	if (sound_pointer+DMA_BUF_LENGTH <= sound_data_end)
	{
		mem_copy(sound_pointer,DMA_BUF_START,DMA_BUF_LENGTH);
		sound_pointer=sound_pointer+DMA_BUF_LENGTH;
		return 1;	//data transfer unfinished
	}
	else if (sound_pointer <= sound_data_end)
	{
		mem_copy(sound_pointer,DMA_BUF_START,DMA_BUF_LENGTH);
		sound_pointer=sound_pointer+DMA_BUF_LENGTH;
		return 0;	//data transfer finished	
	}
	else
	{
		return 2;	//sound_pointer invalid
	}
}

/*
*here left a bug!!!
*DMA buf init with 0, no pre-sound data loaded, that would cause a little music break.
*when first DSP interrups, sound data loading, music starts.
*
*to fix this bug, load pre-sound data when init DMA buf
*/

void play_sound()
{
  if (process_sound()!=0)
    return;
  on_speaker(DSP_BASE);
  enable_sb_irq(0x20);  
  setup_DMA();
  set_DSP_TimeCon(DSP_BASE,0x1f10);
  start_DSP_SingleInit(DSP_BASE,DMA_BUF_LENGTH);	//now play 
}
  
/*
void play_sound()
{
	if (process_sound()==0)		//ok
	{
		on_speaker(DSP_BASE);
		setup_DMA();
		set_DSP_TimeCon(DSP_BASE,0x8300);
		set_DSP_TransBlkSize(DSP_BASE,DMA_BUF_LENGTH);
		enable_sb_irq(0x20);
		start_DSP_AutoInit(DSP_BASE);	//now play
	}
	else				//no
		return;
}
*/
