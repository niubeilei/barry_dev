////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_cn1010.mod.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x4cdc490c, "cleanup_module" },
	{ 0xcd58c830, "init_module" },
	{ 0x1c26f7d8, "struct_module" },
	{ 0x1a1a4f09, "__request_region" },
	{ 0x7da8156e, "__kmalloc" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x5999e347, "malloc_sizes" },
	{ 0x20000329, "simple_strtoul" },
	{ 0x6add261b, "remove_proc_entry" },
	{ 0x464b8216, "pci_find_subsys" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x2fd1d81c, "vfree" },
	{ 0x1d26aa98, "sprintf" },
	{ 0xda02d67, "jiffies" },
	{ 0xd533bec7, "__might_sleep" },
	{ 0x865ebccd, "ioport_resource" },
	{ 0x2ed9713c, "proc_mkdir" },
	{ 0x1b7d4074, "printk" },
	{ 0xed5c73bf, "__tasklet_schedule" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0x28c3bbf5, "__down_failed_interruptible" },
	{ 0xa5808bbf, "tasklet_init" },
	{ 0x707f93dd, "preempt_schedule" },
	{ 0x79ad224b, "tasklet_kill" },
	{ 0x6a1d7f68, "kmem_cache_alloc" },
	{ 0x26e96637, "request_irq" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0x2ec06326, "register_chrdev" },
	{ 0x6403bb7d, "create_proc_entry" },
	{ 0xcb95ef42, "wake_up_process" },
	{ 0xd49501d4, "__release_region" },
	{ 0xab821cad, "__wake_up" },
	{ 0x37a0cba, "kfree" },
	{ 0x932da67e, "kill_proc" },
	{ 0x9d669763, "memcpy" },
	{ 0xdf20e9c, "interruptible_sleep_on_timeout" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0x7e9ebb05, "kernel_thread" },
	{ 0xd22b546, "__up_wakeup" },
	{ 0xcdd91958, "pci_enable_device" },
	{ 0x16969c4a, "kernel_flag" },
	{ 0xd6c963c, "copy_from_user" },
	{ 0xdc43a9c8, "daemonize" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

