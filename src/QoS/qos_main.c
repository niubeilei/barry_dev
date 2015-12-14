#include "KernelSimu/module.h"

#include "aosUtil/Types.h"
#include "aosUtil/Alarm.h"
#include "QoS/qos_rule.h"
#include "QoS/qos_traffic.h"
#include "QoS/qos_nf.h"
#include "QoS/qos_bw_man.h"
#include "QoS/qos_shaper.h"
#include "QoS/qos_config.h"
#include "QoS/qos_proc.h"
#include "QoS/qos_logstat.h"


//extern int AosQos_initConfModule(void);

#ifdef __KERNEL__
static __init int aosqos_init(void)
#else
int aosqos_init(void)
#endif
{
	int ret = 0;

	aos_min_log(eAosMD_QoS, "INFO:Initing module");

	ret = AosQos_initRuleModule();
	if (ret != 0) {
		aos_min_log(eAosMD_QoS, "ERROR:Init rule failed");
		goto fail1;
	}
	aos_min_log(eAosMD_QoS, "INFO:Initing Traffic monitor");

	ret = AosQos_initTrafficModule();
	if (ret != 0) {
		aos_min_log(eAosMD_QoS, "ERROR:Init Traffic failed");
		goto fail2;
	}

	ret = ros_qos_bw_ctrl_init();
	if (ret)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to init QoS Bandwidth Manager module");
		goto fail3;
	}	

	ret = ros_qos_shaper_init();
	if (ret)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to init QoS Traffic Shaper module");
		goto shaper_init_fail;
	}

#ifdef __KERNEL__	
	ret = ros_qos_register_proc();
	if (ret)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to register QoS proc");
		goto reg_proc_fail;
	}
#endif
	
	aos_min_log(eAosMD_QoS, "INFO:Initing Netfilter");

	ret = AosQos_initNFModule();
	if (ret != 0) {
		aos_min_log(eAosMD_QoS, "ERROR:Init Netfilter failed");
		goto fail3;
	}
	ret = AosQos_initConfModule();
	if (ret != 0) {
		aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Failed to init QoS Config module");
		goto fail3;
	}

	ret = qos_logstat_register_cli();
	if (ret != 0) 
	{
		aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Failed to register log statistics CLIs");
		goto fail3;
	}
	aos_min_log(eAosMD_QoS, "QoS log stat CLI register");	
	return ret;

	
shaper_init_fail:
	
#ifdef __KERNEL__
reg_proc_fail:
	ros_qos_unregister_proc();
#endif
	
fail3:
	AosQos_exitTrafficModule();
fail2:
	AosQos_exitRuleModule();
fail1:
	return ret;

}


static __exit void aosqos_exit(void)
{
	AosQos_exitConfModule();
	AosQos_exitNFModule();
	AosQos_exitTrafficModule();
	AosQos_exitRuleModule();
	qos_logstat_unregister_cli();
	return;
}


#ifdef __KERNEL__
module_init(aosqos_init);
module_exit(aosqos_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ping Wang");
#endif
