////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataScannerType_h
#define Aos_SEInterfaces_DataScannerType_h

#include "Util/String.h"

#define AOSDATASCANNER_INVALID					"invalid"
#define AOSDATASCANNER_DOC						"doc"
#define AOSDATASCANNER_DCL						"dcl"
#define AOSDATASCANNER_FILE						"file"
#define AOSDATASCANNER_DIR						"dir"
//#define AOSDATASCANNER_VIRTUALFILE				"virfile"
#define AOSDATASCANNER_BINARYDOC				"binarydoc"
#define AOSDATASCANNER_UNICOMDIR				"unicomdir"
#define AOSDATASCANNER_DUMMY					"dummy"
#define AOSDATASCANNER_CUBE						"cube"
#define AOSDATASCANNER_PARALLEL					"parallel"

class AosDataScannerType 
{
public:
	enum E
	{
		eInvalid,

		eDoc,
		eFile,
		eDir,
//		eVirtualFile,
		eDcl,
		eBinaryDoc,
		eUnicomDir,
		eJimoScanner,

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	
	static bool isValid(const OmnString &code)
	{
		return isValid(toEnum(code));
	}
	
	static E toEnum(const OmnString &code)
	{
		if (code == AOSDATASCANNER_DOC) return eDoc;
		else if (code == AOSDATASCANNER_BINARYDOC) return eBinaryDoc;
//		else if (code == AOSDATASCANNER_IIL) return eIIL;
		else if (code == AOSDATASCANNER_FILE) return eFile;
		else if (code == AOSDATASCANNER_DIR) return eDir;
		else if (code == AOSDATASCANNER_DCL) return eDcl;
		else if (code == AOSDATASCANNER_UNICOMDIR) return eUnicomDir;
		return eInvalid;
	}
	
	static OmnString toString(const E code)
	{
		switch(code)
		{
		case eDoc: 			return AOSDATASCANNER_DOC;
		case eBinaryDoc:	return AOSDATASCANNER_BINARYDOC;
		case eUnicomDir:	return AOSDATASCANNER_UNICOMDIR;
//		case eIIL: 			return AOSDATASCANNER_IIL;
		default: break;
		}
		return AOSDATASCANNER_INVALID;
	}
	static bool check();
	static bool addName(const OmnString &name, const E code);
};

#endif
