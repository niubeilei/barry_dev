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
//
// Modification History:
// 01/02/2011 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/StUtil.h"

#include "Random/RandomUtil.h"
#include "SEBase/SecOpr.h"


AosAccessType::E AosStAccessRecord::mDftAccesses[AosSecOpr::eMax][AosAccessType::eMax];
bool			 AosStAccessRecord::mDftInited = false;
/*
bool 
AosStAccessRecord::addAccess(
		const AosSecOpr::E opr,
		const AosAccessType acctype)
{
	// This function adds 'acctype' to the operator 'opr'. 
	// If there are already too many mAccesses, it will 
	// randomly replace one.
	aos_assert_r(AosSecOpr::isValid(opr), false);
	for (int i=0; i<eAosAcsType_Max; i++)
	{
		if (mAccesses[opr][i] == AosSecOpr::eInvalid)
		{
			// Found the first invalid one. Add it.
			mAccesses[opr][i] = acctype;
			return true;
		}
	}

	// It is full. Randomly pick one and replace it.
	int idx = rand() % eAosAcsType_Max;
	mAccesses[opr][idx] = acctype;
	return true;
}
*/


bool 
AosStAccessRecord::removeAccess(const AosSecOpr::E opr, const AosAccessType::E type)
{
	// This function removes an access for the opoerator 'opr'.
	int num = 0;
	for (int i=0; i<AosAccessType::eMax; i++)
	{
		if (mAccesses[opr][i] == AosAccessType::eInvalid) break;
		num++;
	}

	if (num <= 0)
	{
		// There are no mAccesses to remove. Do nothing.
		return true;
	}

	// Randomly pick one and remove it.
	int idx = rand() % num;
	if (num == 1)
	{
		mAccesses[opr][0] = AosAccessType::eInvalid;
		return true;
	}

	mAccesses[opr][idx] = mAccesses[opr][num-1];
	mAccesses[opr][num-1] = AosAccessType::eInvalid;
	return true;
}


bool 
AosStAccessRecord::modifyAccess(
		const AosSecOpr::E opr, 
		const AosAccessType::E type)
{
	// This function modifies an access
	int num = 0;
	for (int i=0; i<AosAccessType::eMax; i++)
	{
		if (mAccesses[opr][i] == AosAccessType::eInvalid) break;
		num++;
	}

	if (num <= 0)
	{
		// There are no mAccesses to remove. Add it
		mAccesses[opr][0] = type;
		return true;
	}

	// Randomly pick one and remove it.
	int idx = rand() % num;
	mAccesses[opr][idx] = type;
	return true;
	
}


bool 
AosStAccessRecord::normalizeAccess()
{
	// When mAccesses are modified, such as adding, deleting, or 
	// modifying, we did not check the logical relationships.
	// This function normalizes the mAccesses.
	for (int i=AosSecOpr::eRead; i<AosSecOpr::eMax; i++)
	{
		aos_assert_r(normalizeAccess((AosSecOpr::E)i), false);
	}
	return true;
}


bool
AosStAccessRecord::normalizeAccess(const AosSecOpr::E opr)
{
	bool accs[AosAccessType::eMax];
	for (int i=0; i<AosAccessType::eMax; i++)
	{
		accs[i] = false;
	}

	// 1. First remove duplicated entries
	for (int i=0; i<AosAccessType::eMax; i++)
	{
		if (mAccesses[opr][i] > AosAccessType::eInvalid &&
			mAccesses[opr][i] < AosAccessType::eMax)
		{
			accs[mAccesses[opr][i]] = true;
		}
		else
		{
			break;
		}
	}

	// 2. Normalize it.
	if (accs[AosAccessType::ePrivate])
	{
		// If there are no public, do nothing. Otherwise, 
		// Either remove the private but keep 
		// the public or keep the public and remove the private.
		if (accs[AosAccessType::ePublic] ||
			accs[AosAccessType::eLoggedPublic] ||
			accs[AosAccessType::eTimedPublic])
		{
			if (!OmnRandom::percent(eKeepPublic))
			{
				accs[AosAccessType::ePublic] = false;
				accs[AosAccessType::eLoggedPublic] = false;
				accs[AosAccessType::eTimedPublic] = false;
			}
			else
			{
				accs[AosAccessType::ePrivate] = false;
			}
		}
	}

	if (accs[AosAccessType::ePublic] ||
		accs[AosAccessType::eLoggedPublic] ||
		accs[AosAccessType::eTimedPublic])
	{
		// Either remove the public or remove group and role
		if (OmnRandom::percent(eKeepPublic))
		{
			accs[AosAccessType::eGroup] = false;
			accs[AosAccessType::eRole] = false;
		}
		else
		{
			if (accs[AosAccessType::eGroup] ||
				accs[AosAccessType::eRole])
			{
				accs[AosAccessType::ePublic] = false;
				accs[AosAccessType::eLoggedPublic] = false;
				accs[AosAccessType::eTimedPublic] = false;
			}
		}
	}

	// Putting them together
	int idx = 0;
	for (int i=AosAccessType::ePrivate; i<AosAccessType::eMax; i++)
	{
		if (accs[i])
		{
			mAccesses[opr][idx++] = (AosAccessType::E)i;
			if (idx >= AosAccessType::eMax) break;
		}
	}

	if (idx < AosAccessType::eMax) mAccesses[opr][idx] = AosAccessType::eInvalid;
	//if (idx < eAosAcsType_Max) mAccesses[idx][0] = eAosAcsType_Invalid;
	return true;
}


bool
AosStAccessRecord::isSame(AosStAccessRecord *local_ard)
{
	aos_assert_r(mOwnLocdid == local_ard->mOwnLocdid, false);
	aos_assert_r(mOwnSvrdid == local_ard->mOwnSvrdid, false);
	aos_assert_r(mSvrdid == local_ard->mSvrdid, false);

	for(int i = AosSecOpr::eRead; i < AosSecOpr::eMax; i++)
	{
		if (local_ard->mAccesses[i][0] == AosAccessType::eInvalid)
		{
			local_ard->mAccesses[i][0] = AosAccessType::toEnum(
				AosSecOpr::getDftAccess((AosSecOpr::E)i));
		}

		if (mAccesses[i][0] == AosAccessType::eInvalid)
		{
			mAccesses[i][0] = AosAccessType::toEnum(
				AosSecOpr::getDftAccess((AosSecOpr::E)i));
		}

		for (int k=0; k<AosAccessType::eMax; k++)
		{
			aos_assert_r(mAccesses[i][k] ==  local_ard->mAccesses[i][k], false);
		}
	}
	return true;
}


bool
AosStAccessRecord::setXml(const AosXmlTagPtr &xml)
{
	aos_assert_r(xml, false);
	AosAccessType::E type;
	OmnString access;
	for (int opr = AosSecOpr::eRead; opr<AosSecOpr::eMax; opr++)
	{
		OmnString opp = AosSecOpr::toString((AosSecOpr::E)opr);
		aos_assert_r(opp!="", false);
		aos_assert_r(AosSecOpr::isValid((AosSecOpr::E)opr), false);
		for (int k=0; k<AosAccessType::eMax; k++)
		{
			type = mAccesses[opr][k];

			if (AosAccessType::isValid(type))
			{	
				//aos_assert_r(opp != "", false);
				//access = xml->getAttrStr(opp);
				if (access!="") access <<", "<<AosAccessType::toString(type);
				else access << AosAccessType::toString(type);
				//xml->setAttr(opp, access);

			}
		}
		xml->setAttr(opp, access);
		if (access=="" && xml->getAttrStr(opp)!="") xml->removeAttr(opp);
		access = "";
	}
	return true;
	/*
	for (int opr = AosSecOpr::eRead; opr<AosSecOpr::eMax; opr++)
	{
		aos_assert_r(AosSecOpr::isValid((AosSecOpr::E)opr), false);
		for (int k=0; k<eAosAcsType_Max; k++)
		{
			type = mAccesses[opr][k];
			if (AosIsAccTypeValid(type))
			{	
				OmnString opp = AosSecOpr::toString((AosSecOpr::E)opr);
				aos_assert_r(opp != "", false);
				access = xml->getAttrStr(opp);
				if (access!="") access <<", "<<AosAccessType_Code2Str(type);
				else access << AosAccessType_Code2Str(type);
				xml->setAttr(opp, access);
				access = "";
			}
		}
	}
	*/
}




