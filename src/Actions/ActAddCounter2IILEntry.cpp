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
// This action sets an attribute:
// 	<action zky_type="AOSACTION_SETATTR" xpath="xxx">
// 		<doc .../>
// 		<value .../>
// 	</action>
// 	where <doc> is a Doc Selection tag. <value> is a Value Tag. 
// 	This is the value to be set.
//
// Modification History:
// 12/26/2012	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActAddCounter2IILEntry.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "IILUtil/IILId.h"
#include "Rundata/Rundata.h"
#include "UtilTime/TimeGran.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"

AosActAddCounter2IILEntry::AosActAddCounter2IILEntry(const bool flag)
:
AosSdocAction(AOSACTTYPE_ADDCOUNTERIILENTRY, AosActionType::eAddCounterIILEntry, flag)
{
}

AosActAddCounter2IILEntry::~AosActAddCounter2IILEntry()
{
}


bool	
AosActAddCounter2IILEntry::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<sdoc AOSTAG_XPATH="xxx"
	// 		  AOSTAG_VALUE_UNIQUE="true|false"
	// 		  AOSTAG_DOCID_UNIQUE="true|false">
	// 		<docselector .../>
	// 		<value .../>
	// 	</sdoc>
	// where "<docselector>" is a doc selector that selects a doc; AOSTAG_XPATH
	// specifies the attribute, "<value>" specifies how to retrieve the
	// value. If AOSTAG_VALUE_UNIQUE is not specified, it defaults to false.
	// If AOSTAG_DOCID_UNIQUE is not specified, it defaults to false.
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString idtype = sdoc->getAttrStr("idtype", "loop");
	if (idtype == "loop")
	{
		AosXmlTagPtr workingdoc = rdata->getWorkingDoc();
		aos_assert_r(workingdoc, false);
		AosXmlTagPtr doc = workingdoc->getFirstChild(true);
		while(doc)
		{
			rdata->setWorkingDoc(doc, false);
			bool rslt = createEntry(sdoc, rdata);
			if (!rslt)
			{
				OmnAlarm << "faild" << enderr;
			}
			doc = workingdoc->getNextChild(); 
		}
		rdata->setWorkingDoc(workingdoc, false);
	}
	else
	{
		return createEntry(sdoc, rdata);
	}
	return true;
}


bool
AosActAddCounter2IILEntry::createEntry(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnString counter_id;
	AosValueRslt valueIdRslt;
	bool rslt = AosValueSel::getValueStatic(valueIdRslt, sdoc, AOSTAG_COUNTERID, rdata);
	aos_assert_r(rslt, false);

	counter_id = valueIdRslt.getStr();
	aos_assert_r(counter_id != "", false);

	OmnString counterName;
	rslt = AosResolveCounterName::resolveCounterName2(
			sdoc, AOSTAG_COUNTERNAMES, counterName, rdata);
	aos_assert_r(rslt, false);

	int64_t counterValue = 0;
	AosValueRslt valueVRslt;
	rslt = AosValueSel::getValueStatic(valueVRslt, sdoc, AOSTAG_COUNTERVALUE, rdata);
	if (rslt)
	{
		counterValue = valueVRslt.getI64(); 
	}
	if (counterValue == 0) return true;

	// gets counter stattype from smart doc. It's optional.
	AosXmlTagPtr statTypeChild = sdoc->getFirstChild(AOSTAG_COUNTERSTATTYPE);
	AosValueRslt statTypeRslt;
	OmnString statTypeStr = "sum";
	rslt = AosValueSel::getValueStatic(statTypeRslt, sdoc, AOSTAG_COUNTERSTATTYPE, rdata);
	if (rslt)
	{
		statTypeStr = statTypeRslt.getStr();
	}
	AosStatType::E statType = AosStatType::toEnum(statTypeStr);
	aos_assert_r(AosStatType::isOrigStat(statType), false);

	AosValueRslt valueCtypeRslt;
	OmnString ctype = "ntm";
	rslt = AosValueSel::getValueStatic(valueCtypeRslt, sdoc, AOSTAG_COUNTERTYPE, rdata);
	if (rslt)
	{
		ctype = valueCtypeRslt.getStr();
	}
	AosTimeGran::E time_gran = AosTimeGran::toEnum(ctype);
	aos_assert_r(AosTimeGran::isValid(time_gran), false);

	int level = 0;
	AosValueRslt valueLevelRslt;
	rslt = AosValueSel::getValueStatic(valueLevelRslt, sdoc, "counter_level", rdata);
	if (rslt)
	{
		level = valueLevelRslt.getI64(); 
	}
	aos_assert_r(level >= 0, false);

	int append_bit = 0;
	AosValueRslt valueAppendBitRslt;
	rslt = AosValueSel::getValueStatic(valueAppendBitRslt, sdoc, "counter_appendbit", rdata);
	if (rslt)
	{
		append_bit = valueAppendBitRslt.getI64();
	}
	aos_assert_r(append_bit >= 0, false);

	u16 entry_type = AosCounterUtil::composeTimeEntryType(level, append_bit, statType, time_gran);
	OmnString cname = AosCounterUtil::composeTerm2(entry_type, counterName, 0);
//OmnScreen << "cname: " << cname << endl;
	u64 iilid = getIILID(counter_id, rdata);
	aos_assert_r(iilid, false);

	u64 init_value = 0;
	OmnString alg = sdoc->getAttrStr("zky_algorithm", "accumulate");
	if (alg == "accumulate")
	{
		AosXmlTagPtr tag = sdoc->getFirstChild("counter_other");
		aos_assert_r(tag, false);

		OmnString totalname;
		rslt = AosResolveCounterName::resolveCounterName2(
				tag, totalname, rdata);
		aos_assert_r(rslt, false);

		int total_level = tag->getAttrInt("zky_level", 0);
		int append_bit1 = tag->getAttrInt("append_bit", false);
		OmnString timegranstr= tag->getAttrStr("zky_timegran", "ntm");
		AosTimeGran::E time_gran1 = AosTimeGran::toEnum(timegranstr);
		aos_assert_r(AosTimeGran::isValid(time_gran1), false);
		u16 entry_type = AosCounterUtil::composeTimeEntryType(total_level, append_bit1, statType, time_gran1);
		OmnString total_cname = AosCounterUtil::composeTerm2(entry_type, totalname, 0);

//OmnScreen << "totalname: " << total_cname << endl;
// bool is_unique, found;
// u64 value = AosGetKeyedStrValue(iilid, total_cname, 0, is_unique, found, rdata);
//OmnScreen << " totalname old :" << value << endl;
		u64 vv = 0;
		AosIncrementKeyedValue(iilid, total_cname, vv, counterValue, init_value, true, rdata);
		init_value = vv ;
// value = AosGetKeyedStrValue(iilid, total_cname, 0, is_unique, found, rdata);
//OmnScreen << " totalname new :" << value << endl;
	}
// bool is_unique, found;
// u64 value = AosGetKeyedStrValue(iilid, cname, 0, is_unique, found, rdata);
//OmnScreen << " cname old :" << value << endl;
	u64 vv = 0;
	AosIncrementKeyedValue(iilid, cname, vv, counterValue, init_value, true, rdata);
// value = AosGetKeyedStrValue(iilid, cname, 0, is_unique, found, rdata);
//OmnScreen << " cname new :" << value << endl;
	rdata->setOk();
	return true;
}


u64
AosActAddCounter2IILEntry::getIILID(
		const OmnString &counter_id,
		const AosRundataPtr &rdata)
{
	// Retrieve the counterID
	// 'cname' should be in the form:
	//      counter_id|$$|name|$$|name|$$|...
	//  counter_id :AOSZTG_COUNTER + '_' + counter_id
	//
	OmnString kk = AosIILName::composeCounterIILName(counter_id);
	aos_assert_r(kk != "", false);
	u64 iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, kk, 0, rdata);
	if (iilid == 0)
	{
		// It is not created yet. Create it now.
		iilid = AosCreateIIL(kk, eAosIILType_Str, true, rdata);
		aos_assert_r(iilid, 0);
		u64 tmp_iilid = AOSIILID_COUNTER_IILIDS;
		AosSetStrValueDocUnique(tmp_iilid, false, kk, iilid, false, rdata);
		aos_assert_r(tmp_iilid == AOSIILID_COUNTER_IILIDS, 0);
	}

	return iilid;
}


AosActionObjPtr
AosActAddCounter2IILEntry::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActAddCounter2IILEntry(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

