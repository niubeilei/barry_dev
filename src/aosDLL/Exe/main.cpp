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
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "aosDLL/DllMgr.h"
#include "Actions/AllActions.h"
#include "DataGenSeqno/AllDataGenSeqno.h"
#include "SeReqProc/AllSeReqProcs.h"
#include "IILTrans/AllIILTrans.h"
#include "Alarm/Alarm.h"
#include "Actions/SdocActionObj.h"
#include "AppMgr/App.h"
#include "CounterServer/Ptrs.h"
#include "CounterServer/VirCtnrSvr.h"
#include "CounterClt/CounterClt.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/AllCounterProc.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "DataStore/StoreMgr.h"
#include "DbQuery/Query.h"
#include "Debug/Debug.h"
#include "DocProc/DocProc.h"
#include "DocClient/DocClient.h"
#include "DocMgr/Ptrs.h"
#include "DocMgr/DocMgr.h"
#include "DocSelector/DocSelObjImpl.h"
#include "DocSelector/Torturer/DocSelRandUtil.h"
#include "DocServer/DocSvrConnMgr.h"
#include "DocServer/AllDocSvrProcs.h"
#include "MsgClient/MsgClient.h"
#include "IdGen/IdGenMgr.h"
#include "Job/JobMgr.h"
#include "IILTrans/AllIILTrans.h"
#include "MultiLang/LangDictMgr.h"
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/AllTerms.h"
#include "QueryClient/QueryClient.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "RemoteBackupClt/RemoteBackupClt.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/FrontEndSvr.h"
#include "SEModules/SeRundata.h"
#include "SEModules/SiteMgr.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/SizeIdMgrObj.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEInterfaces/UserMgmtObj.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/LangDictObj.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/DocSelRandObj.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEInterfaces/CounterCltObj.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SecUtil/AllAccesses.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "SmartDoc/SmartDoc.h"
#include "StorageEngine/SizeIdMgr.h"
#include "StorageMgr/StorageMgr.h"
#include "SoapServer/SoapServer.h"
#include "TaskMgr/TaskMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "TransServer/Ptrs.h"
#include "IILTransServer/IILTransServer.h"
#include "IILTransServer/Ptrs.h"
#include "SeLogClient/SeLogClient.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "ShortMsgClt/ShortMsgClt.h"
#include "SmartDoc/SmartDocImpl.h"
#include "Util/FileWatcher.h"
#include "Util/MemoryChecker.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordTransClient/WordTransClient.h"
#include "WordParser/Ptrs.h"
#include "VersionServer/VersionServer.h"
#include "UserMgmt/UserMgmt.h"
#include "Python/Python.h"
#include "Conds/AllConds.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Thread.h"
#include "Timer/TimerMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>



int 
main(int argc, char **argv)
{
	// AosAppRundataPtr adata = OmnNew AosSeRundata();
	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");
	OmnApp::setFrontEndServer(true);

	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	cout << "Before calling the function: " << endl;
	AosDllMgr::getSelf()->callFun(rdata, "libTest.so", "ChenDingTest");
	cout << "After calling the function: " << endl;
	return 0;
}


