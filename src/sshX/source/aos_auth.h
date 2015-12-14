///////////////////////////////////////////////////////////////////////
// Author: Pownall.Zhang
// Creation Time: 10/27/2006
// Description:
// * File only for windows sshX program test using.
//
// Change History
// 10/27/2006 aosWinLogFunc.h created
///////////////////////////////////////////////////////////////////////
#ifndef AOS_AUTH_H
#define AOS_AUTH_H

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include <winsock2.h>
#include <network.h>

#ifdef _WIN32
#else
#include <stdint.h>
#endif

//Which direct packet from net ,Which telnet write packet to binary file
#define CLIENTTOSERVER		0x0
#define SERVERTOCLIENT		0x1

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned long		u32;

#ifdef _WIN32
typedef __int64				u64;
#else
typedef uint64_t			u64;
#endif

//通信状态
#define SSH_START			0xA0
#define SSH_AUTH			0xA1
#define SSH_NORMAL			0xA2
#define SSH_END				0xA3

//版本号
#define SSH_CLIENT_VER		0x10
//客户端标识符
#define SSH_CLIENT_ID		0x305E4E1B

#define SSH_AUTH_ALLOW		0xff
#define SSH_AUTH_DENY		0xf0

//HAC认证缓存长度
#define AUTH_INPUT_BUF_LEN	256
//重试次数为4次
#define AUTH_INPUT_MAX_NUM	3

extern u8	ssh_auth_status;
extern int	ssh_auth_number;

#define SET_SSH_AUTH_START()	ssh_auth_status = SSH_START
#define SET_SSH_AUTH_NORMAL()	ssh_auth_status = SSH_NORMAL
#define SET_SSH_AUTH_PASS()		ssh_auth_status = SSH_AUTH
#define SET_SSH_AUTH_FAIL()		ssh_auth_status = SSH_START

#define SSH_AUTH_PROCESS()		ssh_auth_status == SSH_START
#define SSH_AUTH_PASS()			ssh_auth_status == SSH_AUTH
#define SSH_AUTH_NORMAL()		ssh_auth_status == SSH_NORMAL

#define INC_SSH_AUTH_NUMBER()	ssh_auth_number++
#define SSH_AUTH_NUMBER()		ssh_auth_number

typedef struct SSH_Proxy_Header
{
	u32	version;
	u32	message_id;
	u32 length;
}HDR,*LPHDR;

typedef struct SSH_Save_Log
{
	u32 log_length;
	u8	*log;
}SSHSAVELOG,*LPSSHSAVELOG;

#define FREE_SAVE_LOG( saveLog )\
	saveLog.log_length=0;\
	free( saveLog.log );

extern u64			ssh_session_id;
extern u8			ssh_save_log_num;
extern SSHSAVELOG	ssh_save_log[32];

extern SOCKET get_log_socket(Socket s);
extern int aos_build_log_auth_data(char *input);
extern int aos_build_auth_data(char *);
extern int aos_verify_auth(void *receive,int len);
extern int aos_build_log_data(u8 *body, u32 inlen, u8 *out, u32 *outlen, u8 direct);

#endif
