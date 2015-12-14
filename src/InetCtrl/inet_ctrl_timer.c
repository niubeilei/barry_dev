/*#include <linux/list.h>*/
#include "inet_ctrl_timer.h"

#include "inet_ctrl.h"
#include "inet_ctrl_hash.h"

#include "KernelSimu/list.h"
#include <linux/timer.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <asm/atomic.h>
#include <linux/kthread.h>

#include <asm/uaccess.h>
#include <linux/interrupt.h>

/*
#include "KernelSimu/timer.h"
#include "KernelSimu/kernel.h"
#include "aosUtil/Memory.h"
*/

#define	UDP_TIMEOUT_SECS		20
#define	SLOW_TIMEOUT_SECS		60
#define	FAST_TIME_OUT			(UDP_TIMEOUT_SECS*HZ)
#define	SLOW_TIME_OUT			(SLOW_TIMEOUT_SECS*HZ)

#define	WAKEUP_WRITEFILE_EVENT	31
#define	WAKEUP_READFILE_EVENT	30

static void fast_time_out(unsigned long);
static void slow_time_out(unsigned long);

static DEFINE_TIMER(inetctrl_fast_timeout, fast_time_out, 0, 0);
static DEFINE_TIMER(inetctrl_slow_timeout, slow_time_out, 0, 0);

static struct task_struct *writefile_thread = NULL;
static unsigned long writefile_event = 0;

static atomic_t terminate = ATOMIC_INIT(0), running = ATOMIC_INIT(0);

static void wakeup_writefile_thread(int evt)
{
	if (writefile_thread == NULL)
		return;
	set_bit(evt, &writefile_event);
	wake_up_process(writefile_thread);
}

static int writefile_run(void *unused)
{
	writefile_thread = current;

	atomic_inc(&running);
	daemonize("writefiled");
	set_user_nice(current, -10);
	current->flags |= PF_NOFREEZE;

	set_fs(KERNEL_DS);
	
	while (!atomic_read(&terminate)) {
		set_current_state(TASK_RUNNING);

		/* write event */
		if (test_bit(WAKEUP_WRITEFILE_EVENT, &writefile_event)) {
			clear_bit(WAKEUP_WRITEFILE_EVENT, &writefile_event);

			/* write to persist disk */
			/*
			 * Notes: don't add any lock here, because I/O operation may
			 * result in interrupt, but using file in kernel, it can't enter
			 * into interrupt twice, ortherwise it will panic.
			 *
			 * I changed the lock to the location where data_list is used.
			 * Please refer to inet_ctrl.c
			 */
			if (!atomic_read(&terminate)) 
				writeto_online_file();
		}
	
		/* read event */
		if (test_bit(WAKEUP_READFILE_EVENT, &writefile_event)) {
			clear_bit(WAKEUP_READFILE_EVENT, &writefile_event);
		
			/* read from persist disk */
			if (!atomic_read(&terminate))
				readfrom_online_file();
		}

		/* No pending events. Let's sleep. */
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
	}
	set_current_state(TASK_RUNNING);
	
	atomic_dec(&running);
	return 0;
}

/* go through the whole hash tab */
static int state_udp_checking(void* k, void* d, void* args)
{
	state_entry_t* entry = (state_entry_t*)d;
	int cur_time;
	
	if (entry->st_tuple.proto != FLOW_TYPE_UDP)
		return 0; /* next */
	
	 /* time out, so remove it */
	 cur_time = get_secoftoday();
	if (cur_time - entry->st_access > UDP_TIMEOUT_SECS) {
		state_entry_data_t* data = entry->st_data;
		if (atomic_read(&data->data_refcnt) == 1) {
			/* no state contains this data except this state */
			FLAG_SET(data->data_flag, ENTRY_UDP_TIMEOUT);
			data->data_timeleft -= cur_time - data->data_enter;
			data->data_total_timeleft -= cur_time - data->data_enter;
		}

		/* 
		 * Must remove at once, or only after this state is cleared, user can get online,
		 * also 1 minute 
		 */
		inet_ctrl_hash_remove((five_tuple_t*)k);
	} else
		FLAG_UNSET(entry->st_data->data_flag, ENTRY_UDP_TIMEOUT);

	return 0;
}

static void fast_time_out(unsigned long arg)
{
	inet_ctrl_hash_map(state_udp_checking, NULL);
	mod_timer(&inetctrl_fast_timeout, FAST_TIME_OUT+jiffies);
		
	INETCTRL_INFO("fast time out\n");
}

static int walk_data(void* elm, void* arg)
{
	state_entry_data_t* data = (state_entry_data_t*)elm;
	int tv, cur_t;
	
	if (!ENTRY_DATA_GOOD(data)) {
		remove_state_data(data);
		return 0;
	}
	
	 /* no state holds this data, can't count time */
	if (FLAG_ISSET(data->data_flag, DATA_PRE_DELETE))
		goto out;
	
	cur_t = get_secoftoday();
	tv = cur_t - data->data_enter;
	data->data_enter = cur_t;
	
	/* critical, due to the difference between unsigned type and singed type */
	if (data->data_timeleft < tv)
		data->data_timeleft = 0;
	else
		data->data_timeleft -= tv;

	if (data->data_total_timeleft < tv)
		data->data_total_timeleft = 0;
	else
		data->data_total_timeleft -= tv;
	
out:
	if (FLAG_ISSET(data->data_flag, ONLINE_DAY_CHANGE)) {
		FLAG_UNSET(data->data_flag, ONLINE_DAY_CHANGE);
		/* refresh day */
		reset_online_ctrl(&data->data_time->rule_online_head, &data->data_rec, 0);
	} else if (FLAG_ISSET(data->data_flag, ONLINE_WEEK_CHANGE)) {
		FLAG_UNSET(data->data_flag, ONLINE_WEEK_CHANGE);
		/* refresh week */
		reset_online_ctrl(&data->data_time->rule_online_head, &data->data_rec, 1);
	}

	return 0;
}

static int update_hashtab(void* k, void* d, void* args)
{
	state_entry_t* entry = (state_entry_t*)d;
	
	if (FLAG_ISSET(entry->st_flag, ENTRY_PRE_DELETE)) {
		inet_ctrl_hash_remove((five_tuple_t*)k);
		return 0;
	}
	
	entry->st_update += SLOW_TIMEOUT_SECS;
	return 0;
}

/* go through the entry data list */
static void slow_time_out(unsigned long arg)
{
	execute_slow_timeout();
	mod_timer(&inetctrl_slow_timeout, SLOW_TIME_OUT+jiffies);
	
	INETCTRL_INFO("slow time out\n");
}

int inetctrl_init_timer(void)
{
	/* must read from file */
	wakeup_writefile_thread(WAKEUP_READFILE_EVENT);

	inetctrl_fast_timeout.expires = jiffies + FAST_TIME_OUT;
	inetctrl_slow_timeout.expires = jiffies + SLOW_TIME_OUT;
	
	add_timer(&inetctrl_fast_timeout);
	add_timer(&inetctrl_slow_timeout);
	
	INETCTRL_INFO("Initialize the timer\n");
	return 0;
}

int inetctrl_remove_timer(void)
{
	del_timer(&inetctrl_fast_timeout);
	del_timer(&inetctrl_slow_timeout);
	
	/*
	 * Below code will be no useful because Bottom Half will be faster than
	 * kernel thread, so before data is written to file, the data list
	 * will be empty
	 */
	/* execute_slow_timeout(); */

	INETCTRL_INFO("Remove the timer\n");
	return 0;
}

int start_writefile_thread(void)
{
	INETCTRL_INFO("loading write file daemon ...\n");
	
	kernel_thread(writefile_run, NULL, CLONE_KERNEL);
	return 0;
}

int stop_writefile_thread(void)
{
	atomic_inc(&terminate);
	wakeup_writefile_thread(WAKEUP_WRITEFILE_EVENT);

	/* Wait until thread is running */
	while (atomic_read(&running))
		schedule();

	INETCTRL_INFO("unloading write file daemon ...\n");
	return 0;
}

void execute_slow_timeout()
{
	inet_ctrl_hash_map(update_hashtab, NULL);
	walk_statedata_list(walk_data, NULL);
	
	/* wake up a kernel thread to do I/O */
	wakeup_writefile_thread(WAKEUP_WRITEFILE_EVENT);
}


