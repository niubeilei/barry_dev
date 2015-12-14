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
// This class is the same as AosRVGStr except that its correct value
// will be generated from a list.  For more information about this class, 
// refer to the comments for AosRVGStr.
//
// Modification History:
// 02/06/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGStrEnum.h"

#include "Alarm/Alarm.h"
#include "aos/aosReturnCode.h"
#include "aosUtil/Alarm.h"
#include "Random/RandomSelStr.h"
#include "Random/RandomStr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/GenRecord.h"
#include "Util/GenTable.h"


AosRVGStrEnum::AosRVGStrEnum(const AosCliCmdTorturerPtr &cmd)
:
AosRVGStr(cmd)
{
}


AosRVGStrEnum::~AosRVGStrEnum()
{
}


AosRVGStrEnumPtr
AosRVGStrEnum::createInstance(
			const AosCliCmdTorturerPtr &cmd,
			const OmnXmlItemPtr &def,
			const OmnString &cmdTag, 
            OmnVList<AosGenTablePtr> &tables)
{
	AosRVGStrEnumPtr inst = OmnNew AosRVGStrEnum(cmd);
	if (!inst)
	{
		OmnAlarm << "Run out of memory" << enderr;
		return 0;
	}

	if (!inst->config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to create the parm: " << def->toString() << enderr;
		return 0;
	}

	return inst;
}


bool
AosRVGStrEnum::config(const OmnXmlItemPtr &def, 
					const OmnString &cmdTag,
					OmnVList<AosGenTablePtr> &tables)
{
	// 
	// 	<Parm>
	// 		<type>
	// 		<Name>
	// 		<DataType>
	// 		<FieldName>
	// 		<Selector>
	// 			<Values>[value, weight], [value, weight], ...</Values>
	// 			<BadValue>
	// 			<SeparatorDef>       (Optional)
	// 				<Separator>
	// 			    <MinSeg>
	// 			    <MaxSeg>
	// 			</SeparatorDef>
	// 			<IncorrectGen>       (Optional)
	// 			    <StrType>
	// 			    <MinLen>
	// 			    <MaxLen>
	// 			    <BadValue>
	// 			</IncorrectGen>
	// 		</Selector>
	// 		<KeyType>
	// 		<BadValue>
	// 		<CorrectOnly>
	// 		<StrType>
	// 		<TableName>
	// 		...
	// 	</Parm>
	//
	if (!AosRVGStr::config(def, cmdTag, tables))
	{
		OmnAlarm << "Failed to parse parm: " << def->toString() << enderr;
		return false;
	}

	OmnXmlItemPtr selectorDef = def->getItem("Selector");
	if (!selectorDef)
	{
		OmnAlarm << "Missing Selector Tag: " << def->toString() << enderr;
		return false;
	}

	if (!configSelector(selectorDef))
	{
		OmnAlarm << "Failed to configure the selector: " 
			<< def->toString() <<enderr;
		return false;
	}

	mGenerateIncorrectWeight = eDefaultGenerateIncorrectWeight;
	mIsGood = createSelectors();
	return mIsGood;
}


bool
AosRVGStrEnum::configSelector(const OmnXmlItemPtr &def)
{
	mValueSelector = OmnNew AosRandomSelStr(mArgName, def);
	return mValueSelector->isGood();
}


OmnString
AosRVGStrEnum::genValue()
{
	return mValueSelector->nextStr();
}


OmnString
AosRVGStrEnum::genIncorrectValue(
			AosRVGReturnCode &rcode,
            OmnString &errmsg)
{
	errmsg = mArgName;
	return mValueSelector->nextIncorrectStr(rcode, errmsg);
}


OmnString 
AosRVGStrEnum::getNextValueFuncCall() const
{
	return "nextStr";
}

