////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OCSPServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_OCSPServer_OCSPServer_h
#define Aos_OCSPServer_OCSPServer_h

#include "aosUtil/CharPtree.h"
#include "CliClient/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "TcpAppServer/Ptrs.h"
#include "UtilComm/TcpMsgReader.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(AosOCSPServerSingleton, 
						AosOCSPServer,
						AosOCSPServerSelf,
						OmnSingletonObjId::eOCSPServer, 
						"OCSPServer");

class AosOCSPServer: public OmnTcpMsgReader
{
	OmnDefineRCObject;
public:	
	enum 
	{
		eAosOCSPSvr_MaxMsgLength = 3000,
		eAosOCSPSvr_MaxStrLength = 1000,
		eAosOCSPSvr_MaxCrlFileNum = 200
	};

	enum eAosOCSPSvr_RespType
	{
		eAosOCSPSvr_JNS,
		eAosOCSPSvr_Standard,
		eAosOCSPSvr_EasyMode
	};

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
		int 		pvno;
		OmnString 	nonce;	
		OmnString 	OCSQS_Name;
		OmnString 	transID;
		OmnString 	time;
		int			status;
		OmnString 	nonce2; 
		int			nonce2BitNum;
		OmnString 	SN;
	}	OCSP_MSG;


	typedef struct {
		OmnString userOrig;
		OmnString userSys;
		OmnString userSn;
		OmnString srcOrig;
		OmnString srcSys;
		OmnString src;
		OmnString operation;
	}	OCSP_RESP_MSG;

	typedef struct {
		int 		no;
		OmnString 	fileName;	
	}	CRL_FILE_RECORD;

private:
	AosTcpAppServerPtr			mServer;
	aosCharPtree *				mPermTree;
	AosCliClientPtr 			mCliClient;

	bool						mStartFlag;
	bool						mRespFlag;
	eAosOCSPSvr_RespType		mRespType;

	static CRL_FILE_RECORD		mCrlFileRecords[eAosOCSPSvr_MaxCrlFileNum];
	static int					mCrlFileNum;
	static OmnString 			mCRLFilePos;

public:
	AosOCSPServer();
	~AosOCSPServer();

	static AosOCSPServer *	getSelf();

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

	static			int changeCrlList(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);

	bool				registerCliCallbackFunc();

	

	bool			setJNSRespType();
	bool			setStandardRespType();
	bool			setEasyModeRespType();


private:
	bool			procMsg(char* msg,const int msgLen,const OmnTcpClientPtr &conn);
	bool			getCmd(const OmnConnBuffPtr &buff , OmnString &cmd);
	bool			sendResp(OmnString &rslt,const OmnTcpClientPtr &conn);
	
	bool			parseReq(OCSP_MSG &msgBody,char *msg,const unsigned int msgLen);
	bool			checkPermission(OCSP_MSG &ocspMsg);
	bool			addPermission(OmnString  *sn);
	
	bool			getStringBy(OmnString &str,const char *msg,const int msgLen,const char stopchar);

	void			setStartFlag(const bool flag);
	void			setRespFlag(const bool flag);
	bool			createRespMsg(const OCSP_MSG ocspMsg,const bool permission,OmnString &strOcspResp);

	bool			addCRLFile(const int no,const OmnString &fileName);
};
#endif
