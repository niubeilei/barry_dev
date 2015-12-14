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
// This dataset uses one data scanner as its inputs. The data scanner
// can be any type. Most datasets can be implemented by this class.
//
// Modification History:
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataConverter/Jimos/DataConvMap.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataConvMap_0(const OmnString &version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataConvMap(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosDataConvMap::AosDataConvMap(const OmnString &version)
:
AosDataConverter("map", version)
{
}


AosDataConvMap::~AosDataConvMap()
{
}


bool
AosDataConvMap::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	// It uses a map to convert data. The converted values are 
	// strings. 
	//	<worker_doc>
	//		<map>
	//			<entry from="xxx" to="xxx"/>
	//			<entry from="xxx" to="xxx"/>
	//			...
	//		</map>
	//	</worker_doc>
	if (!AosDataConverter::config(rdata, worker_doc, jimo_doc)) return false;

	if (!worker_doc)
	{
		AosSetErrorUser(rdata, "dataconvmap_missing_config") << enderr;
		return false;
	}

	AosXmlTagPtr map_tag = worker_doc->getFirstChild("map");
	if (!map_tag)
	{
		AosSetErrorUser(rdata, "dataconvmap_missing_map_def") << enderr;
		return false;
	}

	AosXmlTagPtr tag = map_tag->getFirstChild();
	while (tag)
	{
		OmnString from_val = tag->getAttrStr("from");
		OmnString to_val = tag->getAttrStr("to");
		if (from_val != "")
		{
			mMap[from_val] = to_val;
		}
		tag = map_tag->getNextChild();
	}

	if (mMap.size() <= 0)
	{
		AosSetErrorUser(rdata, "dataconvmap_map_is_empty") << enderr;
		return false;
	}

	tag = worker_doc->getFirstChild("valueset");
	if (!tag)
	{
		AosSetErrorUser(rdata, "dataconvmap_missing_valueset");
		return false;
	}

	mValueset = AosCreateValueset(rdata, tag);
	if (!mValueset)
	{
		AosSetErrorUser(rdata, "dataconvmap_failed_create_valueset", worker_doc);
		return false;
	}

	return true;
}


AosJimoPtr
AosDataConvMap::cloneJimo() const
{
	try
	{
		return OmnNew AosDataConvMap(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosDataConvMap::run( 
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &xml)
{
	AosXmlTagPtr root_tag = getRootTag(rdata, xml);
	if (!root_tag) return true;

	AosXmlTagPtr tag;
	reset();
	while ((tag = getNextValueTag(rdata, root_tag)))
	{
		bool missing;
		OmnString from_value = getInputValue(rdata, tag, missing);
		OmnString to_value
		if (missing)
		{
			to_value = mMissingDefault;
		}
		else
		{
			mapitr_t itr = mMap.find(from_value);
			if (itr == mMap.end())
			{
				to_value = mNotFoundDefault;
			}
			else
			{
				to_value = itr->second;
			}
		}

		setOutputValue(rdata, tag, to_value);
	}

	return true;
}


bool 
AosDataConvMap::run(
		const AosRundataPtr &rdata,
		const OmnString &data)
{
}

