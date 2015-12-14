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
#include "GenericObjs/Cluster/InstallCluster.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "JQLParser/JQLParser.h"
#include "Jimo/Ptrs.h"

#include "JimoAgentMonitor/JimoInstall.h"
#include "JimoAgentMonitor/Ptrs.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "SEUtil/ParmNames.h"

#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


extern shared_ptr<AosJimoAPIClient> gThriftClient;  

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosMethodInstallCluster_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosMethodInstallCluster(version);
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


AosMethodInstallCluster::AosMethodInstallCluster(const int version)
:
AosGenericMethod(version)
{
}


AosMethodInstallCluster::~AosMethodInstallCluster()
{
}


bool
AosMethodInstallCluster::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosMethodInstallCluster::cloneJimo() const
{
	return OmnNew AosMethodInstallCluster(*this);
}


bool 
AosMethodInstallCluster::proc(
		AosRundata *rdata,
		const OmnString &cluster_name, 
		const vector<AosGenericValueObjPtr> &parms)
{
	// This function install jimodb. 
	// jql format: install cluster cluster_name
	string clustername = cluster_name.data();
	JmoRundata jimoRdata;
	JmoCallData call_data;
	gThriftClient->JimoInstall(jimoRdata, call_data, clustername); 
	if (jimoRdata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Install cluster '" << cluster_name  << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


/*
bool
AosJqlStmtGenericobj::installCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoInstall(mJmoRundata, mHandler, clustername); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Install cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::startCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "start"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Start cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::restartCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "restart"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Restart cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::cleanCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "clean"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Clean cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::stopAccessCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "stop_access"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Stop access cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::startAccessCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "start_access"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Start access cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::restartAccessCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "restart_access"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Restart access cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::stopHtmlCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "stop_html"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Stop html cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::startHtmlCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "start_html"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Start html cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::restartHtmlCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "restart_html"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Restart html cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::stopCluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "stop"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Stop cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


bool
AosJqlStmtGenericobj::stop9Cluster(const AosRundataPtr &rdata)
{
	aos_assert_r(mType == "cluster", false);
	aos_assert_r(mName != "", false);

	string clustername = mName.data();
	mClient->JimoOpr(mJmoRundata, mHandler, clustername, "stop 9"); 
	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS) 
	{
		OmnString msg;
		msg << "Stop 9 cluster '" << mName << "' successfully";
		rdata->setJqlMsg(msg);
		return true;
	}
	return false;
}


	bool 		installCluster(const AosRundataPtr &rdata);
	bool 		startCluster(const AosRundataPtr &rdata);
	bool 		stopCluster(const AosRundataPtr &rdata);
	bool 		stop9Cluster(const AosRundataPtr &rdata);
	bool 		restartCluster(const AosRundataPtr &rdata);
	bool 		cleanCluster(const AosRundataPtr &rdata);
	bool        stopAccessCluster(const AosRundataPtr &rdata);
	bool        startAccessCluster(const AosRundataPtr &rdata);
	bool        restartAccessCluster(const AosRundataPtr &rdata);
	bool        stopHtmlCluster(const AosRundataPtr &rdata);
	bool        startHtmlCluster(const AosRundataPtr &rdata);
	bool        restartHtmlCluster(const AosRundataPtr &rdata);
*/
