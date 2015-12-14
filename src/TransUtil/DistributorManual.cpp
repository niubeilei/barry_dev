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
// This distributor distributes objects based on a map.
//
//
// Modification History:
// 2013/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/DistributorManual.h"

#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"



AosDistributorManual::AosDistributorManual(
		//const AosServerInfo::Type type, 
		const AosXmlTagPtr &conf)
:
//mType(type),
mMaxId(0),
mMap(0)
{

	if (!init(conf))
	{
		OmnThrowException("internal_error");
		return;
	}
}


AosDistributorManual::~AosDistributorManual()
{
	OmnDelete [] mMap;
	mMap = 0;
}


int
AosDistributorManual::routeReq(const u64 &distid)
{
	aos_assert_r(distid >= 0 && distid <= mMaxId, -1);
	aos_assert_r(mMap, -1);
	return mMap[distid];
}


bool
AosDistributorManual::init(const AosXmlTagPtr &conf)
{
	// 'conf' is in the following format:
	// 	<conf max_ids="xxx">
	//		<range target_id="xxx">def,def,def</range>
	//		<range target_id="xxx">def,def,def</range>
	//		...
	// 	</conf>
	// Each 'def' can be one of the following:
	// 	1. ddd: it defines a map: [ddd, target_id]
	// 	2. min:max: it defines a map: [i, target_id], where i is in [min, max]
	// 	3. mod:mm:nn mMap[i] = target_id, where i % mm == nn
	aos_assert_r(conf, false);

	mMaxId = conf->getAttrInt("max_ids", -1);
	if (mMaxId <= 0 || mMaxId >= eMaxIds)
	{
		OmnAlarm << "Invalid max ids: " << mMaxId << ". " 
			<< conf->toString() << enderr;
		return false;
	}

	mMap = OmnNew int[mMaxId+1];
	memset(mMap, -1, sizeof(int) * mMaxId);	

	AosXmlTagPtr tag = conf->getFirstChild();
	while (tag)
	{
		int target_id = tag->getAttrInt("target_id", -1);
		if (target_id < 0)
		{
			OmnAlarm << "Invalid target id: " << conf->toString() << enderr;
			return false;
		}			

		OmnString txt = tag->getNodeText();
		if (txt != "")
		{
			vector<OmnString> defs;
			int nn = AosStrSplit::splitStrByChar(txt, ", ", defs, eMaxIds);
			aos_assert_r(nn > 0, false);

			for (u32 i=0; i<defs.size(); i++)
			{
				vector<OmnString> parts;
				AosStrSplit::splitStrByChar(defs[i], ":", parts, 3);
				if (parts.size() == 1)
				{
					if (!parts[0].isDigitStr())
					{
						OmnAlarm << "Invalid configuration: " << conf->toString() << enderr;
						return false;
					}

					int pp = atoi(parts[0].data());
					if (pp < 0 || (u32)pp > mMaxId)
					{
						OmnAlarm << "Invalid configuration: " << conf->toString() << enderr;
						return false;
					}

					mMap[pp] = target_id;
				}
				else if (parts.size() == 2)
				{
					// It is in the form: 
					// 	min:max
					if (!parts[0].isDigitStr() || !parts[1].isDigitStr())
					{
						OmnAlarm << "Invalid config: " << conf->toString() << enderr;
						return false;
					}

					int min = atoi(parts[0].data());
					int max = atoi(parts[1].data());
					if (min < 0 || (u32)min > mMaxId || max < 0 || (u32)max > mMaxId)
					{
						OmnAlarm << "Invalid config: " << conf->toString() << enderr;
						return false;
					}

					if (min > max)
					{
						OmnAlarm << "Invalid config: " << conf->toString() << enderr;
						return false;
					}

					for (int i=min; i<=max; i++)
					{
						mMap[i] = target_id;
					}
				}
				else if (parts.size() == 3)
				{
					// It is in the form:
					// 	mod:mm:nn
					if (parts[0] != "mod")
					{
						OmnAlarm << "Invalid config: " << conf->toString() << enderr;
						return false;
					}
				
					if (!parts[1].isDigitStr())
					{
						OmnAlarm << "Invalid config: " << conf->toString() << enderr;
						return false;
					}

					int mm = atoi(parts[1].data());
					if (mm < 0 || (u32)mm >= mMaxId)
					{
						OmnAlarm << "Invalid config: " << conf->toString() << enderr;
						return false;
					}

					if (!parts[2].isDigitStr())
					{
						OmnAlarm << "Invalid config: " << conf->toString() << enderr;
						return false;
					}

					int nn = atoi(parts[2].data());
					if (nn < 0 || (u32)nn >= mMaxId)
					{
						OmnAlarm << "Invalid config: " << conf->toString() << enderr;
						return false;
					}

					for (u32 i=nn; i<=mMaxId; i+=mm)
					{
						mMap[i] = target_id;
					}
				}
				else 
				{
					OmnAlarm << "Invalid config: " << conf->toString() << enderr;
					return false;
				}
			}
		}

		tag = conf->getNextChild();
	}

	// Check whether all are set
	for (u32 i=0; i<mMaxId; i++)
	{
		if (mMap[i] < 0)
		{
			OmnAlarm << "invalid config: " << conf->toString() << enderr;
			return false;
		}
	}

	return true;
}

