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
// Modification History:
// 02/25/2008 : Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef aos_net_conn_h
#define aos_net_conn_h

#include "net/types.h"
#include "porting_c/mutex.h"
#include "util_c/types.h"
#include "util_c/dyn_array.h"
#include "util_c/gen_ptr.h"

struct aos_conn;
struct aos_action;
struct aos_buffer;

#define MAX_SOCK_TO_SELECT 1023
#define AOS_CONN_MAGIC 260980143
#define AOS_CONN_MAX_USER_DATA 10

typedef int (*aos_conn_read_callback_t)(
		struct aos_conn *conn, 
		struct aos_buffer *buffer, 
		const int is_timedout,
		const u32 remote_addr, 
		const u16 remote_port);

typedef int (*aos_conn_integrity_check_t)( struct aos_conn *conn);
typedef int (*aos_conn_release_memory_t)( struct aos_conn *conn);
typedef int (*aos_conn_hold_t)( struct aos_conn *conn);
typedef int (*aos_conn_put_t)( struct aos_conn *conn);
typedef int (*aos_conn_connect_t)( struct aos_conn *conn);
typedef int (*aos_conn_disconnect_t)( struct aos_conn *conn);
typedef int (*aos_conn_destroy_t)( struct aos_conn *conn);
typedef int (*aos_conn_is_conn_good_t)( struct aos_conn *conn);
typedef char *(*aos_conn_dump_t)( struct aos_conn *conn);

typedef int (*aos_conn_read_t)(
		struct aos_conn *conn, 
		const int sec, 
		const int usec, 
		aos_conn_read_callback_t callback,
		int *is_eof, 
		int *is_conn_broken);

typedef int (*aos_conn_read_to_buff_t)(
		struct aos_conn *conn, 
		const int sec, 
		const int usec, 
		struct aos_buffer *buff, 
		u32 *remote_addr, 
		u16 *remote_port,
		int *is_eof, 
		int *is_conn_broken);

typedef int (*aos_conn_add_user_data_t)(
		struct aos_conn *conn, 
		aos_gen_ptr_tag_e tag, 
		void *data);

typedef int (*aos_conn_del_user_data_t)(
		struct aos_conn *conn, 
		aos_gen_ptr_tag_e tag, 
		void *data);

typedef int (*aos_conn_get_user_data_t)(
		struct aos_conn *conn, 
		aos_gen_ptr_tag_e tag, 
		void **data);

#define AOS_CONN_MEMFUNC_DECL						\
	aos_conn_add_user_data_t		add_user_data;	\
	aos_conn_del_user_data_t		del_user_data;	\
	aos_conn_get_user_data_t		get_user_data;	\
	aos_conn_hold_t					hold;			\
	aos_conn_put_t					put;			\
	aos_conn_release_memory_t		release_memory;	\
	aos_conn_read_t					read;			\
	aos_conn_read_to_buff_t			read_to_buff;	\
	aos_conn_is_conn_good_t			is_conn_good;	\
	aos_conn_connect_t				connect;		\
	aos_conn_disconnect_t			disconnect;		\
	aos_conn_integrity_check_t		integrity_check;\
	aos_conn_destroy_t				destroy;		\
	aos_conn_dump_t					dump



#define AOS_CONN_MEMDATA_DECL						\
	int					sock;						\
	aos_sock_type_e		type;						\
	int					ref_count;					\
	int					is_destroying;				\
	u32					local_addr;					\
	u16					local_port;					\
	int					local_num_ports;			\
	int					magic;						\
	u32					remote_addr;				\
	u16					remote_port;				\
	int					remote_num_ports;			\
	u16					local_used_port;			\
	u16					remote_used_port;			\
	aos_conn_read_callback_t callback;				\
	aos_gen_ptr_t *		user_data;					\
	aos_dyn_array_t		user_data_array

typedef struct aos_conn_mf
{
	AOS_CONN_MEMFUNC_DECL;
} aos_conn_mf_t;

typedef struct aos_conn
{
	aos_conn_mf_t *mf;

	AOS_CONN_MEMDATA_DECL;
} aos_conn_t;


#ifdef __cplusplus
extern "C" {
#endif

extern int aos_conn_init(aos_conn_t *conn);
extern int aos_conn_integrity_check( struct aos_conn *conn);
extern int aos_conn_hold( struct aos_conn *conn);
extern int aos_conn_put( struct aos_conn *conn);
extern int aos_conn_disconnect( struct aos_conn *conn);
extern int aos_conn_destroy( struct aos_conn *conn);
extern int aos_conn_is_conn_good( struct aos_conn *conn);
extern int aos_conn_release_memory( struct aos_conn *conn);

extern int aos_conn_add_user_data(
		struct aos_conn *conn, 
		aos_gen_ptr_tag_e tag, 
		void *data);

extern int aos_conn_del_user_data(
		struct aos_conn *conn, 
		aos_gen_ptr_tag_e tag, 
		void *data);

extern int aos_conn_get_user_data(
		struct aos_conn *conn, 
		aos_gen_ptr_tag_e tag, 
		void **data);

extern int aos_conn_read_from(
		const int sock, 
		struct aos_buffer *buffer,
		const int timer_sec, 
		const int timer_usec,
		int *is_timedout, 
		u32 *remote_addr, 
		u16 *remote_port, 
		int *is_conn_broken);

#ifdef __cplusplus
}
#endif


#endif


