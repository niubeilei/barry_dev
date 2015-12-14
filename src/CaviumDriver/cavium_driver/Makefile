#HOSTCFLAGS+=-DCAVIUM_NO_MMAP -DCN1010 -DCAVIUM_DEBUG_LEVEL=1 -DINTERRUPT_ON_COMP -DCAVIUM_HUGE_MEMORY -DN1_SANITY -DCAVIUM_RESOURCE_CHECK
CFLAGS+=-DCAVIUM_NO_MMAP -DCN1010 -DCAVIUM_DEBUG_LEVEL=1 -DINTERRUPT_ON_COMP -DCAVIUM_HUGE_MEMORY -DN1_SANITY -DCAVIUM_RESOURCE_CHECK
#HOSTCFLAGS+=-DSSL -DMC2 -Dlinux
CFLAGS+=-DSSL -DMC2 -Dlinux

cavium_cn1010-objs	:=admin_command_que.o bl_nbl_list.o buffer_pool.o cavium.o cavium_proc.o cavium_random.o \
					command_que.o completion_dma_free_list.o context_memory.o direct_free_list.o error_handler.o \
					hw_lib.o init_cfg.o interrupt.o key_memory.o linux_main.o pending_free_list.o pending_list.o\
					request_manager.o sg_dma_free_list.o sg_free_list.o soft_req_queue.o \
					cavium_kernel_common.o kernel_ssl_api.o \
					aos_poll_timer.o aos_request.o
					#poll_thread.o
obj-$(CONFIG_CAVIUM_CN1010) += cavium_cn1010.o
					
					
