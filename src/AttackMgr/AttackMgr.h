////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AttackMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AttackMgr_AttackMgr_h
#define Aos_AttackMgr_AttackMgr_h

#include "AttackMgr/Ptrs.h"
#include "aosUtil/CharPtree.h"
#include "CliClient/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
//#include "TcpAppServer/Ptrs.h"
//#include "UtilComm/TcpMsgReader.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(AosAttackMgrSingleton, 
						AosAttackMgr,
						AosAttackMgrSelf,
						OmnSingletonObjId::eAttackMgr, 
						"AttackMgr");

class AosAttackMgr: public OmnRCObject
{
	OmnDefineRCObject;
public:	

private:
	AosCliClientPtr 			mCliClient;

	OmnVList<AosAttackerPtr>	mAttackList;
	
public:
	AosAttackMgr();
	~AosAttackMgr();

	static AosAttackMgr *	getSelf();

	bool				config(const OmnXmlParserPtr &conf);

	bool				start();
	bool				stop();
	
	bool				setCliClient(const AosCliClientPtr cliClient);

	//
	// CLI callback functions
	//
	static 			int addAttacker(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	static 			int removeAttacker(char *data, 
									 unsigned int *optlen, 
									 struct aosKernelApiParms *parms, 
									 char *errmsg, 
									 const int errlen);
	bool				registerCliCallbackFunc();

	

	void				setStartFlag(const bool flag);
	void				setRespFlag(const bool flag);


private:
	

};
#endif
