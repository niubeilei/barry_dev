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
#ifndef Aos_ShortMsgUtil_CommOpr_h  
#define Aos_ShortMsgUtil_CommOpr_h
 
#define _POSIX_SOURCE 1 //POSIX 系统相容  
#define TRUE 1
#define FALSE 0

void 	signal_handler_IO (int status);   /* definition of signal handler */
int		OpenComm(int iCommPort); //打开串口设备文件,iCommPort串口端口号,返回句柄  
void 	CloseComm(int hComm); //关闭设备文件  
void 	SetCommParam(int hComm,int BaudRate,unsigned char ParityBits,int DataBits,int StopBits); //参数设置,包括:波特率,校验位,停止位,数据位  
int 	WriteComm(int hComm, const char * const pBuff); //写串口  
char *ReadComm(int hComm, char* buff, const int size); //以同步方式读串口  
#endif 
