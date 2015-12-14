////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashUtil.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/HashUtil.h"

#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"

unsigned int OmnHashUtil::mBitMasks[eMaxNumBits] = 
{
	0x1,
	0x3,
	0x7,
	0xf,

	0x1f,
	0x3f,
	0x7f,
	0xff,

	0x1ff,
	0x3ff,
	0x7ff,
	0xfff,

	0x1fff,
	0x3fff,
	0x7fff,
	0xffff,

	0x1ffff,
	0x3ffff,
	0x7ffff,
	0xfffff,

	0x1fffff,
	0x3fffff,
	0x7fffff,
	0xffffff,

	0x1ffffff,
	0x3ffffff,
	0x7ffffff,
	0xfffffff,

	0x1fffffff,
	0x3fffffff,
	0x7fffffff,
	0xffffffff
};


unsigned int		
OmnHashUtil::getBitmask(const int numBits)
{
	OmnCheckAReturn(numBits > 0, "Number of bits is negative: ", numBits);
	OmnCheckAReturn(numBits < eMaxNumBits, "numBits too big: ", numBits);

	return mBitMasks[numBits - 1]; 
}


unsigned int
OmnHashUtil::getNearestBitmask(const uint value)
{
	// 
	// A bitmask is a continuous series of '1'. This function
	// returns the smallest bitmask >= value. If 'value' is
	// bigger than the biggest bitmask, return the biggest one.
	//
	// This function will always return a valid value. 
	//
	for (uint i=eMaxNumBits-1; i>=0; i--)
	{
		if (mBitMasks[i] <= value)
		{
			// 
			// This is the first value that is smaller than the value.
			// Return the result.
			//
			return mBitMasks[i];
		}
	}

	// 
	// This means 'value' is less than 1. We will raise a warning
	// and return the smallest bitmask
	//
	OmnWarn << "Value too small: " << value << enderr;
	return 0xff;
}


bool
OmnHashUtil::verifyBitmask(const uint value)
{
	// 
	// This function checks whether 'value' is one of the bitmasks
	// defined in this class. 
	//
	for (int i=0; i<=eMaxNumBits; i++)
	{
		if (mBitMasks[i] == value)
		{
			return true;
		}
	}

	return false;
}

u64
AosGetHashKeyInMap(const char *word, const int len)
{
	//Ken Lee 2013/04/01
	static map<OmnString, u64> sgHashKeyMap;
	static OmnMutex sgLock;
	static bool sgHashKeyMapInited = false;

	if(!sgHashKeyMapInited)
	{
		sgLock.lock();
		if(!sgHashKeyMapInited)
		{
			OmnString array[] = {
				"_zt44_unicom_upn_citycode",
				"_zt44_unicom_upn_towncode",
				"_zt44_unicom_cpn_citycode",

				"_zt44_unidx_big_type_system_type_msisdn",
				"_zt44_unidx_big_type_msisdn_start_datetime",
				"_zt44_unidx_big_type_msisdn_system_type_start_datetime",
				"_zt44_unidx_big_type_msisdn_deal_time_start_datetime",
				"_zt44_unidx_big_type_msisdn_system_type_deal_time_start_datetime",
				"_zt44_unidx_big_type_other_party_start_datetime",
				"_zt44_unidx_cellid_start_datetime",
				"_zt44_unidx_cellid_system_type_start_datetime",
				"_zt44_unidx_cellid_deal_time_start_datetime",
				"_zt44_unidx_cellid_system_type_deal_time_start_datetime",
				"_zt44_unidx_cellid_lac_start_datetime",
				"_zt44_unidx_cellid_lac_system_type_start_datetime",
				"_zt44_unidx_cellid_lac_deal_time_start_datetime",
				"_zt44_unidx_cellid_lac_system_type_deal_time_start_datetime",

				"_zt44_unst_cdrs_epochday",
				"_zt44_unst_system_type_epochday",
				"_zt44_unst_system_type_deal_time_epochday",
				"_zt44_unst_reclaim_epochday",
				"_zt44_unst_rollback_epochday",
				"_zt44_unst_short_callduration_epochday",
				"_zt44_unst_long_callduration_epochday",

				"_zt44_unst_cellid_lac_cdrs_epochday",
				"_zt44_unst_cellid_lac_calltime_epochday",
				"_zt44_unst_cellid_lac_callduration_epochday",
				"_zt44_unst_cellid_lac_callcost_epochday",
				"_zt44_unst_cellid_lac_cdrs_epochhour",
				"_zt44_unst_cellid_lac_calltime_epochhour",
				"_zt44_unst_cellid_lac_callduration_epochhour",
				"_zt44_unst_cellid_lac_callcost_epochhour",
				"_zt44_unst_cellid_lac_hourofday_epochday",

				"_zt44_unst_city_town_cellid_lac_calltype_msisdn_epochhour",
				"_zt44_unst_city_town_cellid_lac_calltype_msisdn_epochday",
				"_zt44_unst_city_town_cellid_lac_calltype_msisdn_epochmonth",
				"_zt44_unst_city_town_cellid_lac_calltype_msisdn_epochweek",
				"_zt44_unst_city_town_cellid_lac_calltype_msisdn",
				"_zt44_unst_city_town_cellid_lac_calltype_calltime",
				"_zt44_unst_city_town_cellid_lac_calltype_callduration",
				"_zt44_unst_city_town_cellid_lac_calltype_callcost",
				"_zt44_unst_city_town_cellid_lac_calltype_callduration_level",
				"_zt44_unst_city_town_cellid_lac_calltype_shortmsg_num",

				"_zt44_unst_msisdn_calltime",
				"_zt44_unst_msisdn_shortmsg_num",
				"_zt44_unst_msisdn_callduration",
				"_zt44_unst_msisdn_cdrs",

				"_zt44_unst_city_town__cdrs_epochday",
				"_zt44_unst_city_town__system_type",
				"_zt44_unst_city_town__system_type_mid_epochday",
				"_zt44_unst_city_town__reclaim_epochday",
				"_zt44_unst_city_town__reclaim_mid_epochday",
				"_zt44_unst_city_town__rollback_epochday",
				"_zt44_unst_city_town__rollback_mid_epochday",
				"_zt44_unst_city_town__short_callduration_epochday",
				"_zt44_unst_city_town__short_callduration_mid_epochday",
				"_zt44_unst_city_town__long_callduration_epochday",
				"_zt44_unst_city_town__long_callduration_mid_epochday",

				"_zt44_unst_city__cellid_lac_cdrs",
				"_zt44_unst_city_town__cellid_lac_cdrs",
				"_zt44_unst_city_town__cellid_lac_cdrs_mid",
				"_zt44_unst_city__cellid_lac_calltime",
				"_zt44_unst_city_town__cellid_lac_calltime",
				"_zt44_unst_city_town__cellid_lac_calltime_mid",
				"_zt44_unst_city__cellid_lac_callduration",
				"_zt44_unst_city_town__cellid_lac_callduration",
				"_zt44_unst_city_town__cellid_lac_callduration_mid",
				"_zt44_unst_city__cellid_lac_callcost",
				"_zt44_unst_city_town__cellid_lac_callcost",
				"_zt44_unst_city_town__cellid_lac_callcost_mid",
				"_zt44_unst_city_town__cellid_lac_hourofday_epochday",

				"_zt44_unayst_other_party_first_call",
				"_zt44_unayst_other_party_last_call",
				"_zt44_unayst_other_party_day_first_call",
				"_zt44_unayst_other_party_calltime",
				"_zt44_unayst_other_party_callduration",
				"_zt44_unayst_other_party_shortmsg_num",
				"_zt44_unayst_other_party_msisdn",
				"_zt44_unayst_other_party_epchoday_msisdn_calltime",
				"_zt44_unayst_other_party_epchoday_msisdn_callduration",
				"_zt44_unayst_other_party_epchoday_msisdn_shortmsg_num",

				"_zt44_unayst_epochday_dayofweek_calltime",
				"_zt44_unayst_epochday_dayofweek_callduration_level",
				"_zt44_unayst_epochday_dayofweek_shortmsg_num",
				"_zt44_unayst_other_party_carrier_city_town",
				"_zt44_unayst_other_party_town_code",
				"_zt44_unayst_innet_epochday",
				"_zt44_unayst_onnet_epochday",
				"_zt44_unayst_outnet_epochday",
				"_zt44_unayst_city_cellid_lac_calltype_calltime",
				"_zt44_unayst_city_cellid_lac_calltype_callduration",
				"_zt44_unayst_city_cellid_lac_calltype_callcost",
				"_zt44_unayst_city_cellid_lac_calltype_other_party_customer_num",
				"_zt44_unayst_city_cellid_lac_calltype_shortmsg_num",

				"_zt44_unst_city_calltime",
				"_zt44_unst_city_shortmsg_num",
				"_zt44_unst_city_fee",
				"_zt44_unst_city_fee2",
				"_zt44_unst_city_callduration",
				"_zt44_unst_city_callduration2",
				"_zt44_unst_city_data",
				"_zt44_unst_city_data2",

				"_zt44_unst_msisdn_day_first_call",
				"_zt44_unst_msisdn_innet_epochday",
				"_zt44_unst_msisdn_onnet_epochday",
				"_zt44_unst_msisdn_outnet_epochday",
				"_zt44_unayst_innet_st_epochday",
				"_zt44_unayst_onnet_st_epochday",
				"_zt44_unayst_outnet_st_epochday",
				"_zt44_unst_msisdn_innet_st_epochday",
				"_zt44_unst_msisdn_onnet_st_epochday",
				"_zt44_unst_msisdn_outnet_st_epochday",

				"_zt44_unicom_lac_cellid_citycode",
				"_zt44_unst_city_lac_cellid"
			};

			u32 size = sizeof(array) / sizeof(OmnString); 

			// Ketty 2013/07/17
			//u32 phy_num = AosGetNumPhysicals();
			u32 phy_num = AosGetNumPhysicals();
			if (phy_num == 0) phy_num = 1;
			u32 vir_num = AosGetNumCubes();
			u32 virs_per_phy = vir_num / phy_num;

			u32 ss = 1;
			u32 hash_key = 0;
			for(u32 i = 0; i<size; i++)
			{
				hash_key = ss + virs_per_phy * (i % phy_num);
				if((i % phy_num + 1) >= phy_num)
				{
					ss++;
				}
				sgHashKeyMap[array[i]] = hash_key;
				//cout << array[i].data() << ":" << hash_key << endl;
			}
	
			sgHashKeyMapInited = true;
		}
		sgLock.unlock();
	}

	OmnString w(word, len);
	if (sgHashKeyMap.find(w) != sgHashKeyMap.end())
	{
		return sgHashKeyMap[w];	
	}

	/* if (len > 5)
	{
		OmnString d(w.data(), 5);
		if (d == "_zt44")
		{
			OmnScreen << "missing hashkey:" << w << endl; 
		}
	} */

	return 0;
}

