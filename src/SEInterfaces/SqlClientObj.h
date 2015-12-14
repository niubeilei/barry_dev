////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 02/27/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SqlClientObj_h
#define Aos_SEInterfaces_SqlClientObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosSqlClientObj : virtual public OmnRCObject
{
private:
	static AosSqlClientObjPtr		smSqlClient;

public:
	virtual bool setValue(
					const OmnString &container, 
					const OmnString &name,
					const OmnString &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool setValue(	
					const OmnString &container, 
					const OmnString &name,
					const int64_t &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool setValue(	
					const OmnString &container, 
					const OmnString &name,
					const u64 &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool setValue(
					const OmnString &name,
					const OmnString &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool setValue(
					const OmnString &name,
					const int64_t &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool setValue(
					const OmnString &name,
					const u64 &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool updateValue(
					const OmnString &container, 
					const OmnString &name,
					const OmnString &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool updateValue(
					const OmnString &container, 
					const OmnString &name,
					const int64_t &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool updateValue(
					const OmnString &container, 
					const OmnString &name,
					const u64 &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool updateValue(
					const OmnString &name,
					const OmnString &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool updateValue(
					const OmnString &name,
					const int64_t &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool updateValue(
					const OmnString &name,
					const u64 &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool getValue(
					const OmnString &container, 
					const OmnString &name,
					OmnString &value,
					const OmnString &dft,
					const AosRundataPtr &rdata) = 0;

	virtual bool getValue(
					const OmnString &container, 
					const OmnString &name,
					int64_t &value,
					const int64_t &dft,
					const AosRundataPtr &rdata) = 0;

	virtual bool getValue(
					const OmnString &container, 
					const OmnString &name,
					u64 &value,
					const u64 &dft,
					const AosRundataPtr &rdata) = 0;

	virtual bool getValue(
					const OmnString &name,
					OmnString &value,
					const OmnString &dft,
					const AosRundataPtr &rdata) = 0;

	virtual bool getValue(
					const OmnString &name,
					int64_t &value,
					const int64_t &dft,
					const AosRundataPtr &rdata) = 0;

	virtual bool getValue(
					const OmnString &name,
					u64 &value,
					const u64 &dft,
					const AosRundataPtr &rdata) = 0;

	static void setSqlClient(const AosSqlClientObjPtr &d) {smSqlClient = d;}
	static AosSqlClientObjPtr getSqlClient() {return smSqlClient;}
};

inline bool AosDbSetValue(
		const OmnString &container, 
		const OmnString &name,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->setValue(container, name, value, rdata);
}


inline bool AosDbSetValue(	
		const OmnString &container, 
		const OmnString &name,
		const int64_t &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->setValue(container, name, value, rdata);
}


inline bool AosDbSetValue(	
		const OmnString &container, 
		const OmnString &name,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->setValue(container, name, value, rdata);
}


inline bool AosDbSetValue(
		const OmnString &name,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->setValue(name, value, rdata);
}


inline bool AosDbSetValue(
		const OmnString &name,
		const int64_t &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->setValue(name, value, rdata);
}


inline bool AosDbSetValue(
		const OmnString &name,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->setValue(name, value, rdata);
}


inline bool AosDbUpdateValue(
		const OmnString &container, 
		const OmnString &name,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->updateValue(container, name, value, rdata);
}


inline bool AosDbUpdateValue(
		const OmnString &container, 
		const OmnString &name,
		const int64_t &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->updateValue(container, name, value, rdata);
}


inline bool AosDbUpdateValue(
		const OmnString &container, 
		const OmnString &name,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->updateValue(container, name, value, rdata);
}


inline bool AosDbUpdateValue(
		const OmnString &name,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->updateValue(name, value, rdata);
}


inline bool AosDbUpdateValue(
		const OmnString &name,
		const int64_t &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->updateValue(name, value, rdata);
}


inline bool AosDbUpdateValue(
		const OmnString &name,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->updateValue(name, value, rdata);
}


inline bool AosDbGetValue(
		const OmnString &container, 
		const OmnString &name,
		OmnString &value,
		const OmnString &dft,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->getValue(container, name, value, dft, rdata);
}


inline bool AosDbGetValue(
		const OmnString &container, 
		const OmnString &name,
		int64_t &value,
		const int64_t &dft,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->getValue(container, name, value, dft, rdata);
}


inline bool AosDbGetValue(
		const OmnString &container, 
		const OmnString &name,
		u64 &value,
		const u64 &dft,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->getValue(container, name, value, dft, rdata);
}


inline bool AosDbGetValue(
		const OmnString &name,
		OmnString &value,
		const OmnString &dft,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->getValue(name, value, dft, rdata);
}


inline bool AosDbGetValue(
		const OmnString &name,
		int64_t &value,
		const int64_t &dft,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->getValue(name, value, dft, rdata);
}


inline bool AosDbGetValue(
		const OmnString &name,
		u64 &value,
		const u64 &dft,
		const AosRundataPtr &rdata)
{
	AosSqlClientObjPtr sqlclient = AosSqlClientObj::getSqlClient();
	aos_assert_r(sqlclient, false);
	return sqlclient->getValue(name, value, dft, rdata);
}

#endif
