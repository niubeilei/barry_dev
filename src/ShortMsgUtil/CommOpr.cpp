////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 01/03/2010: Copy by Brian 
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgUtil/CommOpr.h"

#include <stdio.h>  
#include <unistd.h>
#include <fcntl.h>  
#include <termios.h>  
#include <sys/ioctl.h> 
#include <string.h> 
#include <iostream> 
using namespace std;


int OpenComm(int iCommPort) //打开串口设备文件,iCommPort串口端口号,返回句柄  
{  
    int hComm;  
    char szCommFileName[127];  
    sprintf(szCommFileName,"/dev/ttyS%d",iCommPort);
    hComm=open(szCommFileName,O_RDWR|O_NONBLOCK);//加上O_NONBLOCK:非块模式(专用于命名管道),O_NDELAY:非阻塞模式
    hComm=open(szCommFileName,O_RDWR);//加上O_NONBLOCK:非块模式(专用于命名管道),O_NDELAY:非阻塞模式
	if(hComm != -1)
	{
	    printf("串口打开成功！\n");
	}
	//在使实现异步访问方式前,安装信号处理函数  
	//saio.sa_handler = signal_handler_IO;  
    //saio.sa_mask = 0;  
    //saio.sa_flags = 0;  
    //saio.sa_restorer = NULL;  
    //sigaction(SIGIO,&saio,NULL);  
 
    //允许进程去接收SIGIO 信号  
    //fcntl(hComm, F_SETOWN, getpid());  
 
    //允许串口设备为异步访问  
    //fcntl(hComm, F_SETFL, FASYNC);  
 
    return hComm;  
}  
 
void CloseComm(int hComm) //关闭设备文件  
{  
    tcflush(hComm,TCIOFLUSH); //清空输入输出数据线  
    usleep(100000); //延时100毫秒  
    close(hComm);  
}  
 

void SetCommParam(int hComm,int BaudRate,unsigned char ParityBits,int DataBits,int StopBits)//参数设置,包括:波特率,校验位,停止位,数据位  
{  
    int i;  
    struct termios options;  
    int speed_value[]={B115200,B38400,B19200,B9600,B4800};  
    int speed_name[]={115200,38400,19200,9600,4800};  
    tcflush(hComm,TCIOFLUSH);  
 
    tcgetattr(hComm,&options);  
//  bzero(&options,sizeof(struct termios));  
 
    options.c_cflag=(CLOCAL|CREAD|CRTSCTS); //Control options  
    options.c_cflag&=~CSIZE; //Control options  
    options.c_lflag=ICANON; //Local options  
    //options.c_oflag&=~OPOST; //Output options  
    options.c_oflag=0; //Output options,Raw模式  
    options.c_iflag=(IGNPAR|ICRNL); //Input options  
    options.c_cc[VTIME]=0;  
    options.c_cc[VMIN]=1;  
    options.c_cc[VEOF]=4;  
//  options.c_iflag&=~(IXON|IXOFF|IXANY); //Input options  
 
    /*以下为Control characters 
    options.c_cc[VINTR]=0;//Ctrl-c 
    options.c_cc[VQUIT]=0;//Ctrl- 
    options.c_cc[VERASE]=0;//del 
    options.c_cc[VKILL]=0;//@  
    options.c_cc[VEOF]=4;//Ctrl-d 
    options.c_cc[VTIME]=0;//不使用分割字元组的计时器 
    options.c_cc[VMIN]=1;//在读取到 1 个字元前先停止 
    options.c_cc[VSWTC]=0;//'\0' 
    options.c_cc[VSTART]=0; //Ctrl-q 
    options.c_cc[VSTOP]=0; //Ctrl-s 
    options.c_cc[VSUSP]=26; //Ctrl-z 
    options.c_cc[VEOL]=0; //'\0' 
    options.c_cc[VREPRINT]=0; //Ctrl-r 
    options.c_cc[VDISCARD]=0; //Ctrl-u 
    options.c_cc[VWERASE]=0; //Ctrl-w 
    options.c_cc[VLNEXT]=0; //Ctrl-v 
    options.c_cc[VEOL2]=0; //'\0'*/ 
 
    //设置波特率  
    for(i=0;i<(signed int)sizeof(speed_name)/(signed int)sizeof(int);i++)  
    {  
        if(BaudRate==speed_name[i])  
        {  
            options.c_cflag|=speed_value[i];  
            break;  
        }  
    }  
 
    //设置校验位  
    switch(ParityBits)  
    {  
        case 'o':case 'O': //奇校验  
            options.c_cflag|=(PARODD|PARENB);  
            options.c_iflag|=INPCK;  
            break;  
        case 'e':case 'E': //偶校验  
            options.c_cflag|=PARENB;  
            options.c_cflag&=~PARODD;  
            options.c_iflag|=INPCK;  
            break;  
        case 's':case 'S': //Space校验  
            options.c_cflag&=~PARENB;  
            options.c_cflag&=~CSTOPB;  
            options.c_iflag|=INPCK;  
            break;  
        default: //默认为无校验,即'n'或'N'  
            options.c_cflag&=~PARENB;  
            options.c_iflag&=~INPCK;  
    }  
 
    //设置数据位  
    if(DataBits==7)  
        options.c_cflag|=CS7;  
    else 
        options.c_cflag|=CS8;  
 
    //设置停止位  
    if(StopBits==1)  
        options.c_cflag&=~CSTOPB;  
    else 
        options.c_cflag|=CSTOPB;  
 
    //清空数据线,并启用新的设置  
    tcflush(hComm,TCIFLUSH);  
    tcsetattr(hComm,TCSANOW,&options);  
 
}  
 
int WriteComm(int hComm,const char * const pBuff) //写串口  
{  
    int iBytes=0;  
    iBytes = write(hComm, pBuff, strlen(pBuff)); 
	cout << "wirte: " <<  pBuff << endl;
    return(iBytes);  
}  
 
char *ReadComm(int hComm, char* buff, const int size) //以同步方式读串口 
{  
    int iBytes;  
	int i;
	for (i=0; i<size; i++)
	{
		iBytes = read(hComm, (void*)&buff[i], 1);
		if (iBytes == -1)
		{
			perror("Failed to read!");
			return NULL;
		}
		if (buff[i] == '\n')
		{
			buff[i+1] = 0;
			break;
		}
	}
	cout << "Read: " << buff << endl; 
    return(buff);
}  
/* 
void signal_handler_IO (int status)
{
	printf("received SIGIO signal.\n");
	wait_flag = FALSE;
}
*/

