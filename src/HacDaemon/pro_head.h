#include "stdio.h"
#include "module.h"

typedef unsigned char		u8;
typedef unsigned short 		u16;

#define SSH_PREFIX_LENGTH		8

#define DM_REP_SEQ 4

#define MSG_BUFFER_LENGTH		512

typedef struct v_char
{
	char 	buf[MSG_BUFFER_LENGTH];
	u16		len;
}VCHAR, *LPVCHAR;

struct daemon_fd
{
    int flag;
    char *session_id;
    char *proto;
    int fd_send[DM_REP_SEQ];
    struct tcp_pro_data *pro_data;
    VCHAR pro_thread_data;		//ervery thread use data point, init value is NULL !
};

struct tcp_pro_data
{
	int flags;	//if flags = 0x00, don't send msg to replay module.
			//if flags = 0x01, send client to server msg to replay module.
			//if flags = 0x10, send server to client.
			//if flags = 0x11, send both to replay module.
	char *name;
	//process client to server data.
	int(*client_to_server)(u8 *data, u32 *datalen, struct daemon_fd *mon);
	//process server to client data.
	int(*server_to_client)(u8 *data, u32 *datalen, struct daemon_fd *mon);
	//destory function when end connection
	void(*pro_destory)( struct daemon_fd *mon );
};

extern struct tcp_pro_data ftp_pro_data;
extern struct tcp_pro_data telnet_pro_data;
extern struct tcp_pro_data ssh_pro_data;

extern void log_var_init( VCHAR pro_thread_data );
extern int write_log_to_database( LPVCHAR log_var, struct daemon_fd *mon );
extern int log_var_add( u8 *data, u32 data_len, struct daemon_fd *mon );
extern int log_var_release( VCHAR log_var );
