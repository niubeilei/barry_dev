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
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Jimo/Interface.h"

#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Thread/Thread.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"

AosInterface::AosInterface()
:
mLock(OmnNew OmnMutex())
{
}


AosInterface::~AosInterface()
{
}


AosInterfacePtr 
AosInterface::clone(AosRundata *rdata) const
{
	try
	{
		return OmnNew AosInterface(*this);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return 0;
	}
	return 0;
}


OmnString 
AosInterface::toString() const
{
	return "";
}


bool 
AosInterface::config(	
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	// The 'def' is:
	// 	<def ...>
	// 		<method name="xxx" mandatory="true|false"/>
	// 		<method name="xxx" mandatory="true|false"/>
	// 		...
	// 	</def>
	aos_assert_rr(def, rdata, false);
	mDef = def->toString();
	AosXmlTagPtr tag = def->getFirstChild();
	if (!tag)
	{
		AosSetErrorU(rdata, "missing_methods") << mDef << enderr;
		return false;
	}

	mMethods.clear();
	while (tag)
	{
		OmnString name = tag->getAttrStr("name");
		if (name == "")
		{
			AosSetErrorUser(rdata, "missing name") << tag->toString()
				<< ". " << AOSDICTERM("input_is", rdata) 
				<< mDef << enderr;
			return false;
		}

		AosMethodId::E method_id = AosMethodId::toEnum(name);
		if (!AosMethodId::isValid(method_id))
		{
			AosSetErrorUser(rdata, "invalid_method") << name 
				<< ". " << AOSDICTERM("input_is", rdata) 
				<< mDef << enderr;
			return false;
		}

		bool mandatory = tag->getAttrBool("mandatory", false);
		itr_t itr = mMethods.find(name);
		if (itr != mMethods.end())
		{
			AosSetErrorUser(rdata, "name_not_unique") << name 
				<< ". " << AOSDICTERM("input_is", rdata) 
				<< mDef << enderr;
			return false;
		}
		mMethods[name] = AosMethodDef(method_id, 0, mandatory);
		tag = def->getNextChild();
	}
	return true;
}


bool
AosInterface::retrieveMethods(
		AosRundata *rdata, 
		AosJimo *jimo)
{
	// This function retrieves all the methods defined in this
	// class from 'jimo'. It returns false if any of the mandatory
	// methods are not supported by the jimo.
	aos_assert_rr(jimo, rdata, false);
	mLock->lock();
	aos_assert_rl(mMethods.size() > 0, mLock, false);
	itr_t itr = mMethods.begin();
	AosMethodId::E method_id;
	while (itr != mMethods.end())
	{
		bool mandatory = itr->second.mandatory;
		OmnString name = itr->first;

		void *func = jimo->getMethod(rdata, name.data(), method_id);
		if (func)
		{
			if (method_id != itr->second.method_id)
			{
				AosSetErrorUser(rdata, "method_id_mismatch")
					<< AOSDICTERM("expected_value", rdata) 
					<< ": " << AosMethodId::toName(itr->second.method_id)
					<< ", " << AOSDICTERM("actual_value", rdata)
					<< ": " << AosMethodId::toName(method_id)
					<< ". " << AOSDICTERM("interface_def_doc", rdata) << mDef << enderr;
				mLock->unlock();
				return false;
			}

			itr->second.func = func;
		}
		else 
		{
			if (mandatory)
			{
				AosSetErrorUser(rdata, "missing_method")
					<< name << AOSDICTERM("interface_def_doc", rdata) << mDef << enderr;
				mLock->unlock();
				return false;
			}
			itr->second.func = 0;
		}

		itr++;
	}

	return true;
}


void * 
AosInterface::getMethod(
		AosRundata *rdata, 
		const char *name, 
		AosMethodId::E &method_id)
{
	mLock->lock();
	itr_t itr = mMethods.find(name);
	if (itr == mMethods.end())
	{
		mLock->unlock();
		return 0;
	}

	void *func = itr->second.func;
	method_id = itr->second.method_id;
	mLock->unlock();
	return func;
}


bool 
AosInterface::serializeTo(
		AosRundata *rdata, 
		AosBuff *buff) 
{
	// Only the method_id and mandatory flag are serialized.
	aos_assert_rr(buff, rdata, false);
	mLock->lock();
	u32 size = mMethods.size();
	buff->setU32(size);
	if (size == 0)
	{
		mLock->unlock();
		return true;
	}

	itr_t itr = mMethods.begin();
	while (itr != mMethods.end())
	{
		aos_assert_rl(AosMethodId::isValid(itr->second.method_id), mLock, false);
		aos_assert_rl(itr->first != "", mLock, false);
		buff->setU32(itr->second.method_id);
		buff->setU32(itr->second.mandatory);
		buff->setOmnStr(itr->first);
		itr++;
	}

	mLock->unlock();
	return true;
}


bool 
AosInterface::serializeFrom(
		AosRundata *rdata, 
		AosBuff *buff)
{
	aos_assert_rr(buff, rdata, false);
	mLock->lock();
	u32 size = buff->getU32(0);
	mMethods.clear();
	for (u32 i=0; i<size; i++)
	{
		AosMethodId::E method_id = (AosMethodId::E)buff->getU32(0);
		bool mandatory = buff->getU32(0);
		OmnString name = buff->getOmnStr("");
		aos_assert_rl(AosMethodId::isValid(method_id), mLock, false);
		aos_assert_rl(name != "", mLock, false);

		mMethods[name] = AosMethodDef(method_id, 0, mandatory);
	}
	mLock->unlock();
	return true;
}

