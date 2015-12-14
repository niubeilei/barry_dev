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
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocSelector_SelectorType_h
#define AOS_DocSelector_SelectorType_h



class AosDocSelType
{

public:
	enum E
	{
		eInvalid,

		eCreatedDoc,
		eRetrievedDoc,
		eSourceDoc,
		eTargetDoc,
		eDocToCreate,
		eByObjid,
		eByDocid,
		eByLocalVar,

		eMax
	};

	bool isValid(const E type)
	{
		return (type > eInvalid && type < eMax);
	}

	E toEnum(const OmnString &str)
	{
		if (str.length() <= 0) return eInvalid;

		const char *data = str.data();
		switch (data[0])
		{
		case '1':
			 return eCreatedDoc;

		case '2':
			 return eRetrievedDoc;

		case '3':
			 return eSourceDoc;

		case '4':
			 return eTargetDoc;

		case '5':
			 return eDocToCreate;

		case '6':
			 return eByObjid;

		case '7':
			 return eByDocid;

		case '8':
			 return eByLocalVar;

		default:
			 break;
		}
		OmnAlarm << "Invalid type: " << str << enderr;
		return eInvalid;
	}
};
#endif

