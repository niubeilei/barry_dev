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
// 10/24/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/Update.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"


// Update is defined in the form:
// 	<update>
// 		<cmd .../>
// 		<cmd .../>
// 		...
// 	</update>
// 
// Currently, we support the following commands:
// 	<cmd type="inc">
// 		<fname>xxx</fname>
// 		<value>xxx</value>
// 	</cmd>
//
// 	dec	(minus some value)
//	set: set a field to a value
//	remove: remove a field
//	sum: add two or more fields into another field. 
AosUpdate::AosUpdate(AosXmlTagPtr &update)
{
	AosXmlTagPtr command = update->getFirstChild();
	while(command)
	{
		AosUpdatePtr update;
		try
		{
			update = getInstance(command);
			if (update)
				mUpdates.append(update);
		}
		catch(UpdateException &e)
		{
			OmnAlarm << e.what()<<":"<<command->toString() << enderr;
		}
		command = command->getNextChild();
	}

	OmnScreen << "----------------------- entries -------------------" << mUpdates.entries() <<endl; 
	if (!mUpdates.entries())
		throw UpdateException("Update Initialize Error");
}


AosUpdatePtr
AosUpdate::getInstance(AosXmlTagPtr &command)
{
	OmnString type = command->getAttrStr("type");
	UpdateType opType = getUpdateType(type);

	switch(opType)
	{
	case eInc:
		return OmnNew UpdateInc(command);
	default:
		return 0;
	}
}


AosUpdate::UpdateType
AosUpdate::getUpdateType(OmnString &type)
{
	const char *data = type.data();
	switch(data[0])
	{
	case 'i':
		if (type == "inc")
			return eInc;
	default:
		return eUnknow;
	}
}

bool
AosUpdate::update(AosXmlTagPtr &xml)
{
	bool result=true;
	for (i64 i=0; i<mUpdates.entries(); i++)
	{
		result = mUpdates[i]->updateData(xml);
		aos_assert_r(result,false);
	}
	return true;
}

UpdateInc::UpdateInc(AosXmlTagPtr &command)
{
	AosXmlTagPtr fname, value;
	fname = command->getFirstChild("fname");
	if(fname)
		mPath = fname->getNodeText();
	value = command->getFirstChild("value");
	if(value)
	{
		OmnString incNum = value->getNodeText();
		mValue = incNum.toInt();

	}

	// if (!(mPath != ""))
	if (mPath == "")
		throw UpdateException();
}


bool
UpdateInc::updateData(AosXmlTagPtr &xml)
{
	bool exist;
	i64 num = 0;
	bool res;
	OmnString value = xml->xpathQuery(mPath, exist, "");
	if (value != "")
	{
		num+=mValue;
		OmnString tmp;
		tmp << num;
		res = xml->xpathSetAttr(mPath,tmp); 	
		aos_assert_r(res , false);
	}
	else
	{
		num = 1;
		OmnString tmp;
		tmp << num;
		res = xml->xpathSetAttr(mPath,tmp); 	
		aos_assert_r(res , false);
	}
	return true;
}

