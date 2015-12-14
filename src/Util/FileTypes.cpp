////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileTypes.cpp
// Description:
//	This class maintains a list of known file extensions and provide utility
//  functions to manipulate these extensions. 
//
//	Members:
//		mExtensions[eMaxExtensions]:	the list of known extensions (such as 'txt', 'doc')
//		mTypes[eMaxExtensions];			the enum value of the types corresponding to the 
//										extension in mExtensions[].
//		mTypeStr[eLastValidEntry]:		the string value of the enum E.
//		mDefaultType:					The default type (currently, eNoType)
//		mNumExt:						The number of known extensions. 
//
//	How-To:
//		One should call "config(...)" at the application initialization stage
//		to initialize the data, i.e., define the extensions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/FileTypes.h"


OmnString		OmnFileTypes::mExtensions[OmnFileTypes::eMaxExtensions];
OmnFileTypes::E	OmnFileTypes::mTypes[OmnFileTypes::eMaxExtensions];
OmnFileTypes::E OmnFileTypes::mDefaultType = OmnFileTypes::eNoType;
int				OmnFileTypes::mNumExt = 0;
OmnString		OmnFileTypes::mTypeStr[OmnFileTypes::eLastValidEntry+1] = 
				{
					"notype",
					"notype",

					"text",
					"binary",

					"notype"
				};


bool
OmnFileTypes::config(const OmnXmlParserPtr &parser)
{
	// 
	// It assumes:
	//		<FileExtDefs>
	//			<DefaultType>
	//			<DefList>
	//				<FileExtDef>
	//					<FileExtension>
	//					<FileType>
	//				</FileExtDef>
	//				...
	//			</DefList>
	//		</FileExtDefs>
	//

	if (!parser)
	{
		return true;
	}

	OmnXmlItemPtr defs = parser->tryItem("FileExtDefs");
	if (!defs)
	{
		// 
		// No definition. 
		//
		return true;
	}

	OmnXmlItemPtr list = defs->tryItem("DefList");
	if (list)
	{
		mNumExt = 0;
		list->reset();
		while (list->hasMore())
		{
			OmnXmlItemPtr def = list->next();
			OmnString extension = def->getStr("FileExtension", "");
			if (extension == "")
			{
				// 
				// Incorrect.
				//
				OmnAlarm << "File extension is missing: " << list->toString() << enderr;
				return false;
			}

			OmnString type = def->getStr("FileType", "");
			type.toLower();
			E typeFound = eNoType;
			for (int i=0; i<eLastValidEntry; i++)
			{
				if (mTypeStr[i] == type)
				{
					typeFound = (E) i;
					break;
				}
			}

			if (typeFound == eNoType)
			{
				OmnAlarm << "File type not defined: " << type << ". " << list->toString() << enderr;
				return false;
			}

			mExtensions[mNumExt] = extension;
			mTypes[mNumExt] = typeFound;
			mNumExt++;
		}
	}

	// 
	// Retrieve the default type
	//
	OmnString defaultType = defs->getStr("DefaultType", "");
	if (defaultType != "")
	{
		bool found = false;
		for (int i=0; i<eLastValidEntry; i++)
		{
			if (mTypeStr[i] == defaultType)
			{
				found = true;
				mDefaultType = (E)i;
				break;
			}
		}

		if (!found)
		{
			// 
			// The default type is incorrect
			//
			OmnAlarm << "Default type incorrect: " << defaultType << ". " << defs->toString() << enderr;
			return false;
		}
	}

	return true;
}
#endif
