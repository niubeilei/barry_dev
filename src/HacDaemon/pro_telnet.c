#include "pro_head.h"

//process client to server data.
static int pro_telnet_client_to_server(u8 *data, u32 *datalen, struct daemon_fd *mon )
{
	//if( mon->pro_thread_data == NULL )
		//mon->pro_thread_data = log_var_init();
	log_var_add( data, *datalen, mon );
	//printf( "%s", (char *)data );
    return 1;
}

//process server to client data.
static int pro_telnet_server_to_client(u8 *data, u32 *datalen, struct daemon_fd *mon )
{
	//add your code to here
	return 1;
}

//destory function when end connection
static void pro_telnet_destory( struct daemon_fd *mon )
{
	//add your code to here
	log_var_release( mon->pro_thread_data );
}

struct tcp_pro_data telnet_pro_data ={
	2,		//0x10
	"telnet",
	pro_telnet_client_to_server,
	pro_telnet_server_to_client,
	pro_telnet_destory
};
