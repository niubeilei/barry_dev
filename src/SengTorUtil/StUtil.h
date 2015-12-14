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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StUtil_h
#define AosSengTorUtil_StUtil_h

#include <map>
#include <vector>
#include <ext/hash_map>
#include "SengTorturer/Ptrs.h"
#include "SEBase/SecOpr.h"
#include "XmlUtil/XmlTag.h"
#include "Util/String.h"
#include "Util/HashUtil.h"
#include "SengTorUtil/StType.h"
#include "Rundata/Rundata.h"
#include "Thrift/AosJimoAPI.h"

#define AOSTEST_DOCID 				"sengt_docid" 

/*
namespace std
{
	using namespace __gnu_cxx;
	struct Omn_Str_hash
	{
		size_t operator()(const OmnString& str) const
		{
			// unsigned long __h = 0;
			// for (size_t i=0; i<(size_t)str.length(); i++)
			// 	__h = 5*__h + str[i];
			// return size_t(__h);
			return __stl_hash_string(str.getBuffer());
		}
	};

	struct compare_str
	{
		bool operator()(const OmnString &p1, const OmnString &p2) const
		{
			return p1==p2;
		}
	};  

	struct u64_hash
	{
	    size_t operator()(const u64 i) const
	    {
	        unsigned long __h = 0;
			__h = i >> 32;
	        __h +=(int)&__h + i;
	        return size_t(__h);
	    }
	};

	struct u64_cmp
	{
	    bool operator()(const u64 &p1, const u64 &p2) const
	    {
	        return p1==p2;
	    }
	};
};
*/

using namespace std;


struct AosStUser
{
	u64 				locdid;
	u64 				svrdid;
	OmnString 			objid;
	u64					parent_locdid;
	OmnString			cid;
	OmnString			username;
	OmnString			passwd;
	OmnString			groups;
};

struct Session
{
	OmnString 			sessionId;
	OmnString 			cid;
	AosXmlTagPtr 		session;
	u64					userdocid;
};

struct AosStAccessRecord 
{
	enum
	{
		eKeepPublic = 10
	};

	u64					mOwnLocdid;  	//own local docid 
	u64		 			mOwnSvrdid;	//Access Record objid
	u64					mSvrdid;  	//own server docid

//	OmnString			mAccesses[AosSecOpr::eMax][AosAccessType::eMax];
	AosAccessType::E			mAccesses[AosSecOpr::eMax][AosAccessType::eMax];
	int							mNumAcc[AosSecOpr::eMax];
	static AosAccessType::E		mDftAccesses[AosSecOpr::eMax][AosAccessType::eMax];
	static bool					mDftInited;

	AosStAccessRecord()
	{
		if (!mDftInited) initDefault();
		for (int i = 0; i<AosSecOpr::eMax; i++)
		{
			for (int k = 0; k<AosAccessType::eMax; k++)
			{
				AosAccessType::E type = AosAccessType::eInvalid;
				mAccesses[i][k] = type; 
			}
		}
		for(int opr = AosSecOpr::eRead; opr<AosSecOpr::eMax; opr++)
		{
			mNumAcc[opr] = 0;
		}
	}

	static void initDefault()
	{
		for (int i = AosSecOpr::eInvalid+1; i<AosSecOpr::eMax; i++)
		{
			OmnString oop = AosSecOpr::getDftAccess((AosSecOpr::E)i);
			mDftAccesses[i][0] = AosAccessType::toEnum(oop);
			mDftAccesses[i][1] = AosAccessType::eInvalid;
		}
		mDftAccesses[AosSecOpr::eInvalid][0] = AosAccessType::eInvalid;
		mDftAccesses[AosSecOpr::eMax][0] = AosAccessType::eInvalid;
	}

	bool removeAccess(const AosSecOpr::E opr, const AosAccessType::E type);
	bool modifyAccess(const AosSecOpr::E opr, const AosAccessType::E type);
	bool normalizeAccess();
	bool normalizeAccess(const AosSecOpr::E opr);
	bool addAccess(const AosSecOpr::E opr, const AosAccessType::E type)
	{
		aos_assert_r(AosSecOpr::isValid(opr), false);
		aos_assert_r(AosAccessType::isValid(type), false);
		for (int i = 0; i< AosAccessType::eMax; i++)
		{
			if (!AosAccessType::isValid(mAccesses[opr][i]))
			{
				mAccesses[opr][i] = type; 
				mNumAcc[opr]++;
				return true;
			}
		}
		return false;
	}

	bool isSame(AosStAccessRecord *local_ard);
	bool isAccFull(const AosSecOpr::E opr)
	{
		return (mNumAcc[opr]>=AosSecOpr::eMax);
	};

	bool setXml(const AosXmlTagPtr &xml);

	bool canDoOpr(
			const AosSecOpr::E opr, 
			const OmnString &owner_cid,
			const OmnString &crt_cid)
	{
		return canDoOpr(opr, owner_cid, crt_cid, mAccesses);
	}

	static bool canDoOprDefault(
			const AosSecOpr::E opr, 
			const OmnString &owner_cid,
			const OmnString &crt_cid)
	{
		return canDoOpr(opr, owner_cid, crt_cid, mDftAccesses);
	}

	static bool canDoOpr(
			const AosSecOpr::E opr, 
			const OmnString &owner_cid,
			const OmnString &crt_cid,
			const OmnString accs[AosSecOpr::eMax][AosAccessType::eMax])
	{
		return false;
	}

	static bool canDoOpr(
			const AosSecOpr::E opr, 
			const OmnString &owner_cid,
			const OmnString &crt_cid,
			AosAccessType::E accs[AosSecOpr::eMax][AosAccessType::eMax])
	{
		aos_assert_r(AosSecOpr::isValid(opr), false);
		if (accs[opr][0] == AosAccessType::eInvalid)
		{
			accs[opr][0] = AosAccessType::toEnum(AosSecOpr::getDftAccess(opr));
		}

		int idx = 0;
		while (idx < AosAccessType::eMax)
		{
			AosAccessType::E type = accs[opr][idx++];
			switch (type)
			{
			case AosAccessType::ePrivate:
				 if (crt_cid == owner_cid) return true;
				 break;

			case AosAccessType::eGroup:
			case AosAccessType::eRole:
				 return false;
				 OmnNotImplementedYet;
				 //return false;

			case AosAccessType::ePublic:
				 return true;

			case AosAccessType::eLoggedPublic:
				 if (crt_cid != "") return true;
				 break;

			case AosAccessType::eTimedPublic:
				 return true;
				 OmnNotImplementedYet;
				 //return false;

			case AosAccessType::eInvalid:
				 // Run out of the accesses. Return false.
				 return false;

			default:
				 OmnAlarm << "Unrecognized access type: " << type << enderr;
				 return false;
			}
		}
		return false;
	}
};


class AosStUtil
{
public:
	static AosSecOpr::E getSecOpr() {return (AosSecOpr::E)(rand() % (AosSecOpr::eMax - 2) + 1);}
};
#endif

