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
// 01/18/2012 Moved from Util/ by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_CommonValues_h
#define Omn_Random_CommonValues_h

#include "Random/RandomUtil.h"
#include "Util/String.h"
#include <vector>
using namespace std;


class AosCommonValues
{
	struct ValueGroup
	{
		enum
		{
			eFirstGroup = 10,
			eSecondGroup = 30,
			eThirdGroup = 100,
			eNumEntries = 1000
		};
	
		OmnString	mValues[eNumEntries];
	
		ValueGroup(const OmnString &prefix)
		{
			for (int i=0; i<eNumEntries; i++)
			{
				OmnString str = prefix;
				mValues[i] << str << "_" << i;
			}
		}

		~ValueGroup() {}
	
		OmnString pickValue()
		{
			int idx = OmnRandom::intByRange(
						0, 				eFirstGroup, 	50,
						eFirstGroup+1, 	eSecondGroup, 	20,
						eSecondGroup+1, eThirdGroup, 	10,
						eThirdGroup, 	eNumEntries-1, 	5, 
						eNumEntries, 	eNumEntries, 	3);

			if (idx == eNumEntries)
			{
				return OmnRandom::word30();
			}
			return mValues[idx];
		}

		OmnString pickValues(const OmnString &sep)
		{
			int nn = OmnRandom::intByRange(1, 5, 60, 6, 10, 30, 11, 50, 10);
			int total = 0;
			OmnString str;
			for (int i=0; i<nn; i++)
			{
				if (total++ != 0) str << sep;
				str << mValues[OmnRandom::nextU32(0, eNumEntries-1)];
			}
			return str;
		}

		bool pickUniqueValues(vector<OmnString> &values)
		{
			values.clear();
			int nn = OmnRandom::intByRange(
							1, 5, 60, 
							6, 10, 30, 
							11, 50, 10);
			char map[eNumEntries];
			memset(map, 0, sizeof(map));
			int total = 0;
			while (total < nn)
			{
				int idx = OmnRandom::nextU32(0, eNumEntries-1);
				if (!map[idx])
				{
					map[idx] = 1;
					values.push_back(mValues[idx]);
					total++;
				}
			}
			return true;
		}
		OmnString pickUniqueValues(const OmnString &sep)
		{
			int nn = OmnRandom::intByRange(1, 5, 60, 6, 10, 30, 11, 50, 10);
			char map[eNumEntries];
			memset(map, 0, sizeof(map));
			int total = 0;
			OmnString str;
			while (total < nn)
			{
				int idx = OmnRandom::nextU32(0, eNumEntries-1);
				if (!map[idx])
				{
					map[idx] = 1;
					if (total != 0) str << sep;
					str << mValues[idx];
					total++;
				}
			}
			return str;
		}
	};

private:
	static ValueGroup		smAttrnames;
	static ValueGroup		smAttrvalues;
	static ValueGroup		smDocNames;
	static ValueGroup		smOprCodes;
	static ValueGroup		smUserGroups;
	static ValueGroup		smUserRoles;
	static ValueGroup		smObjids;
	static ValueGroup		smUserDomains;
	static ValueGroup		smXpaths;
	static ValueGroup		smOperations;

public:
	static OmnString pickAttrName() {return smAttrnames.pickValue();}
	static OmnString pickAttrNames(const OmnString &s) {return smAttrnames.pickValues(s);}

	static OmnString pickAttrValue() {return smAttrvalues.pickValue();}
	static OmnString pickAttrValues(const OmnString &s) {return smAttrvalues.pickValues(s);}

	static OmnString pickDocName() {return smDocNames.pickValue();}
	static OmnString pickDocNames(const OmnString &s) {return smDocNames.pickValues(s);}

	static OmnString pickOprCode() {return smOprCodes.pickValue();}
	static bool pickUniqueOprCodes(vector<OmnString> &codes) 
	{
		return smOprCodes.pickUniqueValues(codes);
	}

	static OmnString pickUserGroup() {return smUserGroups.pickValue();}
	static OmnString pickUserGroups(const OmnString &s) {return smUserGroups.pickValues(s);}

	static OmnString pickUserRole() {return smUserRoles.pickValue();}
	static OmnString pickUserRoles(const OmnString &s) {return smUserRoles.pickValues(s);}

	static OmnString pickObjid() {return smObjids.pickValue();}
	static OmnString pickObjids(const OmnString &s) {return smObjids.pickValues(s);}

	static OmnString pickUserDomain() {return smUserDomains.pickValue();}
	static OmnString pickUserDomains(const OmnString &s) {return smUserDomains.pickValues(s);}

	static OmnString pickXpath();
	static OmnString pickOperation() {return smOperations.pickValue();}
};

#endif
