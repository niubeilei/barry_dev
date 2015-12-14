#include "iac.h"
#include <stdio.h>

void printf_result(int expect, int ret, int line)
{
	if (expect == 1 && ret == 0)
		printf("%d, Expect: %s, Real: %s -------%s\n", line, "Correct", "true", "OK");
	else if (expect == 0 && ret < 0)
		printf("%d, Expect: %s, Real: %s -------%s\n", line, "Failed", "failed", "OK");
	else
		printf("%d, Expect: %s, Real: %s -------%s\n", line, (expect)?"Correct":"Failed", (ret<0)?"failed":"true", "FAILED");

	return;
}

int main()
{
	int ret;
	char result[8196];
	int len = sizeof(result);

	ret = aos_iac_onoff("on");
	printf_result(1, ret, __LINE__);
	
	ret = aos_iac_log_level("no");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_log_level("block");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_log_level("all");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_set_policy("allow");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_set_policy("deny");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_retrieve(result, &len);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_app("app1");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_app("app2");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_app("app2");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_flow("app1", "tcp", "172.22.0.1", "255.255.255.255", 0, 65534);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_flow("app1", "tcp", "172.22.0.2", "255.255.255.255", 0, 65534);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_flow("app1", "tcp", "172.22.0.2", "255.255.255.255", 0, 65534);
	printf_result(1, ret, __LINE__);

	len = sizeof(result);
	ret = aos_iac_app_retrieve(result, &len);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_timerule("time1");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_timerule("time2");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_timerule("time2");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_timectrl("time1", "12:00:00", "13:00:00", "Mon");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_timectrl("time1", "13:00:00", "12:00:00", "Mon");
	printf_result(0, ret, __LINE__);

	ret = aos_iac_add_timectrl("time1", "12:00:10", "13:00:00", "mon");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_timectrl("time1", "12:00:00", "13:00:00", "mon"); 
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_online("time1", "day", 10, "mon");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_online("time1", "day", 12, "tue");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_online("time1", "day", 10, "mon");
	printf_result(1, ret, __LINE__);

	len = sizeof(result);
	ret = aos_iac_timerule_retrieve(result, &len);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_group("group1");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_group("group2");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_group("group2");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_adduser_byip("group1", "192.168.0.1", "255.255.255.255");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_adduser_byip("group1", "192.168.0.2", "255.255.255.255");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_revuser_byip("group5", "192.168.0.1", "255.255.255.255");
	printf_result(0, ret, __LINE__);

	ret = aos_iac_add_rule("group1", "app1", "time1");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_rule("group1", "app2", "time1");
	printf_result(0, ret, __LINE__);

	ret = aos_iac_remove_rule("group1", "app1", "time1");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_group_retrieve(result, &len);
	printf_result(1, ret, __LINE__);

	len = sizeof(result);
	ret = aos_iac_save_config(result, &len);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_clear_config();
	printf_result(1, ret, __LINE__);

	printf("Twice:Again-----------------------------------------------\n");
	
	ret = aos_iac_gen_app("app3");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_app("app4");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_app("app4");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_flow("app3", "tcp", "172.22.0.1", "255.255.255.255", 0, 65534);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_flow("app3", "tcp", "haha", "255.255.255.255", 0, 65534);
	printf_result(0, ret, __LINE__);

	ret = aos_iac_add_flow("app3", "tcp", "172.22.0.2", "255.255.255.255", 0, 65534);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_flow("app3", "tcp", "172.22.0.2", "255.255.255.255", 0, 65534);
	printf_result(1, ret, __LINE__);
	
	len = sizeof(result);
	ret = aos_iac_app_retrieve(result, &len);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_timerule("time3");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_timerule("time4");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_timerule("time4");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_timectrl("time3", "12:00:00", "15:00:00", "mon");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_timectrl("time3", "12:00:10", "16:00:00", "mon");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_timectrl("time3", "12:00:00", "15:00:00", "mon"); 
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_online("time3", "day", 10, "mon");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_online("time3", "day", 10, "moi");
	printf_result(0, ret, __LINE__);

	ret = aos_iac_add_online("time3", "day", 10, "tue");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_online("time3", "day", 10, "mon");
	printf_result(1, ret, __LINE__);

	len = sizeof(result);
	ret = aos_iac_timerule_retrieve(result, &len);
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_group("group3");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_gen_group("group4");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_group("group4");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_adduser_byip("group3", "192.168.0.1", "255.255.255.255");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_adduser_byip("group3", "192.168.0.2", "255.255.255.255");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_revuser_byip("group3", "192.168.0.1", "255.255.255.255");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_add_rule("group3", "app3", "time3");
	printf_result(1, ret, __LINE__);

	ret = aos_iac_remove_rule("group3", "app3", "time3");
	printf_result(1, ret, __LINE__);

	len = sizeof(result);
	ret = aos_iac_group_retrieve(result, &len);
	printf_result(1, ret, __LINE__);
	
	ret = aos_iac_group_clear();
	printf_result(1, ret, __LINE__);
	
	ret = aos_iac_app_clear();
	printf_result(1, ret, __LINE__);
	
	ret = aos_iac_timerule_clear();
	printf_result(1, ret, __LINE__);
	
	return 0;
}

