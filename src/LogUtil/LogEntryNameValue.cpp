//////////////////////////////////////////////////////////////////////////
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
// 01/01/2013: Rewritten by Chen Ding
//////////////////////////////////////////////////////////////////////////
#if 0
#include "LogUtil/LogEntryNameValue.h"

AosLogEntryNameValue::AosLogEntryNameValue()
{
	mEntrySep = AosSeparators::getEntrySep();
	mFieldSep = AosSeparators::getFieldSep();
	mAttrSep  = AosSeparators::getAttrSep();
}


AosLogEntryNameValue::AosLogEntryNameValue(
		const char entry_sep, 
		const char field_sep, 
		const char attr_sep)
{
	mEntrySep = entry_sep;
	mFieldSep = field_sep;
	mAttrSep  = attr_sep;
}


AosLogEntryNameValue::~AosLogEntryNameValue()
{
}


bool 
AosLogEntryNameValue::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(mBuff, rdata, false);
	return buff->addBuff(mBuff->data(), mBuff->dataLen());
}


bool
AosLogEntryNameValue::addField(
		const OmnString &name, 
		const OmnString &value, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mBuff, rdata, false);
	return mBuff->addNameValuePair(name, value, mFieldSep, mAttrSep);
}


bool
AosLogEntryNameValue::addField(
		const OmnString &name, 
		const u64 &value, 
		const AosRundataPtr &rdata) 
{
	aos_assert_rr(mBuff, rdata, false);
	return mBuff->addNameValuePair(name, value, mFieldSep, mAttrSep);
}


bool
AosLogEntryNameValue::procIndex(
		const AosLogContainerPtr &container,
		const AosRundataPtr &rdata)
{
	// Indexes can be done on: 
	// 	1. Container Attribute Index
	// 	2. Container Keywoord Index
	// 	3. Container Attribute Keyword
	// 	4. Global Attribute Index
	// 	5. Global Keywoord Index
	// 	6. Global Attribute Keyword
	if (!mBuff || !mDocid) return true;

	mBuff->reset();
	int name_start, name_end, value_start, value_end;
	char *data = (char *)mBuff->data();
	while (mBuff->nextNameValuePair(name_start, name_end, value_start, value_end))
	{
		// Found one [name, value] pair. 
		char c1 = data[name_end]; data[name_end] = 0;
		char c2 = data[value_end]; data[value_end] = 0;

		int delta = value_end - value_start + 1;
		if (delta > 0)
		{
			// 1. Process Container and Global Attribute Indexes
			AosIILAssemblerPtr assembler1 = container->getContainerAttrAssm(&data[name_start]);
			AosIILAssemblerPtr assembler2 = container->getGlobalAttrAssm(&data[name_start]);
			if (assembler1 || assembler2)
			{
				AosValueRslt value_rslt;
				value_rslt.setKeyValue(&data[value_start], delta, false, mDocid);

				if (assembler1) assembler1->appendEntry(value_rslt, rdata);
				if (assembler2) assembler2->appendEntry(value_rslt, rdata);
			}

			// 2. Process Keyword indexes
			assembler1 = container->getContainerKeywordAssm(&data[name_start]);
			assembler2 = container->getContainerAttrKeywordAssm(&data[name_start]);
			AosIILAssemblerPtr assembler3 = container->getGlobalKeywordAssm(&data[name_start]);
			AosIILAssemblerPtr assembler4 = container->getGlobalAttrKeywordAssm(&data[name_start]);
			if (assembler1 || assembler2 || assembler3 || assembler4)
			{
				whle (word)
				{
					if (assembler1) assembler1->appendEntry(value_rslt, rdata);
					if (assembler2) assembler2->appendEntry(value_rslt, rdata);
					if (assembler3) assembler3->appendEntry(value_rslt, rdata);
					if (assembler4) assembler4->appendEntry(value_rslt, rdata);
				}
			}
		}
	}

	return true;
}
#endif
