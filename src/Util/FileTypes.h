////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileTypes.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_FileTypes_h
#define Snt_Util_FileTypes_h

#include "Alarm/Alarm.h"
#include "Util/String.h"
// #include "XmlParser/Ptrs.h"


class OmnFileTypes
{
public:
	enum E
	{
		eFirstValidEntry,

		eNoType,

		eText,
		eBinary,

		eLastValidEntry
	};

	enum
	{
		eMaxExtensions = 200
	};

private:
	static OmnString	mExtensions[eMaxExtensions];
	static E			mTypes[eMaxExtensions];
	static OmnString	mTypeStr[eLastValidEntry+1];
	static E			mDefaultType;
	static int			mNumExt;

public:
	static bool config(const OmnXmlParserPtr &parser);
	static OmnString getExtension(const OmnString &filename)
	{
		// 
		// The passed in is a file name. We determine the file extension. If the 
		// file does not have an extension, return "".
		//

		// 
		// Retrieve the file extension. 
		//
		int index = filename.length()-1;
		char *data = filename.getBuffer();
		while (index >= 0 && data[index] != '.')
		{
			index--;
		}

		if (index < 0 || index == filename.length()-1)
		{
			// 
			// Didn't have extension. Return "";
			//
			return "";
		}

		return OmnString(&data[index+1], filename.length() - index - 1);
	}

	static OmnString getFileTypeStr(const OmnString &filename)
	{
		return toStr(getFileType(filename));
	}

	static E getFileType(const OmnString &filename)
	{
		OmnString extension = getExtension(filename);
		for (int i=0; i<mNumExt; i++)
		{
			if (mExtensions[i] == extension)
			{
				// 
				// Found it. 
				//
				return mTypes[i];
			}
		}

		// 
		// didn't find it. 
		//
		return mDefaultType;
	}

	static OmnString toStr(const E e)
	{
		if (e < eFirstValidEntry || e >= eLastValidEntry)
		{
			OmnProgAlarm << "Invalid file type: " << e << enderr;
			return "Invalid";
		}

		return mTypeStr[e];
	}

	static E toEnum(const OmnString &type)
	{
		for (int i=0; i<mNumExt; i++)
		{
			if (mTypeStr[i] == type)
			{
				return (E)i;
			}
		}

		return eNoType;
	}
};
#endif

