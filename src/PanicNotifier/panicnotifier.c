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
#include <linux/syscalls.h>

static int panic_happened(struct notifier_block *n, unsigned long val, void *v)
{
	int fd,ret;
	char *s="panic\n";
	printk( KERN_EMERG "sytem panic. write data to /panic.txt");

	fd = sys_open("/panic.txt",O_CREAT|O_WRONLY,S_IRWXU);
	ret = sys_write(fd,s,strlen(s));
	sys_close(fd);
	return 0;
}

static struct notifier_block panic_notifier = { panic_happened, NULL, 1 };

static int __init panicnotifier_init(void)
{
	printk("panicnotifier module init...\n");
	notifier_chain_register(&panic_notifier_list, &panic_notifier);
	return 0;
}

static void __exit panicnotifier_cleanup(void)
{
	printk("panicnotifier module exit...\n");
	notifier_chain_unregister(&panic_notifier_list, &panic_notifier);
}

module_init(panicnotifier_init);
module_exit(panicnotifier_cleanup);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zu Zhihui");

