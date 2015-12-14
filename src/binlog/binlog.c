////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sched_rr.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h> //for copy_from_user()
#include <linux/wait.h>

#define BINLOG_BIT 4
#define BINLOG_LEN (1<<BINLOG_BIT)

/* binlog flags */
#define BINLOG_NONBLOCK 1


static char binlog_buf[BINLOG_LEN];
static unsigned int binlog_rp;
static unsigned int binlog_wp;
static char * binlog_bufend = binlog_buf+BINLOG_LEN;
static DECLARE_MUTEX(binlog_mutex);
static DECLARE_WAIT_QUEUE_HEAD(binlog_waitqueue);

#define BINLOG_SPACE (BINLOG_LEN-1)
#define BINLOG_MASK BINLOG_SPACE
#define BINLOG_USEDSPACE ((binlog_wp-binlog_rp)&(BINLOG_MASK))
#define BINLOG_FREESPACE (BINLOG_SPACE-BINLOG_USEDSPACE)

#define BINLOG_PINC(p,i) p=(BINLOG_MASK)&((p)+(i))
static inline void loop_memcpy( char * dest, char * loop_start, char * loop_end, char __user * orig, int len)
{
	int left;

	left = loop_end - dest;
	if( left >= len )
	{
		memcpy( dest, orig, len );
		
	}
	else
	{
		memcpy( dest, orig, left );
		memcpy( loop_start, orig + left, len - left );
	}

}

static inline void loop_memcpy_from_user( char * dest, char * loop_start, char * loop_end, char __user * orig, int len)
{
	int left;

	left = loop_end - dest;
	if( left >= len )
	{
		copy_from_user( dest, orig, len );
		
	}
	else
	{
		copy_from_user( dest, orig, left );
		copy_from_user( loop_start, orig + left, len - left );
	}

}



static inline void memcpy_loop( char __user * dest, int len, char * orig, char * loop_start, char * loop_end )
{
	int left;

	left = loop_end - orig;
	if( left >= len )
	{
		memcpy( dest, orig, len );
	}
	else
	{
		memcpy( dest, orig, left );
		memcpy( dest+left, loop_start, len - left );
	}

}


static inline void memcpy_loop_to_user( char __user * dest, int len, char * orig, char * loop_start, char * loop_end )
{
	int left;

	left = loop_end - orig;
	if( left >= len )
	{
		copy_to_user( dest, orig, len );
	}
	else
	{
		copy_to_user( dest, orig, left );
		copy_to_user( dest+left, loop_start, len - left );
	}

}

static inline void delete_unread(void)
{
	unsigned int len;

	memcpy_loop( (char*)&len, sizeof(len), binlog_buf+binlog_rp, binlog_buf, binlog_bufend );
	BINLOG_PINC(binlog_rp, len + sizeof(len));
}

static void print_buf(void)
{
	int i;

	printk( KERN_DEBUG "rp=%d, wp=%d, used=%d, free=%d\n",binlog_rp,binlog_wp,BINLOG_USEDSPACE,BINLOG_FREESPACE);
	printk( KERN_DEBUG "buf=");
	for(i=0;i<BINLOG_LEN;i++)
	{
		printk( "%d:%c,", i, binlog_buf[i]);
	}
	printk( "\n" );
	
}

int binlog_write( char __user * buf, int len )
{
	int llen=2+len;
	int ret;

	printk( KERN_DEBUG "binlog_write()...\n");
	if( llen > BINLOG_SPACE )
	{
		printk( KERN_DEBUG "binlog_write(): len too long\n");
		ret = -EINVAL;
		goto out;
	}

	down(&binlog_mutex);

	while( llen+BINLOG_USEDSPACE > BINLOG_SPACE )
	{
		printk( KERN_DEBUG "binlog_write(): too many records unread, delete one\n");
		// delete a old unread bin log
		delete_unread();
	}

	loop_memcpy( binlog_buf + binlog_wp, binlog_buf, binlog_bufend, (char*)&len, sizeof(len));
	BINLOG_PINC( binlog_wp , sizeof(len));
	/*
	binlog_buf[binlog_wp] = len>>8;
	BINLOG_PINC( binlog_wp , 1);
	binlog_buf[binlog_wp] = len & (1<<8);
	BINLOG_PINC( binlog_wp , 1);
	*/

	loop_memcpy_from_user( binlog_buf + binlog_wp, binlog_buf, binlog_bufend, buf, len);
	BINLOG_PINC( binlog_wp , len);
	printk( KERN_DEBUG "binlog_write(): wp=%d, buf=%s,binlog_write=%s\n", binlog_wp,buf,binlog_buf+2 );

	print_buf();
	ret = len;
out:
	up(&binlog_mutex);
	if(ret>0)
	{
		wake_up( &binlog_waitqueue );
	}
	return ret;
}

int binlog_read( char __user * buf, int len, int flag )
{
	int l,ret;

	printk( KERN_DEBUG "binlog_read()...\n");

	down(&binlog_mutex);
	if( 0 == BINLOG_USEDSPACE )
	{
		if ( (flag & BINLOG_NONBLOCK) )
		{
			printk( KERN_DEBUG "binlog_read(): NONBLOCK but no record, return \n");
			ret = -EAGAIN;
			goto out;
		}
		else
		{
			DECLARE_WAITQUEUE(wait,current);

			printk( KERN_DEBUG "binlog_read(): BLOCK but no record, sleeping...\n");
			up(&binlog_mutex);

			add_wait_queue(&binlog_waitqueue, &wait);
			current->state = TASK_INTERRUPTIBLE;
			schedule();
			current->state = TASK_RUNNING;
			remove_wait_queue(&binlog_waitqueue, &wait);

			down(&binlog_mutex);
		}
	}

	if( 0 == BINLOG_USEDSPACE )
	{
		printk( KERN_DEBUG "binlog_read(): signal while sleeping ...\n");
		ret = 0;
		goto out;	
	}
	
	printk( KERN_DEBUG "binlog_read(): have record\n");
	memcpy_loop( (char*)&l, sizeof(l), binlog_buf+binlog_rp, binlog_buf, binlog_bufend );
	BINLOG_PINC( binlog_rp, sizeof(l) );
	/*
	l = binlog_buf[binlog_rp];
	BINLOG_PINC( binlog_rp, 1 );
	l <<= 8;
	l += binlog_buf[binlog_rp];
	BINLOG_PINC( binlog_rp, 1 );
	*/
	if( len<l )
	{
		printk( KERN_DEBUG "binlog_read(): len too small. %d<%d\n",len,l);
		ret = -EINVAL;
		goto out;
	}

	memcpy_loop_to_user( buf, l, binlog_buf+binlog_rp, binlog_buf, binlog_bufend );
	BINLOG_PINC( binlog_rp, l );
	printk( KERN_DEBUG "binlog_read(): copy and return %d\n",l);

	ret = l;
out:
	up(&binlog_mutex);
	return ret;
}


asmlinkage int sys_binlog_read( char __user * buf, int len, int flag )
{
	printk( KERN_DEBUG "sys_binlog_read()...\n");
	return binlog_read(buf, len, flag);
}

asmlinkage int sys_binlog_write( char __user * buf, int len, int flag )
{
	printk( KERN_DEBUG "sys_binlog_write()...\n");
	return binlog_write( buf, len );
}

static int __init binlog_init(void)
{
	printk("binlog module init...\n");

	binlog_rp=binlog_wp=0;


	return 0;
}

static void __exit binlog_cleanup(void)
{

	printk("binlog module exit...\n");
}

module_init(binlog_init);
module_exit(binlog_cleanup);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zu Zhihui");

