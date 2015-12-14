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
// 	Created: 05/16/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataService/DataService.h"

#include "alarm_c/alarm.h"
#include "DataStore/Ptrs.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Proggie/ProggieUtil/NetRequest.h"
#include "TinyXml/TinyXml.h"


AosDataService::AosDataService()
{
}


bool	
AosDataService::retrieveData(
		TiXmlNode *root, 
		OmnString &contents, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// The request should be in the form:
	// 	<request>
	// 		<item name="operation"><![CDATA[retrieve_data]]></item>
	// 		<item name="type"><![CDATA[the type]]></item>
	// 		<item name="xxx"><![CDATA[xxx]]></item>
	// 		<item name="xxx"><![CDATA[xxx]]></item>
	// 		<xmlobj>
	// 			...
	// 		</xmlobj>
	// 		...
	// 	</request>
	errcode = eAosXmlInt_General;
	errmsg = "Internal error";
	OmnString type = root->getChildTextByAttr("name", "type", 0);
	if (type.length() == 0)
	{
		errmsg = "Missing the type tag!";
		return false;
	}

	if (type == "tablenames")
	{
		OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
		aos_assert_r(store, false);

		std::list<std::string> names;
		int rslt = store->getAllTableNames(names);
		if (rslt <= 0)
		{
			errmsg = "Failed to retrieve table names";
			OmnAlarm << errmsg << enderr;
			return false;
		}
		contents = "<contents><![CDATA[";

		std::list<std::string>::iterator itr;
		for (itr=names.begin(); itr != names.end(); ++itr)
		{
			if (itr == names.begin()) contents << *itr;
			else contents << "|$$|" << *itr;
		}
		errcode = eAosXmlInt_Ok;
		contents << "]]></contents>";
		return true;
	}

	errmsg = "Unrecognized type: ";
	errmsg << type;
	OmnAlarm << errmsg;
	return false;
}

