////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NmsClient.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_NmsClient_h
#define Omn_NMS_NmsClient_h

#include "Network/NetEtyType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/TcpClient.h"
#include "XmlParser/Ptrs.h"

class OmnSoMgcpEndpoint;

class OmnNmsClient : public virtual OmnRCObject,
					 public OmnTcpListener
{
	OmnDefineRCObject;

private:
	struct NmsClient
	{
		OmnNetEtyType::E	mType;
		OmnTcpClientPtr		mConn;

		NmsClient()
		:
		mType(OmnNetEtyType::eInvalidNetEntity)
		{
		}

		NmsClient(const OmnNetEtyType::E t, 
				  const OmnTcpClientPtr &c)
		:
		mType(t),
		mConn(c)
		{
		}
	};

	OmnVList<NmsClient>		mClients;
//	OmnTcpCltGrpPtr			mTcpClts;

public:
	OmnNmsClient();
	~OmnNmsClient();

	bool		start();
	bool		stop();
	bool		stopProg(const OmnString &name, OmnString &err);
	bool		config(const OmnXmlItemPtr &def);
	bool		readResp(const OmnString &name, 
						 const OmnTcpClientPtr &conn,
						 OmnString &err);
	bool		checkProgStatus(const OmnString &name, OmnString &rslt);
	bool		downloadAlgData(const int algId);

	OmnTcpClientPtr	getClientConn(const OmnString &name);
	OmnTcpClientPtr	getSrConn();
	bool		isProgDefined(const OmnString &name);

	virtual OmnString	getName() const {return "NmsClient";}
	virtual void    recvMsg(const OmnConnBuffPtr &buff,
						    const OmnTcpClientPtr &conn);


private:
	bool		addEndpoint(const OmnConnBuffPtr &buff, bool &finished);

	void		procNmsCmd(const OmnConnBuffPtr &, const OmnTcpClientPtr &);
	void		procEpCmd(const OmnConnBuffPtr &, const OmnTcpClientPtr &);

};
#endif
