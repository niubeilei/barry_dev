#include "aosApi.h"
#include <stdio.h>
//#include <string.h>
#include <iostream.h>
#include <stdlib.h>
#define FW_MAX_SHOW_CONFIG 10000

int main(int argc,char **argv)
{
	char rslt[FW_MAX_SHOW_CONFIG];
	int ret;

	int index=1;
	int maxruntime=2;
	int count=0;
	 int total_case = 0;
    int fail_case = 0;
	
	while(index<argc)
	{
		if(strcmp(argv[index],"-t")==0)
		{
			maxruntime=atoi(argv[++index]);
			index++;
			continue;
		}
		if(strcmp(argv[index],"-h")==0)
		{
			cout<<"----------------------helps for usage------------------------------------------"<<endl;
			cout<<"1 usage: ./firewalltester [-t [count]]"<<endl;
			cout<<""<<endl;
			cout<<"2 the parameter [-t [count]] is optinal,it control the total run count of this application ."<<endl;
			cout<<" it's default value is 2 ,if it need to set it .it need a number of 2's times(eg: 2,4,6,...)  "<<endl;
			cout<<""<<endl;
			cout<<"3 the result will be added to the files firewallresult1 and firewallresult2 in the current directory at the mean"<<endl;
			cout<<"  time,and this two result file's content should be the same"<<endl;
			cout<<""<<endl;
			cout<<"4 the result will be added to the files firewallresult1 and firewallresult2 in the current directory at the mean time"<<endl;
			cout<<"  and the contents of firewallresult1 and firewallresult2 shoule be the same"<<endl;
			cout<<""<<endl;
			exit(0);
		}
	}

	while(count<maxruntime)
	{
	//	aos_firewall_clear_config();
		printf("================================ Now run the %d time=========================================\n",count);
		printf("==============================================================================================\n",count);
		FILE *fp=NULL;
		if(count%2==0)
		{
			fp=fopen("firewallresult1","a+");

		}
		else
		{
			fp=fopen("firewallresult2","a+");

		}

		ret = aos_firewall_init(0);
		printf("firewall minsec init ret = %d\n", ret);
		fprintf(fp,"firewall minsec init ret = %d\n", ret);
		printf("-------------------------------------\n");
   total_case++;
    if(ret!=0) fail_case++;
    	
		//ret = aos_firewall_init(1);
		//printf("firewall midsec init ret = %d\n", ret);
		//printf("-------------------------------------\n");

		//ret = aos_firewall_init(2);
		//printf("firewall maxsec init ret = %d\n", ret);
		//printf("-------------------------------------\n");

		ret = aos_firewall_retrieve_config(rslt, FW_MAX_SHOW_CONFIG);
		printf("firewall show config ret = %d\n%s\n", ret, rslt);
		fprintf(fp,"firewall show config ret = %d\n%s\n", ret, rslt);
		printf("-------------------------------------\n");
     total_case++;
     if(ret!=0) fail_case++;

		ret = aos_firewall_syncookie(1);
		printf("firewall syncookie on ret = %d\n", ret);
		fprintf(fp,"firewall syncookie on ret = %d\n", ret);
		printf("-------------------------------------\n");

	total_case++;
    if(ret!=0) fail_case++;

		ret = aos_firewall_antiattack_outcard("syn", 2);
		printf("firewall antiattack outcard ret = %d\n", ret);
		fprintf(fp,"firewall antiattack outcard ret = %d\n", ret);
		printf("-------------------------------------\n");

	total_case++;
    if(ret!=0) fail_case++;

		ret = aos_firewall_outcard_ping(1);
		printf("firewall outcard ping ret = %d\n", ret);
		fprintf(fp,"firewall outcard ping ret = %d\n", ret);
		printf("-------------------------------------\n");

total_case++;
    if(ret!=0) fail_case++;
   
		ret = aos_firewall_vpn_pass_ipsec(1);
		printf("firewall ipsec_pass on ret = %d\n", ret);
		fprintf(fp,"firewall ipsec_pass on ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		ret = aos_firewall_vpn_pass_pptp(1);
		printf("firewall pptp_pass on ret = %d\n", ret);
		fprintf(fp,"firewall pptp_pass on ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		ret = aos_firewall_vpn_pass_l2tp(1);
		printf("firewall l2tp_pass on ret = %d\n", ret);
		fprintf(fp,"firewall l2tp_pass on ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rslt,"");
		ret = aos_firewall_retrieve_config(rslt, FW_MAX_SHOW_CONFIG);
		printf("firewall show config ret = %d\n%s\n", ret, rslt);
		fprintf(fp,"firewall show config ret = %d\n%s\n", ret, rslt);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		ret = aos_firewall_clear_config();
		printf("firewall clear config ret = %d\n", ret);
		fprintf(fp,"firewall clear config ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rslt,"");
		ret = aos_firewall_retrieve_config(rslt, FW_MAX_SHOW_CONFIG);
		printf("firewall show config ret = %d\n%s\n", ret, rslt);
		fprintf(fp,"firewall show config ret = %d\n%s\n", ret, rslt);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   

		cout<<"========================= firewall antiattacke  test section ========================="<<endl;
		cout<<"======================================================================================"<<endl;
		ret = aos_firewall_anti_syn_add("1.1.1.1", 8888, 10);
		cout<<"cmd: firewall antiattack syn 1.1.1.1 8888 10"<<endl;
		printf("firewall antiattack syn add ret = %d\n", ret);
		fprintf(fp,"firewall antiattack syn add ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   

		ret = aos_firewall_anti_icmp_add("1.1.1.1", 10);
		cout<<"cmd: firewall antiattack icmp 1.1.1.1 10"<<endl;
		printf("firewall antiattack icmp add ret = %d\n", ret);
		fprintf(fp,"firewall antiattack icmp add ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rslt,"");
		ret = aos_firewall_retrieve_config(rslt, FW_MAX_SHOW_CONFIG);
		printf("firewall show config ret = %d\n%s\n", ret, rslt);
		fprintf(fp,"firewall show config ret = %d\n%s\n", ret, rslt);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		ret = aos_firewall_anti_icmp_del("1.1.1.1", 10);
		printf("firewall antiattack icmp del ret = %d\n", ret);
		fprintf(fp,"firewall antiattack icmp del ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		ret = aos_firewall_anti_syn_del("1.1.1.1", 8888, 10);
		printf("firewall antiattack syn del ret = %d\n", ret);
		fprintf(fp,"firewall antiattack syn del ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rslt,"");
		ret = aos_firewall_retrieve_config(rslt, FW_MAX_SHOW_CONFIG);
		printf("firewall show config ret = %d\n%s\n", ret, rslt);
		fprintf(fp,"firewall show config ret = %d\n%s\n", ret, rslt);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		cout<<"========================= firewall rules add test section ========================="<<endl;
		cout<<"======================================================================================"<<endl;
		struct fwFilterListEntry rules;
		strcpy(rules.type 	,"fwIpfilAll");
		strcpy(rules.proto 	,"all"			);
		strcpy(rules.chain 	,"in"			);
		strcpy(rules.sip 	,"1.1.1.1"	);
		strcpy(rules.smask 	,"255.255.255.255"	);
		strcpy(rules.dip 	,"2.2.2.2"			);
		strcpy(rules.dmask 	,"255.255.255.255"	);
		strcpy(rules.action ,"permit"			);
		strcpy(rules.log 	,"nolog"			);
		ret = aos_firewall_add_filter(&rules);
 cout<<"firewall ipfil all in 1.1.1.1 255.255.255.255 2.2.2.2 255.255.255.255 permit nolog"<<endl;
		printf("firewall ipfil all ret = %d\n", ret);
		fprintf(fp,"firewall ipfil all ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rules.type, "fwIpfilTcp");
		strcpy(rules.proto 	,"tcp");
		strcpy(rules.chain 	,"in");
		strcpy(rules.sip 	,"1.1.1.1");
		strcpy(rules.smask 	,"255.255.255.0");
		strcpy(rules.sport 	,"1111");
		strcpy(rules.dip 	,"2.2.2.2");
		strcpy(rules.dmask 	,"255.255.255.0");
		strcpy(rules.dport 	,"11111");
		strcpy(rules.action ,"permit");
		strcpy(rules.log 	,"nolog");
		ret = aos_firewall_add_filter(&rules);
 cout<<"firewall ipfil tcp in 1.1.1.1 255.255.255.0 1111 2.2.2.2 255.255.255.0 11111 permit nolog"<<endl;
		printf("firewall ipfil tcp ret = %d\n", ret);
		fprintf(fp,"firewall ipfil tcp ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rules.type, "fwTimefil");
		strcpy(rules.proto 	,"tcp");
		strcpy(rules.chain 	,"in");
		strcpy(rules.sip 	,"1.1.1.1");
		strcpy(rules.smask 	,"255.255.255.0");
		strcpy(rules.dip 	,"2.2.2.2");
		strcpy(rules.dmask 	,"255.255.255.0");
		strcpy(rules.beginTime ,"00:01");
		strcpy(rules.endTime ,"10:01");
		strcpy(rules.week ,"Mon");
		strcpy(rules.action ,"permit");
		strcpy(rules.log 	,"nolog");
		ret = aos_firewall_add_filter(&rules);
		cout<<"firewall timefil in tcp 1.1.1.1 255.255.255.0 2.2.2.2 255.255.255.0 permit 00:01 10:01 Mon nolog"<<endl;
		printf("firewall timefil ret = %d\n", ret);
		fprintf(fp,"firewall timefil ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   

		strcpy(rules.type ,"fwMacfilAll");
		strcpy(rules.proto ,"all");
		strcpy(rules.chain ,"in");
		strcpy(rules.smac ,"11:22:33:44:55:66");
		strcpy(rules.dip ,"2.2.2.2");
		strcpy(rules.dmask ,"255.255.255.255");
		strcpy(rules.action ,"permit");
		strcpy(rules.log ,"nolog");
		ret = aos_firewall_add_filter(&rules);
		cout<<"firewall macfil all in 11:22:33:44:55:66 2.2.2.2 255.255.255.255 permit nolog"<<endl;
		printf("firewall macfil all ret = %d\n", ret);
		fprintf(fp,"firewall macfil all ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rules.type ,"fwMacfilTcp");
		strcpy(rules.proto ,"tcp");
		strcpy(rules.chain ,"in");
		strcpy(rules.smac,"aa:bb:cc:dd:ee:00");
		strcpy(rules.sport ,"1111:2222");
		strcpy(rules.dip ,"2.2.2.2");
		strcpy(rules.dmask ,"255.255.255.255");
		strcpy(rules.dport ,"11111:22222");
		strcpy(rules.action ,"permit");
		strcpy(rules.log ,"nolog");
		ret = aos_firewall_add_filter(&rules);
		cout<<"firewall macfil tcp in aa:bb:cc:dd:ee:00 1111:2222 2.2.2.2 255.255.255.255 11111:22222 permit nolog"<<endl;
		printf("firewall macfil tcp ret = %d\n", ret);
		fprintf(fp,"firewall macfil tcp ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rslt,"");
		ret = aos_firewall_retrieve_config(rslt, FW_MAX_SHOW_CONFIG);
		printf("firewall show config ret = %d\n%s\n", ret, rslt);
		fprintf(fp,"firewall show config ret = %d\n%s\n", ret, rslt);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   

		//del	
		//	struct fwFilterListEntry rules;
		strcpy(rules.type 	,"fwIpfilAll");
		strcpy(rules.proto 	,"all"			);
		strcpy(rules.chain 	,"in"			);
		strcpy(rules.sip 	,"1.1.1.1"	);
		strcpy(rules.smask 	,"255.255.255.255"	);
		strcpy(rules.dip 	,"2.2.2.2"			);
		strcpy(rules.dmask 	,"255.255.255.255"	);
		strcpy(rules.action ,"permit"			);
		strcpy(rules.log 	,"nolog"			);
		ret = aos_firewall_del_filter(&rules);
		printf("firewall ipfil all ret del = %d\n", ret);
		fprintf(fp,"firewall ipfil all ret del = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rules.type, "fwIpfilTcp");
		strcpy(rules.proto 	,"tcp");
		strcpy(rules.chain 	,"in");
		strcpy(rules.sip 	,"1.1.1.1");
		strcpy(rules.smask 	,"255.255.255.0");
		strcpy(rules.sport 	,"1111");
		strcpy(rules.dip 	,"2.2.2.2");
		strcpy(rules.dmask 	,"255.255.255.0");
		strcpy(rules.dport 	,"11111");
		strcpy(rules.action ,"permit");
		strcpy(rules.log 	,"nolog");
		ret = aos_firewall_del_filter(&rules);
		printf("firewall ipfil tcp ret del= %d\n", ret);
		fprintf(fp,"firewall ipfil tcp ret del= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rules.type, "fwTimefil");
		strcpy(rules.proto 	,"tcp");
		strcpy(rules.chain 	,"in");
		strcpy(rules.sip 	,"1.1.1.1");
		strcpy(rules.smask 	,"255.255.255.0");
		strcpy(rules.dip 	,"2.2.2.2");
		strcpy(rules.dmask 	,"255.255.255.0");
		strcpy(rules.beginTime ,"00:01");
		strcpy(rules.endTime ,"10:01");
		strcpy(rules.week ,"Mon");
		strcpy(rules.action ,"permit");
		strcpy(rules.log 	,"nolog");
		ret = aos_firewall_del_filter(&rules);
		printf("firewall timefil ret del= %d\n", ret);
		fprintf(fp,"firewall timefil ret del= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   

		strcpy(rules.type ,"fwMacfilAll");
		strcpy(rules.proto ,"all");
		strcpy(rules.chain ,"in");
		strcpy(rules.smac ,"11:22:33:44:55:66");
		strcpy(rules.dip ,"2.2.2.2");
		strcpy(rules.dmask ,"255.255.255.255");
		strcpy(rules.action ,"permit");
		strcpy(rules.log ,"nolog");
		ret = aos_firewall_del_filter(&rules);
		printf("firewall macfil all ret del= %d\n", ret);
		fprintf(fp,"firewall macfil all ret del = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rules.type ,"fwMacfilTcp");
		strcpy(rules.proto ,"tcp");
		strcpy(rules.chain ,"in");
		strcpy(rules.smac,"aa:bb:cc:dd:ee:00");
		strcpy(rules.sport ,"1111:2222");
		strcpy(rules.dip ,"2.2.2.2");
		strcpy(rules.dmask ,"255.255.255.255");
		strcpy(rules.dport ,"11111:22222");
		strcpy(rules.action ,"permit");
		strcpy(rules.log ,"nolog");
		ret = aos_firewall_del_filter(&rules);
		printf("firewall macfil tcp ret del= %d\n", ret);
		fprintf(fp,"firewall macfil tcp ret del= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rslt,"");
		ret = aos_firewall_retrieve_config(rslt, FW_MAX_SHOW_CONFIG);
		printf("firewall show config ret del= %d\n%s\n", ret, rslt);
		fprintf(fp,"firewall show config ret del= %d\n%s\n", ret, rslt);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   


		
		cout<<"========================= firewall natrules  test section ========================="<<endl;
		cout<<"======================================================================================"<<endl;
		struct fwNatListEntry natrules;
		strcpy(natrules.type ,"fwNatSnat");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.smask ,"255.255.255.255");
		strcpy(natrules.to ,"2.2.2.2");
		ret = aos_firewall_add_nat(&natrules);
		cout<<"firewall snat outcard 1.1.1.1 255.255.255.255 2.2.2.2"<<endl;
		printf("firewall snat ret = %d\n", ret);
		fprintf(fp,"firewall snat ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"fwNatMasq");
		strcpy(natrules.interface ,"incard");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.smask ,"255.255.255.255");
		ret = aos_firewall_add_nat(&natrules);
		cout<<"firewall masq incard 1.1.1.1 255.255.255.255"<<endl;
		printf("firewall masq ret = %d\n", ret);
		fprintf(fp,"firewall masq ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"fwNatDnatIP");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.proto ,"icmp");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.dip ,"2.2.2.2");
		ret = aos_firewall_add_nat(&natrules);
		cout<<"firewall dnat ip outcard icmp 1.1.1.1 2.2.2.2"<<endl;
		printf("firewall dnat ip ret = %d\n", ret);
		fprintf(fp,"firewall dnat ip ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"fwNatDnatPort");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.proto ,"tcp");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.sport ,"88");
		strcpy(natrules.dip ,"2.2.2.2");
		strcpy(natrules.dport ,"8888");
		ret = aos_firewall_add_nat(&natrules);
		cout<<"firewall dnat port outcard tcp 1.1.1.1 88 2.2.2.2 8888"<<endl;
		printf("firewall dnat port ret = %d\n", ret);
		fprintf(fp,"firewall dnat port ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"fwNatRedi");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.proto ,"tcp");
		strcpy(natrules.fromport ,"333");
		strcpy(natrules.toport ,"8888");
		ret = aos_firewall_add_nat(&natrules);
		cout<<"firewall redi outcard tcp 333 8888"<<endl;
		printf("firewall redi ret = %d\n", ret);
		fprintf(fp,"firewall redi ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"webwallRedi");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.fromport ,"333");
		strcpy(natrules.toport ,"8888");
		strcpy(natrules.dip ,"2.2.2.2");
		ret = aos_firewall_add_nat(&natrules);
		cout<<"webwall redi outcard 333 8888 2.2.2.2"<<endl;
		printf("webwall redi ret = %d\n", ret);
		fprintf(fp,"webwall redi ret = %d\n", ret);
		printf("-------------------------------------\n");
   total_case++;
    if(ret!=0) fail_case++;
        
		strcpy(natrules.type ,"webwallDnat");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.fromport ,"333");
		strcpy(natrules.toport ,"8888");
		strcpy(natrules.dip ,"2.2.2.2");
		strcpy(natrules.to ,"3.3.3.3");
		ret = aos_firewall_add_nat(&natrules);
		cout<<"webwall dnat outcard 333 8888 2.2.2.2 3.3.3.3"<<endl;
		printf("webwall dnat ret = %d\n", ret);
		fprintf(fp,"webwall dnat ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rslt,"");
		ret = aos_firewall_retrieve_config(rslt, FW_MAX_SHOW_CONFIG);
		printf("firewall show config ret = %d\n%s\n", ret, rslt);
		fprintf(fp,"firewall show config ret = %d\n%s\n", ret, rslt);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
    
		


		
		//	struct fwNatListEntry natrules;
		strcpy(natrules.type ,"fwNatSnat");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.smask ,"255.255.255.255");
		strcpy(natrules.to ,"2.2.2.2");
		ret = aos_firewall_del_nat(&natrules);
		printf("firewall snat ret del= %d\n", ret);
		fprintf(fp,"firewall snat ret del= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"fwNatMasq");
		strcpy(natrules.interface ,"incard");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.smask ,"255.255.255.255");
		ret = aos_firewall_del_nat(&natrules);
		printf("firewall masq ret del= %d\n", ret);
		fprintf(fp,"firewall masq ret del= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"fwNatDnatIP");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.proto ,"icmp");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.dip ,"2.2.2.2");
		ret = aos_firewall_del_nat(&natrules);
		printf("firewall dnat ip ret del= %d\n", ret);
		fprintf(fp,"firewall dnat ip ret del= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"fwNatDnatPort");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.proto ,"tcp");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.sport ,"88");
		strcpy(natrules.dip ,"2.2.2.2");
		strcpy(natrules.dport ,"8888");
		ret = aos_firewall_del_nat(&natrules);
		printf("firewall dnat port ret = %d\n", ret);
		fprintf(fp,"firewall dnat port ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"fwNatRedi");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.proto ,"tcp");
		strcpy(natrules.fromport ,"333");
		strcpy(natrules.toport ,"8888");
		ret = aos_firewall_del_nat(&natrules);
		printf("firewall redi ret = %d\n", ret);
		fprintf(fp,"firewall redi ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"webwallRedi");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.fromport ,"333");
		strcpy(natrules.toport ,"8888");
		strcpy(natrules.dip ,"2.2.2.2");
		ret = aos_firewall_del_nat(&natrules);
		printf("webwall redi ret = %d\n", ret);
		fprintf(fp,"webwall redi ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(natrules.type ,"webwallDnat");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.fromport ,"333");
		strcpy(natrules.toport ,"8888");
		strcpy(natrules.dip ,"2.2.2.2");
		strcpy(natrules.to ,"3.3.3.3");
		ret = aos_firewall_del_nat(&natrules);
		printf("webwall dnat ret = %d\n", ret);
		fprintf(fp,"webwall dnat ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		strcpy(rslt,"");
		ret = aos_firewall_retrieve_config(rslt, FW_MAX_SHOW_CONFIG);
		printf("firewall show config ret = %d\n%s\n", ret, rslt);
		fprintf(fp,"firewall show config ret = %d\n%s\n", ret, rslt);
		printf("-------------------------------------\n");
total_case++;
    if(ret!=0) fail_case++;
   
		// wrong input
		cout<<"========================= Wrong rules added  test section ========================="<<endl;
		cout<<"======================================================================================"<<endl;
		strcpy(natrules.type ,"webwallDnat");
		strcpy(natrules.interface ,"eth1");
		strcpy(natrules.fromport ,"333");
		strcpy(natrules.toport ,"8888");
		strcpy(natrules.dip ,"2.2.2.2");
		strcpy(natrules.to ,"3.3.3.3");
		ret = aos_firewall_del_nat(&natrules);
		printf("webwall dnat WRONG DEL ret = %d\n", ret);
		fprintf(fp,"webwall dnat WRONG DEL = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret==0) fail_case++;
   
		strcpy(rules.type, "fwTimefil");
		strcpy(rules.proto 	,"tcpp");
		strcpy(rules.chain 	,"in");
		strcpy(rules.sip 	,"1.1.1.1");
		strcpy(rules.smask 	,"255.255.255.0");
		strcpy(rules.dip 	,"2.2.2.2");
		strcpy(rules.dmask 	,"255.255.255.0");
		strcpy(rules.beginTime ,"00:01");
		strcpy(rules.endTime ,"10:01");
		strcpy(rules.week ,"Mon");
		strcpy(rules.action ,"permit");
		strcpy(rules.log 	,"nolog");
		ret = aos_firewall_del_filter(&rules);
		printf("firewall timefil WRONG DEL ret = %d\n", ret);
		fprintf(fp,"firewall timefil WRONG DEL ret = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret==0) fail_case++;
   

		strcpy(rules.type, "fwIpfilTcp");
		strcpy(rules.proto 	,"tcpp");
		strcpy(rules.chain 	,"in");
		strcpy(rules.sip 	,"1.1.1.1");
		strcpy(rules.smask 	,"255.255.255.0");
		strcpy(rules.sport 	,"1111");
		strcpy(rules.dip 	,"2.2.2.");
		strcpy(rules.dmask 	,"255.255.255.0");
		strcpy(rules.dport 	,"11111");
		strcpy(rules.action ,"permit");
		strcpy(rules.log 	,"nolog");
		ret = aos_firewall_add_filter(&rules);
		printf("firewall ipfil tcp ret ADD WRONG= %d\n", ret);
		fprintf(fp,"firewall ipfil tcp ret ADD WRONG = %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret==0) fail_case++;
   

		strcpy(rules.type ,"fwMacfilAll");
		strcpy(rules.proto ,"all");
		strcpy(rules.chain ,"in");
		strcpy(rules.smac ,"11:22:33:44:55:6-");
		strcpy(rules.dip ,"2.2.2.2");
		strcpy(rules.dmask ,"255.255.255.255");
		strcpy(rules.action ,"permit");
		strcpy(rules.log ,"nolog");
		ret = aos_firewall_add_filter(&rules);
		printf("firewall macfil all ret  ADD WRONG= %d\n", ret);
		fprintf(fp,"firewall macfil all ret ADD WRONG= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret==0) fail_case++;
   

		strcpy(natrules.type ,"fwNatSnat");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.smask ,"255.255.255.255");
		strcpy(natrules.to ,"2.2.2.342");
		ret = aos_firewall_add_nat(&natrules);
		printf("firewall snat ret ADD WRONG= %d\n", ret);
		fprintf(fp,"firewall snat ret ADD WRONG= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret==0) fail_case++;
   
		
		strcpy(natrules.type ,"fwNatDnatIP");
		strcpy(natrules.interface ,"outcad");
		strcpy(natrules.proto ,"icmp");
		strcpy(natrules.sip ,"1.1.1.1");
		strcpy(natrules.dip ,"2.2.2.2");
		ret = aos_firewall_add_nat(&natrules);
		printf("firewall dnat ip ret ADD WRONG= %d\n", ret);
		fprintf(fp,"firewall dnat ip ret ADD WRONG= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret==0) fail_case++;
   
		strcpy(natrules.type ,"webwallRedi");
		strcpy(natrules.interface ,"outcard");
		strcpy(natrules.fromport ,"333");
		strcpy(natrules.toport ,"8-88");
		strcpy(natrules.dip ,"2.2.2.2");
		ret = aos_firewall_add_nat(&natrules);
		printf("webwall redi ret ADD WRONG= %d\n", ret);
		fprintf(fp,"webwall redi ret ADD WRONG= %d\n", ret);
		printf("-------------------------------------\n");
total_case++;
    if(ret==0) fail_case++;
   
	
   
		fclose(fp);
		count++;
	}
	
	
	
	cout<<"================================FIREWALL APIS TEST RESULTS SUMMARY ===================================="<<endl;
	printf("the run total count is:%d\n ",count);
	cout<<"total run testcase:"<<total_case<<endl;
	cout<<"run total testcase:"<<fail_case<<endl;
//	printf("the differdence between the result files is: %d\n",system("diff result1 result2"));
	cout<<""<<endl;
	cout<<"========================================================================================================="<<endl;
	return 0;
}
