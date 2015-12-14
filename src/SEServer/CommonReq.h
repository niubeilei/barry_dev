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
// 01/04/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEServer_CommonReq_h
#define AOS_SEServer_CommonReq_h

#include "Debug/Debug.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"


class AosSeReqProc;

class AosCommonReq
{
public:
	static bool getFileNames(
					AosSeReqProc *proc,
					const AosWebRequestPtr &req, 
					const AosXmlTagPtr &child); 
	bool 		createComment(
					const AosXmlTagPtr &cmd,
					OmnString &contents,
					OmnString &errmsg,
					AosXmlRc &errcode);
};
#endif

