///////////////////////////////////////////////////////////////////////
// Author: Pownall.Zhang
// Creation Time: 10/27/2006
// Description:
// * File only for windows sshX program test using.
//
// Change History
// 10/27/2006 aosWinLogFunc.c created
///////////////////////////////////////////////////////////////////////

#include "aos_auth.h"
#include "putty.h"
#include "terminal.h"
#include "storage.h"
#include "win_res.h"
#include "ssh.h"

#include <imm.h>
#include <commctrl.h>
#include <richedit.h>
#include <mmsystem.h>

struct LZ77Context *ssh_compress_ctx=NULL;

u8			ssh_save_log_num=0;
SSHSAVELOG	ssh_save_log[32];

u8  ssh_auth_status	= 0;
int ssh_auth_number	= 0;

u64	ssh_session_id   = 0l;

#define SHA_HASH_LENGTH		20

#define INIT_SSH_HEADER(header,scv,id,len) \
	header->version = scv;\
    header->message_id  = id;\
	header->length	= len;

int aos_build_auth_data(char *input)
{
	char	uhash[SHA_HASH_LENGTH],phash[SHA_HASH_LENGTH];
	char	*pswd = input+AUTH_INPUT_BUF_LEN/2;

	SHA_Simple(input, strlen(input), (unsigned char *)uhash);
	SHA_Simple(pswd, strlen(pswd), (unsigned char *)phash);
	
	//build auth packet
	INIT_SSH_HEADER( ((LPHDR)input), SSH_CLIENT_VER,\
		SSH_CLIENT_ID, SHA_HASH_LENGTH*2 );
	memcpy(input+sizeof(HDR), uhash, SHA_HASH_LENGTH);
	memcpy(input+sizeof(HDR)+SHA_HASH_LENGTH, phash, SHA_HASH_LENGTH);
	
	//return packet length
	return sizeof(HDR)+SHA_HASH_LENGTH*2; /* user and password hash value length is 40 */
}

int aos_build_log_auth_data(char *input)
{
	//build log auth packet
	INIT_SSH_HEADER( ((LPHDR)input), SSH_CLIENT_VER,\
		SSH_CLIENT_ID, sizeof(HDR) );

	INIT_SSH_HEADER( ((LPHDR)(input+sizeof(HDR))), SSH_CLIENT_ID,\
		(u32)(ssh_session_id>>32), (u32)ssh_session_id );

	return sizeof(HDR)*2;
}

int aos_verify_auth(void *receive,int len)
{
	LPHDR header=(LPHDR)receive;
	u8	auth_result;
	u8  *data=(u8 *)receive;
	u64	*sessiond_id;

	if( header->message_id != SSH_CLIENT_ID /*invaild ID*/||\
		header->version != SSH_CLIENT_VER /*invaild version*/)
	{ // error occur
		//SET_SSH_AUTH_INVAILD();
		return -2;
	}

	INC_SSH_AUTH_NUMBER();

	auth_result = (u8)*(data+sizeof(HDR));

	if( auth_result == SSH_AUTH_ALLOW )
	{
		sessiond_id = (u64 *)(data+sizeof(HDR)+sizeof(u8));
		ssh_session_id = *sessiond_id;
		SET_SSH_AUTH_PASS();
		return 0;
	}
	SET_SSH_AUTH_FAIL();
	return -1;
}

int aos_build_log_data(u8 *body, u32 inlen, u8 *out, u32 *outlen, u8 direct)
{
	int i=0, comLen;
	u8  *randValuePtr, randDirect;
	u8	*compress;
	u32 randValue;

	//build log packet
	// == compress ==
	//if( ssh_compress_ctx == NULL )
	//	ssh_compress_ctx = zlib_compress_init();
	//zlib_compress_block( ssh_compress_ctx, body, inlen,\
	//	&compress, &comLen );
	comLen = inlen;
	compress = body;
	memcpy( out+sizeof(HDR), compress, comLen );
	*outlen = comLen+sizeof(HDR);
	// ==============
	randValuePtr = (u8 *)(&randValue);
	for( i=0; i<4; i++ )
		randValuePtr[i] = (u8)(random_byte()&0xff);
	randDirect = (u8)((random_byte()&0xfe)^direct);

	randValue = ( randValue&0xffff0000 )^( comLen&0xffff );
	INIT_SSH_HEADER( ((LPHDR)out), randDirect,\
		randValue^SSH_CLIENT_ID, randValue );
	// ==============
	//sfree( compress );
	return 0;
}