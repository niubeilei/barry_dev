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
// 04/29/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/XmlRc.h"

#include "Util/String.h"

OmnString AosXmlInt_GetErrmsg(const AosXmlRc code)
{
	switch (code)
	{
	case eAosXmlInt_Ok:
		 return "200 OK";

	case eAosXmlInt_InternalError:
		 return "Internal error";

	case eAosXmlInt_General:
		 return "General error";

	case eAosXmlInt_SyntaxError:
		 return "Syntax error";

	case eAosXmlInt_IncorrectOperation:
		 return "Incorrect Operation";

	case eAosXmlInt_IncorrectObjType:
		 return "Incorrect object type";

	case eAosXmlInt_ObjectExist:
		 return "Object Exist";

	case eAosXmlInt_ObjectNotFound:
		 return "Object not found";

	case eAosXmlInt_DbError:
		 return "Database error";

	case eAosXmlInt_FileNotFound:
		 return "File not found";

	case eAosXmlInt_FailedOpenFile:
		 return "Failed to open file";

	case eAosXmlInt_Denied:
		 return "Denied";

	// Chen Ding, 2013/07/27
	// case eAosXmlInt_ObjidChanged:
	// 	 return "Objid changed";

	case eAosXmlInt_ObjidNotUnique:
		 return "Objid not unique";

	default:
		 break;
	}

	return "Unrecognized Error Code";
}


