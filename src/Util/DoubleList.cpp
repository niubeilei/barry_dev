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
// 12/13/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/DoubleList.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


AosDoubleList::AosDoubleList()
{
}


AosDoubleList::~AosDoubleList()
{
}


bool
AosDoubleList::removeFromList()
{
	// This function removes this entry from the list.
	if (mDblListPrevEntry) mDblListPrevEntry->mDblListNextEntry = mDblListNextEntry;
	if (mDblListNextEntry) mDblListNextEntry->mDblListPrevEntry = mDblListPrevEntry;

	mDblListPrevEntry = 0;
	mDblListNextEntry = 0;
	return true;
}


bool
AosDoubleList::moveToFront(const AosDoubleListPtr &entry)
{
	// This function moves 'entry' to be the previous
	// entry of this entry.
	// It is not thread safe!!!!!!!!!!!!!!!!!
	if (this == entry.getPtr()) return true;
	aos_assert_r(entry->mDblListPrevEntry && entry->mDblListNextEntry, false);

	bool rslt = entry->removeFromList();
	aos_assert_r(rslt, false);

	rslt = insertAt(entry);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDoubleList::insertAt(const AosDoubleListPtr &entry)
{
	// This function inserts 'entry' in front of this entry.
	aos_assert_r(entry, false);
	aos_assert_r(mDblListPrevEntry && mDblListNextEntry, false);
	entry->mDblListPrevEntry = mDblListPrevEntry;
	mDblListPrevEntry->mDblListNextEntry = entry;
	mDblListPrevEntry = entry;
	entry->mDblListNextEntry = this;
	return true;
}

