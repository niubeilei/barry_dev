#include "pro_head.h"

//process client to server data.
static int pro_ssh_client_to_server(u8 *data, u32 *datalen, struct daemon_fd *mon )
{
	u32 msgLen = 0;
	
	if( *datalen <= SSH_PREFIX_LENGTH )return 0;

	msgLen = *datalen-SSH_PREFIX_LENGTH;
	//printf("[%d]%c\n", msgLen, *(data+SSH_PREFIX_LENGTH)  );
	//if( mon->pro_thread_data == NULL )
	//	mon->pro_thread_data = log_var_init( );
	//printf("[%d]%c\n", msgLen, *(data+SSH_PREFIX_LENGTH)  );
	log_var_add( data+SSH_PREFIX_LENGTH, msgLen, mon );
	
	return 1;
}

//process server to client data.
static int pro_ssh_server_to_client(u8 *data, u32 *datalen, struct daemon_fd *mon )
{
	//add your code to here
	return 1;
}

//destory function when end connection
static void pro_ssh_destory( struct daemon_fd *mon )
{
	//add your code to here
	log_var_release( mon->pro_thread_data );
}

struct tcp_pro_data ssh_pro_data ={
	2,		//0x10
	"ssh",
	pro_ssh_client_to_server,
	pro_ssh_server_to_client,
	pro_ssh_destory
};
