#include <memory.h>
#include <stdio.h>
#include "pro_head.h"
#include "sqlite3.h"
#include "time.h"

#define STORE_LOG_DATABASE		"/usr/local/keyou/Log/hacref.db"
#define STORE_LOG_DB_FMT		"INSERT INTO logindex VALUES('%s','%ld','%s');"

#define MSG_BUFFER_DB_LENGTH	662

#define IS_ENTER_CHAR				0x0d

#define VCHAR_MALLOC(vchar, nlen) \
	vchar.buf=(char *)malloc(nlen);\
	vchar.len=0;

#define VCHAR_FREE(vchar) \
	if( vchar.buf ){\
		free(vchar.buf);\
		vchar.buf=NULL;\
		vchar.len=0;\
	}

#define TERNIMALCMDNUMS			22
#define KEY_ESC					0x1b
#define KEY_TAB					0x09
#define TELNET_KEY_BACKSPACE	0x08
#define SSH_KEY_BACKSPACE		0x7f

typedef struct TernimalCmd2Str
{
	u32		cmdLen;
	char	cmd[8];
	char	cmd2str[14];
}TERNIMALCMD2STR, *LPTERNIMALCMD2STR;

TERNIMALCMD2STR ternimalCmd2Str[TERNIMALCMDNUMS]={
//	{1,{0x08},"[Backspace]"},	
//	{1,{0x09},"[Tab]"},
	{1,{0x1b},"[esc]"},
	{3,{0x1b,0x5b,0x41},"[up]"},
	{3,{0x1b,0x5b,0x42},"[down]"},
	{3,{0x1b,0x5b,0x43},"[right]"},
	{3,{0x1b,0x5b,0x44},"[left]"},
	{4,{0x1b,0x5b,0x36,0x7e},"[PgDn]"},
	{4,{0x1b,0x5b,0x35,0x7e},"[PgUp]"},
	{4,{0x1b,0x5b,0x34,0x7e},"[End]"},
	{4,{0x1b,0x5b,0x31,0x7e},"[Home]"},
	{4,{0x1b,0x5b,0x32,0x7e},"[Insert]"},
	{3,{0x1B,0x4F,0x50},"[F1]"},
	{3,{0x1B,0x4F,0x51},"[F2]"},
	{3,{0x1B,0x4F,0x52},"[F3]"},
	{3,{0x1B,0x4F,0x53},"[F4]"},
	{5,{0x1B,0x5B,0x31,0x35,0x7E},"[F5]"},
	{5,{0x1B,0x5B,0x31,0x37,0x7E},"[F6]"},
	{5,{0x1B,0x5B,0x31,0x38,0x7E},"[F7]"},
	{5,{0x1B,0x5B,0x31,0x39,0x7E},"[F8]"},
	{5,{0x1B,0x5B,0x32,0x30,0x7E},"[F9]"},
	{5,{0x1B,0x5B,0x32,0x31,0x7E},"[F10]"},             
	{5,{0x1B,0x5B,0x32,0x33,0x7E},"[F11]"},
	{5,{0x1B,0x5B,0x32,0x34,0x7E},"[F12]"}
};

int input_is_enter( u8 *msg, u32 *msg_len )
{
	register u32 ilen = 0;
	int is_find = 0;
	u32 tmp_len = *msg_len;
	
	if( tmp_len > 3 )ilen = tmp_len - 3;
	for( ; ilen < tmp_len; ilen++ )
	{
		if( msg[ilen] == '\r' || msg[ilen] == '\n' )
		{
			msg[ilen]='\0';
			*msg_len--;
			is_find = 1;
		}
	}
	return is_find;
}

void log_var_init( VCHAR pro_thread_data )
{
	/*
	if( logVar == NULL )return 0;
	
	VCHAR_MALLOC( (*logVar), MSG_BUFFER_LENGTH );
	if( logVar->buf == NULL )
	{
		free( logVar );
		return NULL;
	}
	*/
	memset( &pro_thread_data, 0, sizeof(VCHAR) );
	//return (void *)logVar;
}

int write_log_to_database( LPVCHAR log_var, struct daemon_fd *mon )
{
    time_t cur_time;
	sqlite3 *log_db=NULL;
    char *zErrMsg = NULL;
	char query_str[MSG_BUFFER_DB_LENGTH];
	//LPVCHAR log_var = (LPVCHAR)mon->pro_thread_data;
	
    if( sqlite3_open( STORE_LOG_DATABASE, &log_db ) )
    {
		printf("Open SQLite3 file '" STORE_LOG_DATABASE "' failure !\nError id : %s\n", sqlite3_errmsg(log_db) );
		return 0;
    }
	cur_time = time(NULL);
    snprintf( query_str, MSG_BUFFER_DB_LENGTH, STORE_LOG_DB_FMT,\
		mon->session_id, cur_time, log_var->buf );
	printf(stderr, "%s\n", query_str );
    sqlite3_exec( log_db , query_str , 0 , 0 , &zErrMsg );
 
    sqlite3_close(log_db);

	return 1;
}

int log_var_add( u8 *data, u32 data_len, struct daemon_fd *mon )
{
	LPVCHAR log_var = (LPVCHAR)&(mon->pro_thread_data);
	register u32 ilen=0;
	
	if( !log_var )return 0;

	if( data[0] == KEY_ESC )
	{
		//if get control key input, you need save prev command firstly.
		if( log_var->len )
		{
			log_var->buf[ log_var->len ] = 0;
			if( !write_log_to_database( log_var, mon ) )
			{
				//write database failure
				//add your code to here
				return 0;
			}
			//memset( log_var->buf, 0, MSG_BUFFER_LENGTH );
			log_var->len  = 0;
		}
		//find that which control key be inputed ?!
		for( ilen =0; ilen<TERNIMALCMDNUMS; ilen++ )
		{
			if( data_len == ternimalCmd2Str[ilen].cmdLen )
			{
				if( !strncmp( data, ternimalCmd2Str[ilen].cmd, data_len ) )
				{
					VCHAR tmp_var;
					strcpy( tmp_var.buf , ternimalCmd2Str[ilen].cmd2str );
					tmp_var.len = strlen( ternimalCmd2Str[ilen].cmd2str );
					//we need stored this control key now !!!
					if( !write_log_to_database( &tmp_var, mon ) )
					{
						//write database failure
						//add your code to here
						return 0;
					}
					return 1;
				}
			}
		}
		return 0;
	}
	
	for( ilen = 0; ilen < data_len; ilen++ )
	{
		switch( data[ilen] )
		{
		//if input is backspace, we need delete most near character.
		case TELNET_KEY_BACKSPACE :
		case SSH_KEY_BACKSPACE : /* ssh procotol use '0x7f' to delete prev char */
			if( log_var->len )
				log_var->len--;
			break;
		case KEY_TAB :
			{
				char *tabKey = "[Tab]";
				VCHAR tmp_var;

				strcpy( tmp_var.buf, tabKey );
				//we need stored this control key now !!!
				if( !write_log_to_database( &tmp_var, mon ) )
				{
					//write database failure
					//add your code to here
					return 0;
				}
			}
			break;
		case '\r' :
		case '\n' :
			if( log_var->len )
			{
				log_var->buf[ log_var->len ] = 0;
				log_var->len  = 0;	//log_var->len isn't used in write_log_to_database
				if( !write_log_to_database( log_var, mon ) )
				{
					//write database failure
					//add your code to here
					return 0;
				}
			}
			return 1;
		default :
			if( data[ilen] == 0x0 )break;
			log_var->buf[ log_var->len++ ] = data[ilen];
			if( log_var->len >= MSG_BUFFER_LENGTH-4 )
			{
				log_var->buf[ log_var->len ] = 0;
				log_var->len  = 0;	//log_var->len isn't used in write_log_to_database
				if( !write_log_to_database( log_var, mon ) )
				{
					//write database failure
					//add your code to here
					return 0;
				}
			}
			break;
		}
	}
	return 1;
}

int log_var_release( VCHAR log_var )
{
	memset( &log_var, 0, sizeof(VCHAR) );
	//if( log_var == NULL )return 0;
	//VCHAR_FREE( (*logVar) );
	//free( log_var );
	return 1;
}
