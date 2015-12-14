////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BouncerMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_BouncerMgr_BouncerMgr_h
#define Aos_BouncerMgr_BouncerMgr_h

#include "BouncerMgr/Ptrs.h"

#include "BouncerMgr/BouncerConn.h"
#include "BouncerMgr/BouncerEntry.h"
#include "CliClient/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util1/Ptrs.h"
#include "UtilComm/Ptrs.h"
//#include "TcpAppServer/Ptrs.h"
//#include "UtilComm/TcpMsgReader.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(AosBouncerMgrSingleton, 
						AosBouncerMgr,
						AosBouncerMgrSelf,
						OmnSingletonObjId::eBouncerMgr, 
						"BouncerMgr");

class AosBouncerMgr: public OmnRCObject
{
	OmnDefineRCObject;
private:
	AosCliClientPtr 				mCliClient;

public:
	OmnVList<AosBouncerEntryPtr>	mBouncerEntryList;
    OmnMutexPtr         			mLock;
//	AosBandWidthMgrPtr				mBandWidthMgr;
	
public:
	AosBouncerMgr();
	~AosBouncerMgr();

	static AosBouncerMgr *	getSelf();

	bool				config(const OmnXmlParserPtr &conf);

	bool				start();
	bool				stop();
	
	bool				setCliClient(const AosCliClientPtr cliClient);

	//
	// CLI callback functions
	//
	static 			int addBouncer(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	static 			int removeBouncer(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	static 			int setPinholeAttack(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	static 			int setBandWidth(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	static 			int getBandWidth(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	static 			int resetStat(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	bool			registerCliCallbackFunc();

	

	void			setStartFlag(const bool flag);
	void			setRespFlag(const bool flag);

	int 			addBouncer(const OmnString &protocol,
						  const OmnIpAddr &senderIp,
						  const int		  &senderPort,
						  const OmnIpAddr &recverIp,
						  const int		  &recverPort,
						  OmnString		  &resp);

	int 			removeBouncer(const OmnString &protocol,
						  const OmnIpAddr &senderIp,
						  const int		  &senderPort,
						  const OmnIpAddr &recverIp,
						  const int		  &recverPort,
						  OmnString		  &resp);

	int 			getBandWidth(const OmnString &protocol,
						  const OmnIpAddr &senderIp,
						  const int		  &senderPort,
						  const OmnIpAddr &recverIp,
						  const int		  &recverPort,
						  OmnString		  &resp);

	int 			resetStat(const OmnString &protocol,
						  const OmnIpAddr &senderIp,
						  const int		  &senderPort,
						  const OmnIpAddr &recverIp,
						  const int		  &recverPort,
						  OmnString		  &resp);
private:
	

};
#endif
