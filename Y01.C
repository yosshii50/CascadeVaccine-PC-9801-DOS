
#include<io.h>
#include<fcntl.h>

unsigned char check_code1[]={
	0xFA,0x8B,0xEC,0xE8,0x00,0x00,0x5B,0x81,
	0xEB,0x31,0x01,0x2E,0xF6,0x87,0x2A,0x01,
	0x01,0x74,0x0F,0x8D,0xB7,0x4D,0x01,0xBC,
	0x82,0x06,0x31,0x34,0x31,0x24,0x46,0x4C,
	0x75,0xF8};
int check_code2=34;

void main(int argc,char *argv[])
{
	int fil_hdl;
	struct ftime f_time;
	struct{
		unsigned char DMY;
		unsigned int jmpp;
	}buffer1;
	unsigned char buffer2[0x31];
	unsigned int s_eor1;
	unsigned int s_eor2;
	
//	printf("\n９５年５月１０日発見ウイルス用ワクチン／(C)1995 Y_YShikii.\n");
	
	if(argc<2)
	{
		printf("ファイル名を指定して下さい。\n\n");
		exit(1);
	}
	
	printf("[%s] ",argv[1]);
	if((fil_hdl=_open(argv[1],O_RDWR|O_DENYNONE))==-1)
	{
		printf("File Open Error..\n");
		exit(1);
	}
	if(getftime(fil_hdl,&f_time)!=0)
	{
		printf("File Time Get Error..\n");
		exit(1);
	}
	
	if(_read(fil_hdl,&buffer1.DMY,3)==3)
	{
		if(buffer1.DMY==0xE9)
		{
			lseek(fil_hdl,buffer1.jmpp+0x03,SEEK_SET);
			if(_read(fil_hdl,&buffer2,0x33)==0x33)
			{
				if(memcmp(buffer2,check_code1,check_code2)==0)
				{
					printf("\x1B[32m%04X %02X:%02X:%02X \x1B[m",buffer1.jmpp,buffer2[0x2D],buffer2[0x2E],buffer2[0x2F]);
					s_eor1=buffer1.jmpp+0x012F;
					s_eor2=0x0678;
					*(int *)&buffer2[0x2C]=*(int *)&buffer2[0x2C]^s_eor1^s_eor2;
					s_eor1++;
					s_eor2--;
					*(int *)&buffer2[0x2D]=*(int *)&buffer2[0x2D]^s_eor1^s_eor2;
					s_eor1++;
					s_eor2--;
					*(int *)&buffer2[0x2E]=*(int *)&buffer2[0x2E]^s_eor1^s_eor2;
					s_eor1++;
					s_eor2--;
					*(int *)&buffer2[0x2F]=*(int *)&buffer2[0x2F]^s_eor1^s_eor2;
					printf("-> %02X:%02X:%02X Write",buffer2[0x2D],buffer2[0x2E],buffer2[0x2F]);
					lseek(fil_hdl,0,SEEK_SET);
					if(_write(fil_hdl,&buffer2[0x2D],3)==3)
					{
						printf(".. ");
						if(chsize(fil_hdl,buffer1.jmpp+2)==0)
						{
							if(setftime(fil_hdl,&f_time)==0)
							{
								printf("Ok..\n");
							}
						}
						else
						{
							printf("Error..\n");
						}
					}
					else
					{
						printf("-Error\n");
					}
				}
				else
				{
					printf("正常なファイルです。1\n");
				}
			}
			else
			{
				printf("正常なファイルです。2\n");
			}
		}
		else
		{
			printf("正常なファイルです。3\n");
		}
	}
	close(fil_hdl);
	return;
}
