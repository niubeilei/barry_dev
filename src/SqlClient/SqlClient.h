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
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SqlClient_SqlClient_h
#define AOS_SqlClient_SqlClient_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SqlUtil/TableNames.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


OmnDefineSingletonClass(AosSqlClientSingleton,
						AosSqlClient,
						AosSqlClientSelf,
						OmnSingletonObjId::eSqlClient,
						"SqlClient");

class AosSqlClient : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosSqlClient();
	~AosSqlClient();

    // Singleton class interface
    static AosSqlClient *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// SqlClientObj Interface
	virtual bool setValue(
					const OmnString &container, 
					const OmnString &name,
					const OmnString &value,
					const AosRundataPtr &rdata);

	virtual bool setValue(	
					const OmnString &container, 
					const OmnString &name,
					const int64_t &value,
					const AosRundataPtr &rdata);

	virtual bool setValue(	
					const OmnString &container, 
					const OmnString &name,
					const u64 &value,
					const AosRundataPtr &rdata);

	virtual bool setValue(
					const OmnString &name,
					const OmnString &value,
					const AosRundataPtr &rdata);

	virtual bool setValue(
					const OmnString &name,
					const int64_t &value,
					const AosRundataPtr &rdata);

	virtual bool setValue(
					const OmnString &name,
					const u64 &value,
					const AosRundataPtr &rdata);

	virtual bool updateValue(
					const OmnString &container, 
					const OmnString &name,
					const OmnString &value,
					const AosRundataPtr &rdata);

	virtual bool updateValue(
					const OmnString &container, 
					const OmnString &name,
					const int64_t &value,
					const AosRundataPtr &rdata);

	virtual bool updateValue(
					const OmnString &container, 
					const OmnString &name,
					const u64 &value,
					const AosRundataPtr &rdata);

	virtual bool updateValue(
					const OmnString &name,
					const OmnString &value,
					const AosRundataPtr &rdata);

	virtual bool updateValue(
					const OmnString &name,
					const int64_t &value,
					const AosRundataPtr &rdata);

	virtual bool updateValue(
					const OmnString &name,
					const u64 &value,
					const AosRundataPtr &rdata);

	virtual bool getValue(
					const OmnString &container, 
					const OmnString &name,
					OmnString &value,
					const OmnString &dft,
					const AosRundataPtr &rdata);

	virtual bool getValue(
					const OmnString &container, 
					const OmnString &name,
					int64_t &value,
					const int64_t &dft,
					const AosRundataPtr &rdata);

	virtual bool getValue(
					const OmnString &container, 
					const OmnString &name,
					u64 &value,
					const u64 &dft,
					const AosRundataPtr &rdata);

	virtual bool getValue(
					const OmnString &name,
					OmnString &value,
					const OmnString &dft,
					const AosRundataPtr &rdata);

	virtual bool getValue(
					const OmnString &name,
					int64_t &value,
					const int64_t &dft,
					const AosRundataPtr &rdata);

	virtual bool getValue(
					const OmnString &name,
					u64 &value,
					const u64 &dft,
					const AosRundataPtr &rdata);
};
#endif
