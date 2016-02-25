#define delay_outb(value,port)  \
	__asm__ ("outb %%al,%%dx\n\t"  \
		"jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:\n\t"  \
		::"a" ((char) (value)),"d" (port))

/*byte_count is 2^n*/
#define mem_copy(src_addr,dst_addr,byte_count)  \
	__asm__ ("push %%es\n\t"  \
		"push %%ds\n\t"  \
		"mov $0x10,%%ax\n\t"  \
		"mov %%ax,%%es\n\t"  \
		"mov %%ax,%%ds\n\t"  \
		"cld\n\t"  \
		"rep\n\t"  \
		"movsl\n\t"  \
		"pop %%ds\n\t"  \
		"pop %%es\n\t"  \
		::"c" (byte_count/4),"D" (dst_addr),"S" (src_addr))

//useage: enable_sb_irq(0x20). IRQ5
#define enable_sb_irq(irq)  \
	__asm__ ("notb %%bl\n\t"  \
		"mov $0x21,%%dx\n\t"  \
		"inb  %%dx,%%al\n\t"  \
		"andb %%bl,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		::"b" (irq))

#define cli()  __asm__ ("cli")
#define sti()  __asm__ ("sti")
/*set:ch0 0x01,ch1 0x02,ch3 0x08*/
#define DMA_CHANNEL_SET 0x02	//set DSP's DMA channel 1
#define DMA_CHANNEL 1		//set DMA controler channel 1
/*byte,addr inc,no-auto-init,write,channel 1*/
#define DMA_COMMAND 0x59
#define DSP_BASE 0x220
/*set:IRQ2 0x01,IRQ5 0x02,IRQ7 0x04,IRQ10 0x08*/
#define DSP_IRQ_SET 0x02	//IRQ5
/*NOTE:our kernel is less than 60k!!!*/
#define DMA_BUF_START 0xf000	//60k
#define DMA_BUF_LENGTH 0x1000	//4k 

/*setup DSP IRQ*/
//useage: setup_IRQ(DSP_BASE,DSP_IRQ_SET)
#define setup_IRQ(base,irq)  \
	__asm__ ("addb $4,%%dl\n\t"  \
		"movb $0x80,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"addb $1,%%dl\n\t"  \
		"movb %%bl,%%al"  \
		"outb %%al,%%dx\n\t"  \
		::"d" (base),"b" (irq))

/*setup DSP's DMA channel*/
//useage: setup_DMA_channel(DSP_BASE,DMA_CHANNEL_SET)
#define setup_DMA_channel(base,channel)  \
	__asm__ ("addb $4,%%dl\n\t"  \
		"movb $0x81,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"addb $1,%%dl\n\t"  \
		"movb %%bl,%%al"  \
		"outb %%al,%%dx\n\t"  \
		::"d" (base),"b" (channel))

#define on_speaker(base)  \
	__asm__ ("addb $0x0c,%%dl\n\t"  \
		"1:\n\t"  \
		"inb %%dx,%%al\n\t"  \
		"orb %%al,%%al\n\t"  \
		"js 1b\n\t"  \
		"movb $0xd1,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:\n\t"  \
		::"d" (base))

//useage:off_speaker(DSP_BASE)
#define off_speaker(base)  \
	__asm__ ("addb $0x0c,%%dl\n\t"  \
		"1:\n\t"  \
		"inb %%dx,%%al\n\t"  \
		"orb %%al,%%al\n\t"  \
		"js 1b\n\t"  \
		"movb $0xd3,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:\n\t"  \
		::"d" (base))

//TimeCon=65536-(256000000/(channels*sampling rate))
//useage:set_DSP_TimeCon(DSP_BASE,0x8300)
//here:mono 8kHZ,TimeCon=0x8300
#define set_DSP_TimeCon(base,TimeCon)  \
	__asm__ ("addb $0x0c,%%dl\n\t"  \
		"1:\n\t"  \
		"inb %%dx,%%al\n\t"  \
		"orb %%al,%%al\n\t"  \
		"js 1b\n\t"  \
		"movb $0x41,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"1:\n\t"  \
		"inb %%dx,%%al\n\t"  \
		"orb %%al,%%al\n\t"  \
		"js 1b\n\t"  \
		"movb %%bl,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"1:\n\t"  \
		"inb %%dx,%%al\n\t"  \
		"orb %%al,%%al\n\t"  \
		"js 1b\n\t"  \
		"movb %%bh,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:\n\t"  \
		::"d" (base),"b" (TimeCon))

//useage:start_DSP_SingleInit(DSP_BASE,DMA_BUF_LENGTH)
//actual num is DMA_BUF_LENGTH-1
#define start_DSP_SingleInit(base,TransBlkSize)	\
	__asm__ ("addb $0x0c,%%dl\n\t"  \
		"1:\n\t"  \
		"inb %%dx,%%al\n\t"  \
		"orb %%al,%%al\n\t"  \
		"js 1b\n\t"  \
		"movb $0x14,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"1:\n\t"  \
		"inb %%dx,%%al\n\t"  \
		"orb %%al,%%al\n\t"  \
		"js 1b\n\t"  \
		"movb %%bl,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"1:\n\t"  \
		"inb %%dx,%%al\n\t"  \
		"orb %%al,%%al\n\t"  \
		"js 1b\n\t"  \
		"movb %%bh,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:\n\t"  \
		 ::"d" (base),"b" (TransBlkSize-1))

//for future extension, use default mixer volume
#define set_mixer_volume(base,MixerRegIndex,MixerRegSetting)  \
	__asm__ ("addb $4,%%dl\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:\n\t"  \
		"addb $1,%%dl\n\t"  \
		"movb %%bl,%%al\n\t"  \
		"outb %%al,%%dx\n\t"  \
		"jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:jmp 1f\n\t"  \
		"1:\n\t"  \
		::"d" (base),"a" (MixerRegIndex),"b" (MixerRegSetting))

//globle varities are sound_data_start, sound_data_end

unsigned char *sound_data_start=(unsigned char *)0x10000;		//sound file lies in memory 64k
unsigned char *sound_data_end=(unsigned char *)0x10000;			//in case before process_sound(), DSP come with a interrupt. if we can mask DSP interrupt, we don't need it anymore. small bug, fix or not!
unsigned char *sound_pointer=(unsigned char *)0x10000;			//tempoary pointer
