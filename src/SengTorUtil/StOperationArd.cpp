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
// 01/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SengTorUtil/StOperationArd.h"

#include "SengTorUtil/StAccess.h"
#include "XmlUtil/XmlTag.h"

bool 
AosStOperationArd::checkAccess(
		const AosStDocPtr &local_doc,
		const OmnString &opr_id, 
		bool &granted, 
		bool &denied,
		const AosSengTestThrdPtr &thread)
{
	// This function checks whether there are record for 'opr_id'. 
	// If no, 'granted' is false, 'denied' is false.
	// If a record is found, the record will determine the access.
	// 
	// An operation access record consists of one or more sub-records
	// that are keyed by operation id. This function checks whether 
	// there is a record for the operation 'opr_id'. If not, it sets
	// 'granted' to false, 'denied' to false, and returns true.
	//
	// Otherwise, there may be multiple accesses in the found
	// sub-record. For each of the accesses, it checks wehther the operation
	// is granted or denied.
	OprMapItr_t itr = mOperations.find(opr_id);
	if (itr == mOperations.end())
	{
		// Did not find the record.
		granted = false;
		denied = false;
		return true;
	}

	// Found accesses. 
	AosStAccessPtr access = itr->second;
	while (access)
	{
		access->checkAccessByOpr(local_doc, granted, denied, thread);
		if (granted || denied) return true;
		access = access->getNext();
	}

	// Ran all the accesses, but not determined whether to grant or deny.
	// If it is domain operation access record, it may or may not 
	// allow override.
	if (mIsDomainOprArd)
	{
		if (mAllowOverride) 
		{
			denied = false;
			granted = false;
			return true;
		}

		denied = true;
		granted = false;
		return true;
	}

	denied = false;
	granted = false;
	return true;
}

#endif
