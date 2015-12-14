////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliClient.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliClient_CliClient_h
#define Omn_CliClient_CliClient_h


#include "Alarm/Alarm.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/CharPtree.h"
#include "Debug/Rslt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpMsgReader.h"
#include "XmlParser/Ptrs.h"

//struct aosCharPtree;
class OmnCliCmd; 

class AosCliClient : public OmnThreadedObj, public OmnTcpMsgReader
{
	OmnDefineRCObject;

	enum 
	{
		eAosCliClient_thread_waiting_time = 5,
		eCliFuncErrMsgLen = 100,
		eMaxIntArgs = 10,
		eMaxStrArgs = 10,
		eTcpWaitTime = 5
	};

private:
	
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr			mThread;

	OmnTcpClientPtr		mClientConn;
	OmnTcpClientPtr		mNewClientConn;

	aosCharPtree *		mCmdTree;

	aosCharPtree *		mCallbackTree;
	OmnString			mFilename;

	OmnString			mHbMsg;
public:
	AosCliClient();
	~AosCliClient();

	bool		start();
	bool		stop();
	OmnRslt		config(const OmnXmlParserPtr &conf);

	// OmnThreadedObj abstruct functions
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// OmnTcpMsgReader abstruct functions
	virtual OmnString	getTcpMsgReaderName() const;
	virtual int			nextMsg(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);

	bool			setTcpClient(OmnXmlItemPtr &client_config);
	static int 		convertParam(char *data,
				 			unsigned int datalen,
				 			struct aosKernelApiParms *parms);
	bool		addCliFunc(const aosCliFunc &func , const OmnString &opr_id);
		
private:
	bool		addCmd(const OmnString &def);
	bool		addCmds(const OmnString &filename);
	bool		procCli(const OmnConnBuffPtr msg);
	bool 	runCli(const OmnConnBuffPtr &cmd, OmnString &rslt);
	bool		trimEmptyLine(const OmnConnBuffPtr msg);
	bool		runApp(const OmnString &parms, 
				    char **data, 
				    unsigned int *length);

};

#endif

