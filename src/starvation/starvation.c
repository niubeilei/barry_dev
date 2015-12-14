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
#include <linux/proc_fs.h>

#define MAX_STARVATION_TASK 50

typedef struct starvation_task
{
	struct list_head list;
	pid_t		pid;
	struct task 	*task;
}
starvation_task_t;

int g_num_starvation_task=0;
struct list_head g_starvation_task_list; 

static starvation_task_t * find_task_in_list( pid_t pid )
{
	starvation_task_t * st;

	list_for_each_entry( st, &g_starvation_task_list, list )
	{
		if(st->pid == pid )
			return st;
	}

	return 0;
}

asmlinkage int sys_starvation_register( pid_t pid )
{
	starvation_task_t * st;

	/* if list is full, return */

	/* if already registered, return */

	/* if the pid not existes, return */

	/* add this pid to the list */
	
	st = kmalloc( sizeof(*st), GFP_KERNEL );
	if( !st )
	{
		return -1;
	}

	/* need lock */
	list_add( &st->list,&g_starvation_task_list );
	g_num_starvation_task ++;
	return 0;
}

asmlinkage int sys_starvation_unregister( pid_t pid )
{
	starvation_task_t * st;

	/* need lock */

	st = find_task_in_list( pid );
	if ( ! st )
	{
		return -1;
	}

	list_del( &st->list );
	
	
	return 0;
}

static int starvation_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	starvation_task_t * st;

	if ( offset>0 )
		return 0;
	sprintf( buf, "this is starvation proc\n" );

	buf[0]=0;
	list_for_each_entry( st, &g_starvation_task_list, list )
	{
		sprintf( &buf[strlen(buf)], "%d ", st->pid );
	}

	return strlen( buf );
}

static int starvation_register_proc(void)
{
	if ( 0 == create_proc_read_entry("starvation",0444,&proc_root,starvation_proc_read,0) )
	{
		printk( KERN_ERR "create starvation proc file failed\n");
		return -1;
	}
	return 0;
}

static int starvation_unregister_proc(void)
{
	remove_proc_entry( "starvation", &proc_root );
	return 0;
}

static int __init starvation_init(void)
{
	int ret;
	printk("starvation module init...\n");
	 
	INIT_LIST_HEAD( &g_starvation_task_list );
	g_num_starvation_task = 0;
	ret = starvation_register_proc();
	if ( ret )
		return ret;

	return 0;
}

static void __exit starvation_cleanup(void)
{

	printk("starvation module exit...\n");
	starvation_unregister_proc();
}

module_init(starvation_init);
module_exit(starvation_cleanup);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zu Zhihui");

