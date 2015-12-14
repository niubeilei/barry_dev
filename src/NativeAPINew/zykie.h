/*
 * zykie.h
 *
 *  Created on: 2014-11-26
 *      Author: yangyang
 */

#ifndef ZYKIE_H_
#define ZYKIE_H_


#ifdef	__cplusplus
extern "C" {
#endif


typedef unsigned long long my_ulonglong;
typedef char my_bool;

enum zykie_enum_field_types { ZYKIE_TYPE_DECIMAL, ZYKIE_TYPE_TINY,
			ZYKIE_TYPE_SHORT,  ZYKIE_TYPE_LONG,
			ZYKIE_TYPE_FLOAT,  ZYKIE_TYPE_DOUBLE,
			ZYKIE_TYPE_NULL,   ZYKIE_TYPE_TIMESTAMP,
			ZYKIE_TYPE_LONGLONG,ZYKIE_TYPE_INT24,
			ZYKIE_TYPE_DATE,   ZYKIE_TYPE_TIME,
			ZYKIE_TYPE_DATETIME, ZYKIE_TYPE_YEAR,
			ZYKIE_TYPE_NEWDATE, ZYKIE_TYPE_VARCHAR,
			ZYKIE_TYPE_BIT,
                        ZYKIE_TYPE_NEWDECIMAL=246,
			ZYKIE_TYPE_ENUM=247,
			ZYKIE_TYPE_SET=248,
			ZYKIE_TYPE_TINY_BLOB=249,
			ZYKIE_TYPE_MEDIUM_BLOB=250,
			ZYKIE_TYPE_LONG_BLOB=251,
			ZYKIE_TYPE_BLOB=252,
			ZYKIE_TYPE_VAR_STRING=253,
			ZYKIE_TYPE_STRING=254,
			ZYKIE_TYPE_GEOMETRY=255

};

typedef char **ZYKIE_ROW;		/* return data as array of strings */

typedef struct zykie_field {
  char* name;                 /* Name of column */
  char *org_name;             /* Original column name, if an alias */
  char *table;                /* Table of column if column was a field */
  char *org_table;            /* Org table name, if table was an alias */
  char *db;                   /* Database for table */
  char *catalog;	      /* Catalog for table */
  char *def;                  /* Default value (set by mysql_list_fields) */
  unsigned long length;       /* Width of column (create length) */
  unsigned long max_length;   /* Max width for selected set */
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
  unsigned int flags;         /* Div flags */
  unsigned int decimals;      /* Number of decimals in field */
  unsigned int charsetnr;     /* Character set */
  enum zykie_enum_field_types type; /* Type of field. See mysql_com.h for types */
  void *extension;
} ZYKIE_FIELD;

typedef struct st_zykie
{
  //NET		net;			 Communication parameters
  unsigned char	*connector_fd;		 //ConnectorFd for SSL
  char*		host;
  char *user,*passwd,*unix_socket,*server_version,*host_info;
  char          *info, *db;
  struct charset_info_st *charset;
  ZYKIE_FIELD	*fields;
  //MEM_ROOT	field_alloc;
  my_ulonglong affected_rows;
  my_ulonglong insert_id;		 //id if insert on table with NEXTNR
  my_ulonglong extra_info;		 //Not used
  unsigned long thread_id;		 //Id for connection in server
  unsigned long packet_length;
  unsigned int	port;
  unsigned long client_flag,server_capabilities;
  unsigned int	protocol_version;
  unsigned int	field_count;
  unsigned int 	server_status;
  unsigned int  server_language;
  unsigned int	warning_count;
  //struct st_zykie_options options;
  //enum zykie_status status;
  my_bool	free_me;
  my_bool	reconnect;		 //set to 1 if automatic reconnect

  // session-wide random string
  //char	        scramble[SCRAMBLE_LENGTH+1];
  my_bool unused1;
  void *unused2, *unused3, *unused4, *unused5;

  //LIST  *stmts;                      //list of all statements
  const struct st_zykie_methods *methods;
  void *thd;
    //Points to boolean flag in ZYKIE_RES  or ZYKIE_STMT. We set this flag
    //from ZYKIE_stmt_close if close had to cancel result set of this object.
  my_bool *unbuffered_fetch_owner;
   //needed for embedded server - no net buffer to store the 'info'
  char *info_buffer;
  void *extension;
  char *mContentString;
  void *mcontentNode;
} ZYKIE;


typedef struct st_zykie_res {
  my_ulonglong  row_count;
  my_ulonglong current_row_idx;
  ZYKIE_FIELD	**fields;
  //ZYKIE_DATA	*data;
  //ZYKIE_ROWS	*data_cursor;
  unsigned long *lengths;		/* column lengths of current row */
  ZYKIE		*handle;		/* for unbuffered reads */
  //const struct st_zykie_methods *methods;
  ZYKIE_ROW* row;			/* If unbuffered read */
  ZYKIE_ROW	current_row;		/* buffer to current row */
  //MEM_ROOT	field_alloc;
  unsigned int	field_count, current_field;
  my_bool	eof;			/* Used by ZYKIE_fetch_row */
  /* ZYKIE_stmt_close() had to cancel this result */
  my_bool       unbuffered_fetch_cancelled;
  void *extension;

} ZYKIE_RES;

/*
bool zykie_connect(ZYKIE *zykie, const char *host,
				      const char *user, const char *passwd);
*/

ZYKIE *  zykie_init(ZYKIE *zykie);
my_bool	 zykie_connect(ZYKIE *zykie, const char *host,int port,
				      const char *user, const char *passwd);
my_bool	 zykie_query(ZYKIE *zykie,const char* req);
void zykie_close(ZYKIE *conn);

ZYKIE_ROW zykie_fetch_row(ZYKIE_RES *res);
ZYKIE_RES * zykie_store_result(ZYKIE *zykie);

my_ulonglong zykie_num_rows(ZYKIE_RES *res);
unsigned int zykie_num_fields(ZYKIE_RES *res);

my_ulonglong zykie_affected_rows(ZYKIE *zykie);
unsigned int zykie_errno(ZYKIE *zykie);
const char * zykie_error(ZYKIE *zykie);

void zykie_free_result(ZYKIE_RES *res);


#ifdef	__cplusplus
}
#endif

#endif /* ZYKIE_H_ */
