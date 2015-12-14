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
#include "SqlClient/SqlClient.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SqlUtil/SqlUtil.h"
#include "XmlUtil/XmlTag.h"



OmnSingletonImpl(AosSqlClientSingleton,
                 AosSqlClient,
                 AosSqlClientSelf,
                "AosSqlClient");



AosSqlClient::AosSqlClient()
{
}


AosSqlClient::~AosSqlClient()
{
}


bool
AosSqlClient::start()
{
	return true;
}


bool
AosSqlClient::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosSqlClient::stop()
{
    return true;
}


bool 
AosSqlClient::setValue(	
		const OmnString &container, 
		const OmnString &name,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(container != "", rdata, false);
	aos_assert_rr(name != "", rdata, false);

	OmnString nn = name;
	nn.escapeSymbal('\'', '\'');
	OmnString vv = value;
	vv.escapeSymbal('\'', '\'');

	OmnString tname = AosSqlUtil::composeStrValueTablename(container, rdata->getSiteid());
	OmnString stmt = "insert into ";
	stmt << tname << " (name, value) values ('" << nn << "','" << vv << "')";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rr(store, rdata, false);
	if (!store->runSQL(stmt))
	{
		AosSetError(rdata, "failed_set_sql_value_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool 
AosSqlClient::setValue(	
		const OmnString &name,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	return setValue(AOSDBCONTAINER_SYSTEM, name, value, rdata);
}


bool 
AosSqlClient::setValue(	
		const OmnString &container, 
		const OmnString &name,
		const int64_t &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(container != "", rdata, false);
	aos_assert_rr(name != "", rdata, false);

	OmnString nn = name;
	nn.escapeSymbal('\'', '\'');

	OmnString tname = AosSqlUtil::composeInt64ValueTablename(container, rdata->getSiteid());
	OmnString stmt = "insert into ";
	stmt << tname << " (name, value) values ('" << nn << "'," << value << ")";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rr(store, rdata, false);
	if (!store->runSQL(stmt))
	{
		AosSetError(rdata, "failed_set_sql_value_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool 
AosSqlClient::setValue(	
		const OmnString &name,
		const int64_t &value,
		const AosRundataPtr &rdata)
{
	return setValue(AOSDBCONTAINER_SYSTEM, name, value, rdata);
}


bool 
AosSqlClient::setValue(	
		const OmnString &container, 
		const OmnString &name,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(container != "", rdata, false);
	aos_assert_rr(name != "", rdata, false);

	OmnString nn = name;
	nn.escapeSymbal('\'', '\'');

	OmnString tname = AosSqlUtil::composeU64ValueTablename(container, rdata->getSiteid());
	OmnString stmt = "insert into ";
	stmt << tname << " (name, value) values ('" << nn << "'," << value << ")";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rr(store, rdata, false);
	if (!store->runSQL(stmt))
	{
		AosSetError(rdata, "failed_set_sql_value_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool 
AosSqlClient::setValue(	
		const OmnString &name,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	return setValue(AOSDBCONTAINER_SYSTEM, name, value, rdata);
}


bool 
AosSqlClient::updateValue(	
		const OmnString &container, 
		const OmnString &name,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(container != "", rdata, false);
	aos_assert_rr(name != "", rdata, false);

	OmnString nn = name;
	nn.escapeSymbal('\'', '\'');
	OmnString vv = value;
	vv.escapeSymbal('\'', '\'');

	OmnString tname = AosSqlUtil::composeStrValueTablename(container, rdata->getSiteid());
	OmnString stmt = "update ";
	stmt << tname << " set value='" << vv << "' where name='" << nn << "'";

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rr(store, rdata, false);
	if (!store->runSQL(stmt))
	{
		AosSetError(rdata, "failed_set_sql_value_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool 
AosSqlClient::updateValue(	
		const OmnString &name,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	return updateValue(AOSDBCONTAINER_SYSTEM, name, value, rdata);
}


bool 
AosSqlClient::updateValue(	
		const OmnString &container, 
		const OmnString &name,
		const int64_t &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(container != "", rdata, false);
	aos_assert_rr(name != "", rdata, false);

	OmnString nn = name;
	nn.escapeSymbal('\'', '\'');

	OmnString tname = AosSqlUtil::composeStrValueTablename(container, rdata->getSiteid());
	OmnString stmt = "update ";
	stmt << tname << " set value=" << value << " where name='" << nn << "'";

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rr(store, rdata, false);
	if (!store->runSQL(stmt))
	{
		AosSetError(rdata, "failed_set_sql_value_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool 
AosSqlClient::updateValue(	
		const OmnString &name,
		const int64_t &value,
		const AosRundataPtr &rdata)
{
	return updateValue(AOSDBCONTAINER_SYSTEM, name, value, rdata);
}


bool 
AosSqlClient::updateValue(	
		const OmnString &container, 
		const OmnString &name,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(container != "", rdata, false);
	aos_assert_rr(name != "", rdata, false);

	OmnString nn = name;
	nn.escapeSymbal('\'', '\'');

	OmnString tname = AosSqlUtil::composeStrValueTablename(container, rdata->getSiteid());
	OmnString stmt = "update ";
	stmt << tname << " set value=" << value << " where name='" << nn << "'";

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rr(store, rdata, false);
	if (!store->runSQL(stmt))
	{
		AosSetError(rdata, "failed_set_sql_value_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool 
AosSqlClient::updateValue(	
		const OmnString &name,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	return updateValue(AOSDBCONTAINER_SYSTEM, name, value, rdata);
}


bool 
AosSqlClient::getValue(	
		const OmnString &container, 
		const OmnString &name,
		OmnString &value,
		const OmnString &dft,
		const AosRundataPtr &rdata)
{
	value = dft;
	aos_assert_rr(container != "", rdata, false);
	aos_assert_rr(name != "", rdata, false);

	OmnString nn = name;
	nn.escapeSymbal('\'', '\'');

	OmnString tname = AosSqlUtil::composeStrValueTablename(container, rdata->getSiteid());
	OmnString stmt = "select value from ";
	stmt << tname << " where name='" << nn << "'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rr(store, rdata, false);
	OmnDbRecordPtr record;
	if (!store->query(stmt, record) || !record)
	{
		return true;
	}

	OmnRslt rslt;
	value = record->getStr(0, dft, rslt);
	return true;
}


bool 
AosSqlClient::getValue(	
		const OmnString &name,
		OmnString &value,
		const OmnString &dft,
		const AosRundataPtr &rdata)
{
	return getValue(AOSDBCONTAINER_SYSTEM, name, value, dft, rdata);
}


bool 
AosSqlClient::getValue(	
		const OmnString &container, 
		const OmnString &name,
		int64_t &value,
		const int64_t &dft,
		const AosRundataPtr &rdata)
{
	value = dft;
	aos_assert_rr(container != "", rdata, false);
	aos_assert_rr(name != "", rdata, false);

	OmnString nn = name;
	nn.escapeSymbal('\'', '\'');

	OmnString tname = AosSqlUtil::composeInt64ValueTablename(container, rdata->getSiteid());
	OmnString stmt = "select value from ";
	stmt << tname << " where name='" << nn << "'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rr(store, rdata, false);
	OmnDbRecordPtr record;
	if (!store->query(stmt, record) || !record)
	{
		return true;
	}

	OmnRslt rslt;
	value = record->getInt64(0, dft, rslt);
	return true;
}


bool 
AosSqlClient::getValue(	
		const OmnString &name,
		int64_t &value,
		const int64_t &dft,
		const AosRundataPtr &rdata)
{
	return getValue(AOSDBCONTAINER_SYSTEM, name, value, dft, rdata);
}


bool 
AosSqlClient::getValue(	
		const OmnString &container, 
		const OmnString &name,
		u64 &value,
		const u64 &dft,
		const AosRundataPtr &rdata)
{
	value = dft;
	OmnString nn = name;
	nn.escapeSymbal('\'', '\'');

	OmnString tname = AosSqlUtil::composeU64ValueTablename(container, rdata->getSiteid());
	OmnString stmt = "select value from ";
	stmt << tname << " where name='" << nn << "'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_rr(store, rdata, false);
	OmnDbRecordPtr record;
	if (!store->query(stmt, record) || !record)
	{
		value = dft;
		return true;
	}

	OmnRslt rslt;
	value = record->getU64(0, dft, rslt);
	return true;
}


bool 
AosSqlClient::getValue(	
		const OmnString &name,
		u64 &value,
		const u64 &dft,
		const AosRundataPtr &rdata)
{
	return getValue(AOSDBCONTAINER_SYSTEM, name, value, dft, rdata);
}

