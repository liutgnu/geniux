#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<strings.h>
#define FLOPPYSIZE (1024*1440)

int main(int argc,char **argv)
{
	int fd,size=0,ret=-1;
	struct stat f_stat;
	void *buf;

	printf("Generate geniux bootable disk of standard floppy disk size\n");
	if (argc!=2){
		printf("Useage:floppy_img <path_of_fakedisk>\n");
		goto out;
	}
	if ((fd=open(argv[1],O_RDONLY))<0){
		printf("No fakedisk exist! Maybe forget to run \"make fakedisk\" first?\n");
		goto out;
	}
	fstat(fd,&f_stat);
	size=f_stat.st_size;
	buf=malloc(FLOPPYSIZE);
	if (!buf){
		printf("malloc error!\n");
		goto fail;
	}
	bzero(buf,FLOPPYSIZE);
	read(fd,buf,size);
	if ((fd=open("geniux.img",O_RDWR|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH))<0){
		printf("Create new file error!\n");
		goto fail;
	}
	write(fd,buf,FLOPPYSIZE);
	ret=0;

fail:
	free(buf);
out:
	return ret;
}

