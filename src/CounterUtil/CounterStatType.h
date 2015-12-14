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
#if 0
#ifndef AOS_CounterUtil_CounterStatType_h
#define AOS_CounterUtil_CounterStatType_h

class AosCounterStatType
{

public:
	enum E
	{
		eInvalid,

		eCounterSum,
		eCounterAver,
		eCounterMax,
		eCounterMin,
		eCounterMean,
		eCounterStderr,
		eCounterNumber,

		eMax
	};

	bool isValid(const E type)
	{
		return (type > eInvalid && type < eMax);
	}

	static E toEnum(const OmnString &str)
	{
		if (str.length() <= 0) return eInvalid;
		const char *data = str.data();
		switch (data[0])
		{
		case 'a':
			 if (str == "aver") return eCounterAver;
		case 'm':
			 if (str == "max")  return eCounterMax;
			 if (str == "min")  return eCounterMin;
			 if (str == "mean") return eCounterMean; 
			 break;

		case 'n':
			 if (str == "number") return eCounterNumber;
			 break;

		case 's':
			 if (str == "standarterr") return eCounterStderr;
			 if (str == "sum") return eCounterSum;
			 break;

		default:
			 break;
		}
		OmnAlarm << "Invalid counter statistics type: " << str << enderr;
		return eInvalid;
	}
};
#endif
#endif
