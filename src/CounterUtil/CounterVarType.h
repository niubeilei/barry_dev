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
// This is a utility to select docs.
//
// Modification History:
// 05/30/2011	Created by Lynch Yang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CounterUtil_CounterVarType_h
#define AOS_CounterUtil_CounterVarType_h

#define AOSCTVARNAME_USERID					"userid"
#define AOSCTVARNAME_USERID_LEN				6
#define AOSCTVARNAME_USERCID				"usercid"
#define AOSCTVARNAME_USERCID_LEN			7
#define AOSCTVARNAME_OBJID					"objid"
#define AOSCTVARNAME_OBJID_LEN				5
#define AOSCTVARNAME_DOCID					"docid"
#define AOSCTVARNAME_DOCID_LEN				5
#define AOSCTVARNAME_RUNDATA				"rdata"
#define AOSCTVARNAME_RUNDATA_LEN			5
#define AOSCTVARNAME_DOCVALUE				"docvl"
#define AOSCTVARNAME_DOCVALUE_LEN			5

class AosCounterVarType
{

public:
	enum E
	{
		eInvalid,

		eUserCidVar,
		eUserIdVar,
		eObjIdVar,
		eDocIdVar,
		eRunDataVar,
		eDocValueVar,

		eMax
	};

	bool isValid(const E type)
	{
		return (type > eInvalid && type < eMax);
	}

	static E toEnum(char *str, const int len)
	{
		if (!str) return eInvalid;
		char *p = strstr(str, ":");
		if (!p)
		{
			switch (str[0])
			{
			case 'u':
				 if(AOSCTVARNAME_USERCID_LEN == len && strncmp(str, AOSCTVARNAME_USERCID, len) == 0) 
				 {
					 return eUserCidVar;
				 }

				 if(AOSCTVARNAME_USERID_LEN == len && strncmp(str, AOSCTVARNAME_USERID, len) == 0) 
				 {
					 return eUserIdVar;
				 }
				 break;
	
			case 'o':
				 if(AOSCTVARNAME_OBJID_LEN == len && strncmp(str, AOSCTVARNAME_OBJID, len) == 0) 
				 {
					 return eObjIdVar;
				 }
				 break;
	
			case 'd':
				 if(AOSCTVARNAME_DOCID_LEN == len && strncmp(str, AOSCTVARNAME_DOCID, len) == 0) 
				 {
					 return eDocIdVar;
				 }
				 break;

			default:
				 break;
			}
		}
		else
		{
			int len = p - str;
			if (len <= 0) return eInvalid;
			switch(str[0])
			{
			case 'r': 
				 if(strncmp(str, AOSCTVARNAME_RUNDATA, AOSCTVARNAME_RUNDATA_LEN) == 0) 
				 	return eRunDataVar;
				 break;

			case 'd': return eDocValueVar;
				 if(strncmp(str, AOSCTVARNAME_DOCVALUE, AOSCTVARNAME_DOCVALUE_LEN) == 0) 
					return eDocValueVar;

			default : break;
			}
		}

		OmnAlarm << "Invalid counter variable type: " << str << enderr;
		return eInvalid;
	}
};
#endif

