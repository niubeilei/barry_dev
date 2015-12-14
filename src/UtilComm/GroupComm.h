////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GroupComm.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_GroupComm_h
#define Omn_UtilComm_GroupComm_h

#include "Message/MsgId.h"
#include "Porting/WinSock2.h"
#include "Thread/Ptrs.h"
#include "Util/BasicTypes.h"
#include "UtilComm/CommGroup.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"


class OmnGroupComm : public OmnCommGroup
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxComms = 20
	};

	OmnVList<OmnCommPtr>	mComms;
	OmnMutexPtr				mCommLock;
	fd_set 					mReadFds; 		  
	fd_set					mWorkingFds;
	int 					mFdscnt;

public:
	OmnGroupComm(const OmnXmlItemPtr &def);
	~OmnGroupComm();

	OmnRslt			config(const OmnXmlItemPtr &def);

	GroupType		getType() const {return eGroupComm;}

	virtual bool	commRestored(const OmnCommPtr &comm);
    virtual OmnRslt startReading(const OmnCommListenerPtr &requester);
    virtual OmnRslt stopReading(const OmnCommListenerPtr &requester);

	virtual OmnRslt		readFrom(OmnConnBuffPtr &buff,
							const int timerMsec = -1);
	virtual OmnRslt		sendTo(const OmnMsgPtr &msg);
	virtual OmnRslt		sendTo(const OmnConnBuffPtr &buff);
	virtual OmnRslt		sendTo(const char *data, const int len);
	virtual OmnRslt		sendTo(const char *data, const int len,
							   const OmnIpAddr &recverIpAddr,
							   const int recverPort);
	virtual int			getNiid() const;
	virtual OmnIpAddr	getLocalIpAddr() const;
	virtual int			getLocalPort() const;

	int					getGroupId() const {return mGroupId;}
	OmnString			getName() const {return mName;}
	bool				removeComm(const int commId);
	bool				addComm(const OmnCommPtr &comm);
	OmnRslt				readFrom(OmnConnBuffPtr &buff, 
							 OmnCommPtr &comm);

private:
	bool			checkReading(OmnCommPtr &comm);
	bool			getEvent(OmnCommPtr &comm);
	void			checkConns();
};
#endif

