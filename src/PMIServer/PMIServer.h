////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PMIServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_PMIServer_PMIServer_h
#define Aos_PMIServer_PMIServer_h

#include "aosUtil/CharPtree.h"
#include "CliClient/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "TcpAppServer/Ptrs.h"
#include "UtilComm/TcpMsgReader.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(AosPMIServerSingleton, 
						AosPMIServer,
						AosPMIServerSelf,
						OmnSingletonObjId::ePMIServer, 
						"PMIServer");

class AosPMIServer: public OmnTcpMsgReader
{
	OmnDefineRCObject;
public:	

	typedef unsigned long 		UINT32;	
	
	typedef struct {
		UINT32 length;
		UINT32 sum;
		UINT32 serial;
		UINT32 command;
		UINT32 content;
		UINT32 data[0];
	}	AMM_PROTO_HEADER;

	typedef struct {
		OmnString userOrig;
		OmnString userSys;
		OmnString userSn;
		OmnString srcOrig;
		OmnString srcSys;
		OmnString src;
		OmnString operation;
		//bool	operator == (AosPMIServer::AMM_MSG &rhs) const;
	}	AMM_MSG;

	typedef struct _amm_config {
		OmnString		AZN_C_ORG_NAME;
		OmnString		AZN_C_APPSYS_NAME;
		OmnString		src_orig_prefix;
		OmnString		src_sys_prefix ;
		OmnString		src_prefix     ;
		OmnString		opr_prefix     ;
	}	AMM_CONFIG;

private:
	AosTcpAppServerPtr			mServer;
	AMM_CONFIG					mAmmConfig;
	aosCharPtree *				mPermTree;
	AosCliClientPtr 			mCliClient;

	bool						mStartFlag;
	bool						mRespFlag;
	
public:
	AosPMIServer();
	~AosPMIServer();

	static AosPMIServer *	getSelf();

	virtual OmnString	getTcpMsgReaderName() const ;
	virtual int			nextMsg(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);
	bool				config(const OmnXmlParserPtr &conf);

	bool				start();
	bool				stop();
	
	bool				addPermission();
	bool				setCliClient(const AosCliClientPtr cliClient);

	//
	// CLI callback functions
	//
	static 			int startModule(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	static 			int stopModule(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	static 			int responseOn(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	static 			int responseOff(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	bool				registerCliCallbackFunc();

	

	void				setStartFlag(const bool flag);
	void				setRespFlag(const bool flag);


private:
	bool			procMsg(const char* msg,const int msgLen,const OmnTcpClientPtr &conn);
	bool			getCmd(const OmnConnBuffPtr &buff , OmnString &cmd);
	bool			sendResp(OmnString &rslt,const OmnTcpClientPtr &conn);
	
	bool			getMsgBody(AMM_MSG &msgBody,const char *msg,const int msgLen);
	bool			checkPermission(AMM_MSG &msgBody);
	bool			addPermission(AMM_MSG * msgBody);
	bool			getStringBy(OmnString &str,const char *msg,const int msgLen,const char stopchar);

	bool			equal_to_AMM_MSG(const AMM_MSG &msg1,const AMM_MSG &msg2);
	
	void			showAMMMsg(const AMM_MSG &msg);

};
#endif
