////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GroupReliable.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_GroupReliable_h
#define Omn_UtilComm_GroupReliable_h

#include "Debug/Debug.h"
#include "Thread/Ptrs.h"
#include "UtilComm/CommGroup.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"


class OmnGroupReliable : public OmnCommGroup
{
	OmnDefineRCObject;

private:
	OmnCommPtr				mPrimary;
	OmnVList<OmnCommPtr>	mBackup;
	OmnMutexPtr				mBackupLock;

public:
	OmnGroupReliable(const OmnCommMgrPtr &commMgr, 
					 const OmnXmlItemPtr &def);
	~OmnGroupReliable();

	OmnRslt			config(const OmnXmlItemPtr &def);
	GroupType		getType() const {return eReliable;}
	bool			setPrimaryComm(const OmnCommPtr &ni);
	bool			addBackupComm(const OmnCommPtr &ni);
	bool			removeBackupComm(const int niid);

	virtual bool	commRestored(const OmnCommPtr &comm);
	bool			forward(const OmnConnBuffPtr &buff);

	OmnRslt			readFrom(OmnConnBuffPtr &buff,
							 const int timerSec,
							 const int timerMsec, 
							 bool &isTimeout);
	OmnRslt			sendMsgTo(const OmnMsgPtr &msg);
	OmnRslt			sendTo(const OmnConnBuffPtr &buff);
	OmnRslt			sendTo(const OmnString &data);
	OmnRslt			sendTo(const char *data, const int length);
	OmnRslt			sendTo(const char *data, 
						   const int length,
						   const OmnIpAddr &recvIpAddr,
						   const int recvPort);

    virtual OmnRslt startReading(const OmnCommListenerPtr &requester);
	virtual OmnRslt	stopReading(const OmnCommListenerPtr &requester);

	virtual int		getNiid() const;
	virtual OmnIpAddr	getLocalIpAddr() const;
	virtual int			getLocalPort() const;

	// 
	// Chen Ding, 05/10/2003
	//
	virtual OmnIpAddr		getRemoteAddr() const;
	virtual int				getRemotePort() const;
};
#endif

