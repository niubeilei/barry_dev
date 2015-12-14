#include "pro_head.h"
#include <string.h>

#define ABOR	 0
#define CWD		 1
#define DELE	 2
#define LIST	 3
#define MDTM	 4
#define MKD		 5
#define NLST	 6
#define PASS	 7
#define PASV	 8
#define PORT	 9
#define PWD		10
#define QUIT	11
#define RETR	12
#define RMD		13
#define RNFR	14
#define RNTO	15
#define SITE	16
#define SIZE	17
#define STOR	18
#define BINARY	19
#define ASCII	32
#define USER	20
#define ACCT	21
#define APPE	22
#define CDUP	23
#define HELP	24
#define MODE	25
#define NOOP	26
#define REIN	27
#define STAT	28
#define STOU	29
#define STRU 	30
#define SYST 	31

#define FTPCMDNUMBER	33

typedef struct FtpCmd2Str
{
	int		index;
	char	command[8];
	char	cmdstr[12];
}FTPCMD2STR, *LPFTPCMD2STR;

FTPCMD2STR ftpCmd2Str[FTPCMDNUMBER]={
	{ABOR,"ABOR","abort"},
	{CWD ,"CWD" ,"cd"}, 
	{DELE,"DELE","rm"}, 
	{LIST,"LIST","ls"}, 
	{MDTM,"MDTM","MDTM"}, 
	{MKD ,"MKD" ,"mkdir"}, 
	{NLST,"NLST","nlist"}, 
	{PASS,"PASS","password"}, 
	{PASV,"PASV","passive"}, 
	{PORT,"PORT","PORT"}, 
	{PWD ,"PWD" ,"pwd"}, 
	{QUIT,"QUIT","bye"}, 
	{RETR,"RETR","get"},
	{RMD ,"RMD" ,"rmdir"}, 
	{RNFR,"RNFR","rename"},
	{RNTO,"RNTO","rename to"}, 
	{SITE,"SITE","site"}, 
	{SIZE,"SIZE","size"}, 
	{STOR,"STOR","put"}, 
	{BINARY,"TYPE I","binary"},
	{ASCII,"TYPE A","ascii"}, 
	{USER,"USER","name"}, 
	{ACCT,"ACCT","account"}, 
	{APPE,"APPE","append"}, 
	{CDUP,"CDUP","cdup"}, 
	{HELP,"HELP","?"}, 
	{MODE,"MODE","mode"}, 
	{NOOP,"NOOP","noop"},		//Does nothing except return a response. 
	{REIN,"REIN","REIN"}, 
	{STAT,"STAT","status"}, 
	{STOU,"STOU","put"}, 
	{STRU,"STRU","STRU"}, 
	{SYST,"SYST","system"}
};

u32 remove_return( u8 *msg, u32 msg_len )
{
	register u32 ilen = 0;
	
	for( ilen = 0 ; ilen < msg_len; ilen++ )
	{
		if( msg[ilen] == '\r' || msg[ilen] == '\n' )
		{
			msg[ilen]='\0';
			break;
		}
	}
	return ilen;
}

//process client to server data.
static int pro_ftp_client_to_server(u8 *data, u32 *datalen, struct daemon_fd *mon )
{
	int i,sPos = 0, cmdLen=0;
	char cs_buf[256];
	VCHAR log_buf;
	
	while( data[ sPos ]==0x20 )sPos++;
	
	for( i=0; i<FTPCMDNUMBER; i++ )
	{
		char *ptr = (char *)data+sPos;
		cmdLen = strlen( ftpCmd2Str[i].command );
		if( !strncmp( ptr, ftpCmd2Str[i].command, cmdLen ) )
		{
			//add fmt code to here !
			ptr += cmdLen;
			while( *ptr==0x20 )ptr++;
			if( remove_return( ptr, strlen(ptr) ) )
				log_buf.len = snprintf( cs_buf, 256, "%s %s", ftpCmd2Str[i].cmdstr,  ptr );
			else log_buf.len = strcpy( cs_buf,  ftpCmd2Str[i].cmdstr );
			strcpy( log_buf.buf, cs_buf );
			write_log_to_database( &log_buf, mon );
			//printf("%s\n", cs_buf);
			break;
		}
	}
	return 1;
}

//process server to client data.
static int pro_ftp_server_to_client(u8 *data, u32 *datalen, struct daemon_fd *mon )
{
	//add your code to here
	return 1;
}

//destory function when end connection
static void pro_ftp_destory( struct daemon_fd *mon )
{
	//add your code to here
}

struct tcp_pro_data ftp_pro_data ={
	3,		//0x11
	"ftp",
	pro_ftp_client_to_server,
	pro_ftp_server_to_client,
	pro_ftp_destory
};
