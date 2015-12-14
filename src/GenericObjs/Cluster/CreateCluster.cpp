////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2014/12/08 Created by Barry Niu 
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/Cluster/CreateCluster.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "JQLParser/JQLParser.h"
#include "Jimo/Ptrs.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "SEUtil/ParmNames.h"

#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


static OmnMutex sgLock;
static int sgInited = false;
static map<OmnString, OmnString> sgTorturerNames;
static map<OmnString, OmnString> sgTorturerConfig;

static void sgInit()
{
	sgTorturerNames["xml_torturer"] = "xml_doc_test.exe";
	sgTorturerConfig["xml_torturer"] = "config_xml_doc_tester.txt";
}


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosMethodCreateCluster_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosMethodCreateCluster(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosMethodCreateCluster::AosMethodCreateCluster(const int version)
:
AosGenericMethod(version)
{
	if (!sgInited)
	{
		sgLock.lock();
		if (!sgInited)
		{
			sgInit();
		}
		sgLock.unlock();
	}
}


AosMethodCreateCluster::~AosMethodCreateCluster()
{
}


bool
AosMethodCreateCluster::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosMethodCreateCluster::cloneJimo() const
{
	return OmnNew AosMethodCreateCluster(*this);
}


bool 
AosMethodCreateCluster::proc(
		AosRundata *rdata,
		const OmnString &cluster_name, 
		const vector<AosGenericValueObjPtr> &parms)
{
	// This function create a cluster xml. 
	// jql:
	// create "cluster" "cluster_name"
	// (
	// 		ips:"xx1,xx2", 
	//  	user:"xx", 
	// 		install_home:"xxx", 
	// 		bkp_type:"xxx", 
	// 		virtuals:"5"
	// );
	//
	//	xml:
	//	<cluster >
	//		<ips>
	//			<ip><![CDATA[192.168.99.85]]></ip>
	//			<ip><![CDATA[192.168.99.86]]></ip>
	//		</ips>
	//		<tpls>
	//		       <item name="cluster"><![CDATA[config_cluster_tpl]]></item>
	//		       <item name="network"><![CDATA[config_network_tpl]]></item>
	//		       <item name="admin"><![CDATA[config_admin_tpl]]></item>
	//		       <item name="norm"><![CDATA[config_norm_tpl]]></item>
	//		       <item name="access"><![CDATA[config_access_tpl]]></item>
	//		       <item name="html"><![CDATA[config_html_tpl]]></item>
	//		       <item name="proxy"><![CDATA[config_proxy_tpl]]></item>
	//		</tpls>
	//		<install user="jozhi" user_home="" install_home="xxxx" 
	//			data_home="" bkp_type="xx" virtuals="1" script="xxx" 
	//			exe="xxx" shared_libs="xx" WebSite="xx" systemDocs="xx"/>
	//	</cluster>
	//
	

	int index = 0;
	mMap.clear();
	for (u32 i = 0; i < parms.size(); i++)
	{
		OmnString name = parms[i]->getName();
		mMap[name] = (int)i;
	}

	OmnString doc_str = "<cluster zky_objid=\"";
	doc_str << cluster_name << "\"> <ips> ";

	//ip...
	vector <OmnString> ips;

	itr_t itr = mMap.find("ips");
	if (itr == mMap.end()) return false;//did not find ips node
	index = itr->second;
	OmnString ips_values = parms[index]->getValue()->dump();
	if ( ips_values == "") return false;
	AosSplitStr(ips_values, ",", ips, 100);
	for (u32 i = 0; i < ips.size(); i++)
	{
		if (i == 0) ips[i] = removeLeadingQuotes(ips[i]);
		if (i == ips.size()-1) ips[i] = removeTailQuotes(ips[i]);
		doc_str << "<ip><![CDATA[" << ips[i] << "]]></ip>" ;
	}
	doc_str << "</ips> "
			<< "<tpls> <item name=\"cluster\"><![CDATA[config_cluster_tpl]]></item>"
			<< "<item name=\"network\"><![CDATA[config_network_tpl]]></item>"
			<< "<item name=\"admin\"><![CDATA[config_admin_tpl]]></item>"
			<< "<item name=\"norm\"><![CDATA[config_norm_tpl]]></item>"
			<< "<item name=\"access\"><![CDATA[config_access_tpl]]></item>"
			<< "<item name=\"html\"><![CDATA[config_html_tpl]]></item>"
			<< "<item name=\"proxy\"><![CDATA[config_proxy_tpl]]></item>"
			<< "</tpls>";
	
	//user..
	itr = mMap.find("user");
	if (itr == mMap.end()) return false; //did not find user attr 
	index = itr->second;
	OmnString user_value = parms[index]->getValue()->dump();
	if (user_value == "") return false;
	doc_str << "<install user=" << user_value;

	//pwd..
	itr = mMap.find("pwd");
	if (itr == mMap.end()) return false; //did not find pwd attr 
	index = itr->second;
	OmnString pwd_value = parms[index]->getValue()->dump();
	if (pwd_value == "") return false;
	doc_str << " pwd=" << pwd_value;

	//user_home...
	itr = mMap.find("user_home");
	if (itr != mMap.end())
	{
		index = itr->second;
		OmnString user_home_value = parms[index]->getValue()->dump();
		if (user_home_value != "") doc_str << " user_home=" << user_home_value;
	}

	//install_home
	itr = mMap.find("install_home");
	if (itr == mMap.end()) return false; //did not find install_home attr 
	index = itr->second;
	OmnString install_home_value = parms[index]->getValue()->dump();
	if (install_home_value == "") return false;
	doc_str << " install_home=" << install_home_value;

	//data_home...
	itr = mMap.find("data_home");
	if (itr != mMap.end())
	{
		index = itr->second;
		OmnString data_home_value = parms[index]->getValue()->dump();
		if (data_home_value != "") doc_str << " data_home=" << data_home_value;
	}

	//bkp_type
	itr = mMap.find("bkp_type");
	if (itr == mMap.end()) return false; //did not find bkp_type attr 
	index = itr->second;
	OmnString bkp_type_value = parms[index]->getValue()->dump();
	if (bkp_type_value == "") return false; //did not find bkp_type attr
	doc_str << " bkp_type=" << bkp_type_value;

	//virtuals
	itr = mMap.find("virtuals");
	if (itr == mMap.end()) return false; //did not find virtuals attr 
	index = itr->second;
	OmnString virtuals_value = parms[index]->getValue()->dump();
	if (virtuals_value == "") return false;
	doc_str << " virtuals=" << virtuals_value;

	//script
	itr = mMap.find("scripts");
	if (itr == mMap.end()) return false; //did not find script attr 
	index = itr->second;
	OmnString script_value = parms[index]->getValue()->dump();
	if (script_value == "") return false; 
	doc_str << " scripts=" << script_value;

	//exe
	itr = mMap.find("exe");
	if (itr == mMap.end()) return false; //did not find exe attr 
	index = itr->second;
	OmnString exe_value = parms[index]->getValue()->dump();
	if (exe_value == "") return false; 
	doc_str << " exe=" << exe_value;

	//shared_libs
	itr = mMap.find("shared_libs");
	if (itr == mMap.end()) return false; //did not find shared_libs attr 
	index = itr->second;
	OmnString shared_libs_value = parms[index]->getValue()->dump();
	if (shared_libs_value == "") return false; 
	doc_str << " shared_libs=" << shared_libs_value;

	//WebSite
	itr = mMap.find("WebSite");
	if (itr == mMap.end()) return false; //did not find WebSite attr 
	index = itr->second;
	OmnString WebSite_value = parms[index]->getValue()->dump();
	if (WebSite_value == "") return false; 
	doc_str << " WebSite=" << WebSite_value;

	//systemDocs
	itr = mMap.find("systemDocs");
	if (itr == mMap.end()) return false; //did not find systemDocs attr 
	index = itr->second;
	OmnString systemDocs_value = parms[index]->getValue()->dump();
	if (systemDocs_value == "") return false; 
	doc_str << " systemDocs=" << systemDocs_value;
	doc_str << " />"
			<< " </cluster>";

	AosXmlTagPtr doc = AosCreateDoc(doc_str, true, rdata);
	return true;
}


OmnString 
AosMethodCreateCluster::removeLeadingQuotes(const OmnString &str)
{
	int len = str.length();
	const char *data = str.data();

	if (data[0] != '"') return str;
	return str.substr(1, len-1);
}
		

OmnString
AosMethodCreateCluster::removeTailQuotes(const OmnString &str)
{
	int len = str.length();
	const char *data = str.data();

	if (data[len-1] != '"') return str;
	return str.substr(0, len-2);
}

