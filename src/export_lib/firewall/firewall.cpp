#include "aosApi.h"
#include "common.h"

int aos_firewall_retrieve_config(char* buf, int bufsize)
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("firewall show config", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	strncpy(buf, rslt, bufsize-1);
	buf[bufsize-1] = 0;
	return *((int*)tmp);	
}

int aos_firewall_init(u8 level)
{
	OmnString rslt = "";
	char* tmp;

	if (level == 0)
		OmnCliProc::getSelf()->runCliAsClient("firewall minsec init", rslt);
	else if (level == 1)
		OmnCliProc::getSelf()->runCliAsClient("firewall midsec init", rslt);
	else
		OmnCliProc::getSelf()->runCliAsClient("firewall maxsec init", rslt);
	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_clear_config()
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("firewall clear config", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_vpn_pass_ipsec(u8 status)
{
	OmnString rslt = "";
	char* tmp;

	if (status == 1)
		OmnCliProc::getSelf()->runCliAsClient("firewall ipsec_pass on", rslt);
	else
		OmnCliProc::getSelf()->runCliAsClient("firewall ipsec_pass off", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_syncookie(u8 status)
{
	OmnString rslt = "";
	char* tmp;

	if (status == 1)
		OmnCliProc::getSelf()->runCliAsClient("firewall set syncookie on", rslt);
	else
		OmnCliProc::getSelf()->runCliAsClient("firewall set syncookie off", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_vpn_pass_l2tp(u8 status)
{
	OmnString rslt = "";
	char* tmp;

	if (status == 1)
		OmnCliProc::getSelf()->runCliAsClient("firewall l2tp_pass on", rslt);
	else
		OmnCliProc::getSelf()->runCliAsClient("firewall l2tp_pass off", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_outcard_ping(u8 status)
{
	OmnString rslt = "";
	char* tmp;

	if (status == 1)
		OmnCliProc::getSelf()->runCliAsClient("firewall outcard ping  on", rslt);
	else
		OmnCliProc::getSelf()->runCliAsClient("firewall outcard ping off", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_vpn_pass_pptp(u8 status)
{
	OmnString rslt = "";
	char* tmp;

	if (status == 1)
		OmnCliProc::getSelf()->runCliAsClient("firewall pptp_pass on", rslt);
	else
		OmnCliProc::getSelf()->runCliAsClient("firewall pptp_pass off", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}

int aos_firewall_anti_syn_add(char *dip, u32 dport, u32 rate)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;
	char prefix[1024];

	sprintf(prefix, "firewall antiattack syn %s %lu %lu", dip, dport, rate);
		
    sendCmd << prefix;
	//printf("%s\n", sendCmd.getBuffer());
	OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_anti_syn_del(char *dip, u32 dport, u32 rate)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;
	char prefix[1024];

	sprintf(prefix, "firewall antiattack del syn %s %lu %lu", dip, dport, rate);

    sendCmd << prefix;
	OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_anti_icmp_add(char *dip, u32 rate)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;
	char prefix[1024];

	sprintf(prefix, "firewall antiattack icmp %s %lu", dip, rate);
	
    sendCmd << prefix;
	OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_anti_icmp_del(char *dip, u32 rate)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;
	char prefix[1024];

	sprintf(prefix, "firewall antiattack del icmp %s %lu", dip, rate);
	
    sendCmd << prefix;
	OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_blacklist_file_add(char *fname)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;
	char prefix[1024];

	sprintf(prefix, "firewall blacklist load file %s", fname);
	
    sendCmd << prefix;
	OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_blacklist_file_del(char *fname)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;
	char prefix[1024];

	sprintf(prefix, "firewall blacklist remove file %s", fname);
	
    sendCmd << prefix;
	OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_blacklist_url_add(char *url)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;
	char prefix[1024];

	sprintf(prefix, "firewall blacklist load url %s", url);

    sendCmd << prefix;
	OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_blacklist_url_del(char *url)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;
	char prefix[1024];

	sprintf(prefix, "firewall blacklist remove url %s", url);

    sendCmd << prefix;
	OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_add_filter(struct fwFilterListEntry *rules)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;

	if ((strcmp(rules->type, "fwIpfilAll") == 0 || strcmp(rules->type, "fwIpfilIcmp") == 0))
	{
		sendCmd = "firewall ";
		sendCmd << "ipfil " << rules->proto << " " 
			<< rules->chain << " " 
			<< rules->sip << " " << rules->smask << " " 
			<< rules->dip << " " << rules->dmask << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwIpfilStateNew") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "ipfil state new " << rules->chain << " " 
			<< rules->sip << " " << rules->smask << " " 
			<< rules->dip << " " << rules->dmask << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwIpfilStateAck") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "ipfil state ack " << rules->chain << " " 
			<< rules->sip << " " << rules->smask << " " 
			<< rules->dip << " " << rules->dmask << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if ((strcmp(rules->type, "fwIpfilTcp") == 0 || strcmp(rules->type, "fwIpfilUdp") == 0))
	{
		sendCmd = "firewall ";
		sendCmd << "ipfil " << rules->proto << " " 
			<< rules->chain << " " 
			<< rules->sip << " " << rules->smask << " " << rules->sport << " " 
			<< rules->dip << " " << rules->dmask << " " << rules->dport << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if ((strcmp(rules->type, "fwMacfilAll") == 0 || strcmp(rules->type, "fwMacfilIcmp") == 0))
	{
		sendCmd = "firewall ";
		sendCmd << "macfil " << rules->proto << " " 
			<< rules->chain << " " 
			<< rules->smac << " " 
			<< rules->dip << " " << rules->dmask << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if ((strcmp(rules->type, "fwMacfilTcp") == 0 || strcmp(rules->type, "fwMacfilUdp") == 0))
	{
		sendCmd = "firewall ";
		sendCmd << "macfil " << rules->proto << " " 
			<< rules->chain << " " 
			<< rules->smac << " " << rules->sport << " " 
			<< rules->dip << " " << rules->dmask << " " << rules->dport << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}	
	else if (strcmp(rules->type, "fwAntiSyn") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "antiattack syn " << rules->dip << " " 
			<< rules->dport << " " 
			<< rules->rate << " "; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwAntiIcmp") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "antiattack icmp " << rules->dip << " " 
			<< rules->rate << " "; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwTimefil") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "timefil " << " " << rules->chain << " "
			<< rules->proto << " " 
			<< rules->sip << " " << rules->smask << " "  
			<< rules->dip << " " << rules->dmask << " " 
			<< rules->action << " " 
			<< rules->beginTime << " " 
			<< rules->endTime << " " 
			<< rules->week << " " 
			<< rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwBlacklistFile") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "blacklist load file " << rules->proto; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwBlacklistUrl") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "blacklist load url " << rules->proto; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else
	{
		printf("The Filter type is wrong!\n");
	}

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}

int aos_firewall_add_nat(struct fwNatListEntry *rules)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;

	if (strcmp(rules->type, "fwNatSnat") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "snat " << rules->interface << " " 
			<< rules->sip << " " << rules->smask << " " 
			<< rules->to; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwNatMasq") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "masq " << rules->interface << " " 
			<< rules->sip << " " << rules->smask << " "; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwNatDnatIP") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "dnat ip " << rules->interface << " "
		    << rules->proto << " "	
			<< rules->sip << " " 
			<< rules->dip;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwNatDnatPort") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "dnat port " << rules->interface << " "
		    << rules->proto << " "	
			<< rules->sip << " " 
			<< rules->sport << " "
			<< rules->dip << " " << rules->dport;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwNatRedi") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "redi " << rules->interface << " " 
			<< rules->proto << " " 
			<< rules->fromport << " " << rules->toport;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "webwallRedi") == 0)
	{
		sendCmd = "webwall ";
		sendCmd << "redi " << rules->interface << " " 
			<< rules->fromport << " " << rules->toport << " "
			<< rules->dip;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "webwallDnat") == 0)
	{
		sendCmd = "webwall ";
		sendCmd << "dnat " << rules->interface << " " 
			<< rules->fromport << " " << rules->toport << " "
			<< rules->dip << " " << rules->to;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else
	{
		printf("The Nat type is wrong!\n");
	}

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_del_filter(struct fwFilterListEntry *rules)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;

	if ((strcmp(rules->type, "fwIpfilAll") == 0 || strcmp(rules->type, "fwIpfilIcmp") == 0))
	{
		sendCmd = "firewall ";
		sendCmd << "ipfil del " << rules->proto << " " 
			<< rules->chain << " " 
			<< rules->sip << " " << rules->smask << " " 
			<< rules->dip << " " << rules->dmask << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwIpfilStateNew") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "ipfil del state new " << rules->chain << " " 
			<< rules->sip << " " << rules->smask << " " 
			<< rules->dip << " " << rules->dmask << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwIpfilStateAck") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "ipfil del state ack " << rules->chain << " " 
			<< rules->sip << " " << rules->smask << " " 
			<< rules->dip << " " << rules->dmask << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if ((strcmp(rules->type, "fwIpfilTcp") == 0 || strcmp(rules->type, "fwIpfilUdp") == 0))
	{
		sendCmd = "firewall ";
		sendCmd << "ipfil del " << rules->proto << " " 
			<< rules->chain << " " 
			<< rules->sip << " " << rules->smask << " " << rules->sport << " " 
			<< rules->dip << " " << rules->dmask << " " << rules->dport << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if ((strcmp(rules->type, "fwMacfilAll") == 0 || strcmp(rules->type, "fwMacfilIcmp") == 0))
	{
		sendCmd = "firewall ";
		sendCmd << "macfil del " << rules->proto << " " 
			<< rules->chain << " " 
			<< rules->smac << " " 
			<< rules->dip << " " << rules->dmask << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if ((strcmp(rules->type, "fwMacfilTcp") == 0 || strcmp(rules->type, "fwMacfilUdp") == 0))
	{
		sendCmd = "firewall ";
		sendCmd << "macfil del " << rules->proto << " " 
			<< rules->chain << " " 
			<< rules->smac << " " << rules->sport << " " 
			<< rules->dip << " " << rules->dmask << " " << rules->dport << " "
			<< rules->action << " " << rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}	
	else if (strcmp(rules->type, "fwAntiSyn") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "antiattack del syn " << rules->dip << " " 
			<< rules->dport << " " 
			<< rules->rate << " "; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwAntiIcmp") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "antiattack del icmp " << rules->dip << " " 
			<< rules->rate << " "; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwTimefil") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "del timefil " << " " << rules->chain << " "
			<< rules->proto << " " 
			<< rules->sip << " " << rules->smask << " "  
			<< rules->dip << " " << rules->dmask << " " 
			<< rules->action << " " 
			<< rules->beginTime << " " 
			<< rules->endTime << " " 
			<< rules->week << " " 
			<< rules->log;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwBlacklistFile") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "blacklist remove file " << rules->proto; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwBlacklistUrl") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "blacklist remove url " << rules->proto; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else
	{
		printf("The Filter type is wrong!\n");
	}

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}

int aos_firewall_del_nat(struct fwNatListEntry *rules)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;

	if (strcmp(rules->type, "fwNatSnat") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "del snat " << rules->interface << " " 
			<< rules->sip << " " << rules->smask << " " 
			<< rules->to; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwNatMasq") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "del masq " << rules->interface << " " 
			<< rules->sip << " " << rules->smask << " "; 
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwNatDnatIP") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "del dnat ip " << rules->interface << " "
		    << rules->proto << " "	
			<< rules->sip << " " 
			<< rules->dip;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwNatDnatPort") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "del dnat port " << rules->interface << " "
		    << rules->proto << " "	
			<< rules->sip << " " 
			<< rules->sport << " "
			<< rules->dip << " " << rules->dport;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "fwNatRedi") == 0)
	{
		sendCmd = "firewall ";
		sendCmd << "del redi " << rules->interface << " " 
			<< rules->proto << " " 
			<< rules->fromport << " " << rules->toport;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "webwallRedi") == 0)
	{
		sendCmd = "webwall ";
		sendCmd << "del redi " << rules->interface << " " 
			<< rules->fromport << " " << rules->toport << " "
			<< rules->dip;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else if (strcmp(rules->type, "webwallDnat") == 0)
	{
		sendCmd = "webwall ";
		sendCmd << "del dnat " << rules->interface << " " 
			<< rules->fromport << " " << rules->toport << " "
			<< rules->dip << " " << rules->to;
		OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);
	}
	else
	{
		printf("The Nat type is wrong!\n");
	}

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}


int aos_firewall_antiattack_outcard(char *dip, u32 rate)
{
	OmnString rslt = "";
	OmnString sendCmd;
	char* tmp;
	char prefix[1024];

	sprintf(prefix, "firewall antiattack outcard %s %lu", dip, rate);
		
    sendCmd << prefix;
	//printf("%s\n", sendCmd.getBuffer());
	OmnCliProc::getSelf()->runCliAsClient(sendCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}



