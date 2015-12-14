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
// A File with Backup is a reliable file. It creates two or more files.
// These files are kept bit-to-bit identical all the time. 
//
// The class is configured with a number of locations. Each location
// serves as a posibble copy of the file. 
//
// When modifying one file, it 
// automatically modifies all others. When read a file, it reads the 
// first one. If the first one fails, it reads the next one.
//
// When one file fails in writing, that file is marked as bad and
// the class tries to repair by copying the good file into a new 
// location. After that, 
// it reads the file character by character, and compars with the good one.
// If it failed, it try the next one. If all locations are tried but still 
// not being able to repair, the class degrades itself by removing the 
// bad one.
//
// File Name Management
// File name is broken in two parts: Location and Relative File Name (RFN).
// Location is a directory name. This can be different on the primary and
// the backups. RFNs are the remaining of a file name. It is the same on
// both the primary and all the backups. The class assumes it may use Samba
// to communicate with backups or through TCP. 
//
// Modification History:
// 10/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/FileWBack.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosFileWBack::AosFileWBack()
:
mLock(OmnNew OmnMutex())
{
}


AosFileWBack::AosFileWBack(const AosXmlTagPtr &def)
:
mLock(OmnNew OmnMutex()),
mCopies(0),
mNumActives(0)
{
	if (!init(def))
	{
		OmnString errmsg = "Failed to create the file: ";
		errmsg << def->toString();
		OmnExcept e(OmnFileLine, errmsg);
		throw e;
	}
}


bool
AosFileWBack::init(const AosXmlTagPtr &def)
{
	// 	<config AOSCONFIG_FNAME="xxx"
	// 		AOSCONFIG_COPIES="xxx"
	// 		AOSCONFIG_CREATE="true">
	//		<locations>
	//			<record>xxx</record>
	//			<record>xxx</record>
	//			...
	//		</location>
	//	</config>
	aos_assert_r(def, false);

	mFname = def->getAttrStr(AOSCONFIG_FNAME);
	aos_assert_r(mFname != "", false);

	mCopies = def->getAttrInt(AOSCONFIG_COPIES, -1);
	aos_assert_r(mCopies > 0, false);

	mCreate = def->getAttrStr(AOSCONFIG_CREATE) == "true";

	AosXmlTagPtr locations = def->getFirstChild(AOSCONFIG_LOCATIONS);
	aos_assert_r(locations, false);

	AosXmlTagPtr loc = locations->getFirstChild();
	while (loc)
	{
		mLocations.push_back(loc->getNodeText());
		loc = locations->getNextChild();
	}

	bool rslt = init();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosFileWBack::init()
{
	aos_assert_r(mLocations.size() > 0, false);
	if (mCopies < 1) mCopies = 1;

	for (int i=0; i<mCopies; i++)
	{
		OmnFilePtr ff = openFile(i AosMemoryCheckerArgs);
		if (!ff) continue;
		mActiveFiles[mNumActives] = ff;
		mActiveLocs[mNumActives] = i;
		mNumActives++;
	}

	aos_assert_r(mNumActives > 0, false);
	return true;
}


bool
AosFileWBack::consistenceCheck()
{
	// This function checks whether all the files are identical.
	// It assumes the caller has locked the lock.
	aos_assert_r(mNumActives > 0, false);
	if (mNumActives == 1) return true;

	for (int i=1; i<mNumActives; i++)
	{
		if (!mActiveFiles[0]->verifyFile(mActiveFiles[i]))
		{
			return false;
		}
	}

	aos_assert_r(consistenceCheck(), false);
	return true;
}


AosFileWBack::~AosFileWBack()
{
}


bool
AosFileWBack::repairFile(const int badidx)
{
	// One of the file failed and was removed from the active ones.
	// This function tries to repair the file by copying the good 
	// one to a new location, if any. If there are no new locations
	// or no good one, the class fails. 
	//
	// If a new copy is created, it reads the contents character by
	// character and compares the contents with the good one. 
	//
	// The class should have been locked when this function is called.
	aos_assert_r(mNumActives > 0, false);

	OmnScreen << "Repare file: " << badidx << ":" << mLocations[badidx].data()
		<< ":" << mFname.data() << endl;

	aos_assert_r(mNumActives > 0, false);
	if (mNumActives == 1)
	{
		aos_assert_r(badidx == 0, false);
		mNumActives = 0;
		OmnAlarm << "Failed to repair!" << enderr;
		return false;
	}

	if (badidx != mNumActives-1)
	{
		mActiveFiles[badidx] = mActiveFiles[mNumActives-1];
	}
	mNumActives--;

	// 1. Flush the good one
	OmnFilePtr goodone = mActiveFiles[0];
	goodone->flushFileContents();

	OmnString goodfname = mLocations[mActiveLocs[0]];
	goodfname << "/" << mFname;

	for (u32 i=0; i<mLocations.size(); i++)
	{
		if (locationUsed(i)) continue;

		OmnString fname = mLocations[i];
		fname << "/" << mFname;
		OmnString command = "cp ";
		command << goodfname << " " << fname;
		OmnScreen << "Copy file: " << command.data()<< endl;
		system(command.data());

		OmnFilePtr ff = openFile(i AosMemoryCheckerArgs);
		if (ff && ff->isGood())
		{
			// Found a good one. 
			mActiveFiles[mNumActives] = ff;
			mActiveLocs[mNumActives] = i;
			mNumActives++;
			OmnScreen << "Repared file: " << i << ":" << mLocations[i].data()
				<< ":" << mFname.data()<< endl;
			return true;
		}
	}

	OmnAlarm << "Failed to repair the file: " << mLocations[badidx] << enderr;
	return false;
}


bool 		
AosFileWBack::put(
		const u64 &offset, 
		const char *data, 
		const int len, 
		const bool flush)
{
	mLock->lock();
	for (int i=0; i<mNumActives; i++)
	{
		bool rslt = mActiveFiles[i]->put(offset, data, len, flush);
		if (!rslt)
		{
			OmnAlarm << "Failed to write file: " << i 
				<< mLocations[mActiveLocs[i]] << ":" << mFname
				<< ". Will try to repair!" << enderr;
			rslt = repairFile(i);
			i--;
		}
	}

	int num_writes = mNumActives;
	mLock->unlock();
	aos_assert_r(num_writes > 0, false);
	return true;
}


bool
AosFileWBack::readBinaryInt64(
		const u64 &offset, 
		int64_t &value,
		const int64_t &dft, 
		const int64_t &min, 
		const int64_t &max)
{
	// It reads an integer from the file. To ensure the correctness, it 
	// will read two files and the results must be the same. Otherwise,
	// the one that is out of the range is considered bad. If both are
	// out of the range, it will try all others. If all are out of 
	// the range, it fails.
	mLock->lock();
	aos_assert_rl(mNumActives > 0, mLock, false);
	value = mActiveFiles[0]->readBinaryInt64(offset, dft);
	if (mNumActives == 1)
	{
		mLock->unlock();
		return true;
	}

	int64_t value2 = mActiveFiles[1]->readBinaryInt64(offset, dft);
	if (value == value2)
	{
		mLock->unlock();
		return true;
	}

	OmnAlarm << "Contents mismatch: " << value << ":" 
		<< value2 << ":" << offset << enderr;
	int64_t values[eMaxCopies];
	values[0] = value;
	values[1] = value2;
	for (int i=2; i<mNumActives; i++)
	{
		values[i] = mActiveFiles[i]->readBinaryInt64(offset, dft);
	}

	for (int i=0; i<mNumActives; i++)
	{
		OmnScreen << "Values: " << i << ":" << values[i] << endl;
	}

	// Repair it, if possible:
	// If all but one are in the range and all the ones that are in
	// the range are the same, the one that is not in the range is bad.
	int badidx = -1;
	for (int i=0; i<mNumActives; i++)
	{
		if (values[i] < min || values[i] > max)
		{
			if (badidx >= 0)
			{
				// There are more than one that is not in the range. 
				// We cannot determine which one is bad one. 
				OmnAlarm << "Unable to repair!" << enderr;
				mLock->unlock();
				return false;
			}
			badidx = i;
		}
	}

	aos_assert_rl(badidx < mNumActives, mLock, false);

	if (badidx < 0)
	{
		// All are in the range. 
		OmnAlarm << "Unable to repair!" << enderr;
		mLock->unlock();
		return false;
	}

	// All but 'badidx' is are in the range. Check whether all other values
	// are the same
	value = (badidx == 0)?values[1]:values[0];
	for (int i=0; i<mNumActives; i++)
	{
		if (i != badidx && value != values[i])
		{
			OmnAlarm << "Unable to repair!" << enderr;
			mLock->unlock();
			return false;
		}
	}

	// Identified the bad one
	OmnAlarm << "Identified the bad one: " 
		<< badidx << ":" << mLocations[mActiveLocs[badidx]]
		<< ":" << mFname << enderr;
	repairFile(badidx);
	mLock->unlock();
	return true;
}


bool
AosFileWBack::readBinaryU64(
		const u64 &offset, 
		u64 &value, 
		const u64 &dft, 
		const u64 &min, 
		const u64 &max)
{
	// It reads an integer from the file. To ensure the correctness, it 
	// will read two files and the results must be the same. Otherwise,
	// the one that is out of the range is considered bad. If both are
	// out of the range, it will try all others. If all are out of 
	// the range, it fails.
	mLock->lock();
	value = dft;
	aos_assert_rl(mNumActives > 0, mLock, false);
	value = mActiveFiles[0]->readBinaryU64(offset, dft);
	if (mNumActives == 1)
	{
		mLock->unlock();
		return true;
	}

	u64 value2 = mActiveFiles[1]->readBinaryU64(offset, dft);
	if (value == value2)
	{
		mLock->unlock();
		return true;
	}

	OmnAlarm << "Contents mismatch: " << value 
		<< ":" << value2 << ":" << offset << enderr;
	u64 values[eMaxCopies];
	values[0] = value;
	values[1] = value2;
	for (int i=2; i<mNumActives; i++)
	{
		values[i] = mActiveFiles[i]->readBinaryU64(offset, dft);
	}

	for (int i=0; i<mNumActives; i++)
	{
		OmnScreen << "Values: " << i << ":" << values[i] << endl;
	}

	// Repair it, if possible:
	// If all but one are in the range and all the ones that are in
	// the range are the same, the one that is not in the range is bad.
	int badidx = -1;
	for (int i=0; i<mNumActives; i++)
	{
		if (values[i] < min || values[i] > max)
		{
			if (badidx >= 0)
			{
				// There are more than one that is not in the range. 
				// We cannot determine which one is bad one. 
				OmnAlarm << "Unable to repair!" << enderr;
				mLock->unlock();
				return false;
			}
			badidx = i;
		}
	}

	aos_assert_rl(badidx < mNumActives, mLock, false);

	if (badidx < 0)
	{
		// All are in the range. 
		OmnAlarm << "Unable to repair!" << enderr;
		mLock->unlock();
		return false;
	}

	// All but 'badidx' is are in the range. Check whether all other values
	// are the same
	value = (badidx == 0)?values[1]:values[0];
	for (int i=0; i<mNumActives; i++)
	{
		if (i != badidx && value != values[i])
		{
			OmnAlarm << "Unable to repair!" << enderr;
			mLock->unlock();
			return false;
		}
	}

	// Identified the bad one
	OmnAlarm << "Identified the bad one: " 
		<< badidx << ":" << mLocations[mActiveLocs[badidx]]
		<< ":" << mFname << enderr;
	repairFile(badidx);
	mLock->unlock();
	return true;
}


bool		
AosFileWBack::setInt(const u64 &offset, 
					const int value, 
					const bool flushflag)
{
	mLock->lock();
	for (int i=0; i<mNumActives; i++)
	{
		bool rslt = mActiveFiles[i]->setInt(offset, value, flushflag);
		if (!rslt)
		{
			OmnAlarm << "Failed to write file: " << i 
				<< mLocations[mActiveLocs[i]] << ":" << mFname
				<< ". Will try to repair!" << enderr;
			rslt = repairFile(i);
			i--;
		}
	}

	int num_writes = mNumActives;
	mLock->unlock();
	aos_assert_r(num_writes > 0, false);
	return true;
}


bool		
AosFileWBack::setU32(const u64 &offset, 
					const u32 value, 
					const bool flushflag)
{
	mLock->lock();
	for (int i=0; i<mNumActives; i++)
	{
		bool rslt = mActiveFiles[i]->setU32(offset, value, flushflag);
		if (!rslt)
		{
			OmnAlarm << "Failed to write file: " << i 
				<< mLocations[mActiveLocs[i]] << ":" << mFname
				<< ". Will try to repair!" << enderr;
			rslt = repairFile(i);
			i--;
		}
	}

	int num_writes = mNumActives;
	mLock->unlock();
	aos_assert_r(num_writes > 0, false);
	return true;
}


bool		
AosFileWBack::setU64(const u64 &offset, 
					const u64 &value, 
					const bool flushflag)
{
	mLock->lock();
	for (int i=0; i<mNumActives; i++)
	{
		bool rslt = mActiveFiles[i]->setU64(offset, value, flushflag);
		if (!rslt)
		{
			OmnAlarm << "Failed to write file: " << i 
				<< mLocations[mActiveLocs[i]] << ":" << mFname
				<< ". Will try to repair!" << enderr;
			rslt = repairFile(i);
			i--;
		}
	}

	int num_writes = mNumActives;
	mLock->unlock();
	aos_assert_r(num_writes > 0, false);
	return true;
}


bool		
AosFileWBack::setStr(const u64 &offset, 
					const char *value, 
					const int len,
					const bool flushflag)
{
	mLock->lock();
	for (int i=0; i<mNumActives; i++)
	{
		bool rslt = mActiveFiles[i]->setStr(offset, value, len, flushflag);
		if (!rslt)
		{
			OmnAlarm << "Failed to write file: " << i 
				<< mLocations[mActiveLocs[i]] << ":" << mFname
				<< ". Will try to repair!" << enderr;
			rslt = repairFile(i);
			i--;
		}
	}

	int num_writes = mNumActives;
	mLock->unlock();
	aos_assert_r(num_writes > 0, false);
	return true;
}


bool		
AosFileWBack::setStr(const u64 &offset, 
					const OmnString &value, 
					const bool flushflag)
{
	mLock->lock();
	for (int i=0; i<mNumActives; i++)
	{
		bool rslt = mActiveFiles[i]->setStr(offset, value, flushflag);
		if (!rslt)
		{
			OmnAlarm << "Failed to write file: " << i 
				<< mLocations[mActiveLocs[i]] << ":" << mFname
				<< ". Will try to repair!" << enderr;
			rslt = repairFile(i);
			i--;
		}
	}

	int num_writes = mNumActives;
	mLock->unlock();
	aos_assert_r(num_writes > 0, false);
	return true;
}


bool
AosFileWBack::locationUsed(const int idx)
{
	for (int i=0; i<mNumActives; i++)
	{
		if (mActiveLocs[i] == idx) return true;
	}
	return false;
}


OmnFilePtr
AosFileWBack::openFile(const int idx AosMemoryCheckDecl)
{
	OmnString fname = mLocations[idx];
	fname << "/" << mFname;
	OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerFileLine);
	aos_assert_r(ff, 0);
	if (ff->isGood()) return ff;
	
	if (mCreate)
	{
		ff = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerFileLine);
		aos_assert_r(ff, 0);
	}

	if (!ff->isGood())
	{
		OmnAlarm << "Failed to open file: " << fname << enderr;
		return 0;
	}
	return ff;
}


int         
AosFileWBack::readToBuff(
		const u64 &offset,
		const u32 &size,
		char *buff)
{
	mLock->lock();
	while (mNumActives > 0)
	{
		int rslt = mActiveFiles[0]->readToBuff(offset, size, buff);
		if (rslt < 0)
		{
			OmnAlarm << "Failed to read file: "
				<< mLocations[mActiveLocs[0]] << ":" << mFname
				<< ". Will try to repair!" << enderr;
			rslt = repairFile(0);
		}
		mLock->unlock();
		return rslt;
	}

	mLock->unlock();
	OmnAlarm << "Failed to read!" << enderr;
	return false;
}


bool
AosFileWBack::readBinaryU32(
		const u64 &offset, 
		u32 &value, 
		const u32 &dft, 
		const u32 &min, 
		const u32 &max)
{
	// It reads an integer from the file. To ensure the correctness, it 
	// will read two files and the results must be the same. Otherwise,
	// the one that is out of the range is considered bad. If both are
	// out of the range, it will try all others. If all are out of 
	// the range, it fails.
	mLock->lock();
	value = dft;
	aos_assert_rl(mNumActives > 0, mLock, false);
	value = mActiveFiles[0]->readBinaryU32(offset, dft);
	if (mNumActives == 1)
	{
		mLock->unlock();
		return true;
	}

	u32 value2 = mActiveFiles[1]->readBinaryU32(offset, dft);
	if (value == value2)
	{
		mLock->unlock();
		return true;
	}

	OmnAlarm << "Contents mismatch: " << value 
		<< ":" << value2 << ":" << offset << enderr;
	u32 values[eMaxCopies];
	values[0] = value;
	values[1] = value2;
	for (int i=2; i<mNumActives; i++)
	{
		values[i] = mActiveFiles[i]->readBinaryU32(offset, dft);
	}

	for (int i=0; i<mNumActives; i++)
	{
		OmnScreen << "Values: " << i << ":" << values[i] << endl;
	}

	// Repair it, if possible:
	// If all but one are in the range and all the ones that are in
	// the range are the same, the one that is not in the range is bad.
	int badidx = -1;
	for (int i=0; i<mNumActives; i++)
	{
		if (values[i] < min || values[i] > max)
		{
			if (badidx >= 0)
			{
				// There are more than one that is not in the range. 
				// We cannot determine which one is bad one. 
				OmnAlarm << "Unable to repair!" << enderr;
				mLock->unlock();
				return false;
			}
			badidx = i;
		}
	}

	aos_assert_rl(badidx < mNumActives, mLock, false);

	if (badidx < 0)
	{
		// All are in the range. 
		OmnAlarm << "Unable to repair!" << enderr;
		mLock->unlock();
		return false;
	}

	// All but 'badidx' is are in the range. Check whether all other values
	// are the same
	value = (badidx == 0)?values[1]:values[0];
	for (int i=0; i<mNumActives; i++)
	{
		if (i != badidx && value != values[i])
		{
			OmnAlarm << "Unable to repair!" << enderr;
			mLock->unlock();
			return false;
		}
	}

	// Identified the bad one
	OmnAlarm << "Identified the bad one: " 
		<< badidx << ":" << mLocations[mActiveLocs[badidx]]
		<< ":" << mFname << enderr;
	repairFile(badidx);
	mLock->unlock();
	return true;
}


bool
AosFileWBack::readBinaryInt(
		const u64 &offset, 
		int &value,
		const int &dft, 
		const int &min, 
		const int &max)
{
	// It reads an integer from the file. To ensure the correctness, it 
	// will read two files and the results must be the same. Otherwise,
	// the one that is out of the range is considered bad. If both are
	// out of the range, it will try all others. If all are out of 
	// the range, it fails.
	mLock->lock();
	aos_assert_rl(mNumActives > 0, mLock, false);
	value = mActiveFiles[0]->readBinaryInt(offset, dft);
	if (mNumActives == 1)
	{
		mLock->unlock();
		return true;
	}

	int value2 = mActiveFiles[1]->readBinaryInt(offset, dft);
	if (value == value2)
	{
		mLock->unlock();
		return true;
	}

	OmnAlarm << "Contents mismatch: " << value << ":" 
		<< value2 << ":" << offset 
		<< ". " << toString() << enderr;
	int values[eMaxCopies];
	values[0] = value;
	values[1] = value2;
	for (int i=2; i<mNumActives; i++)
	{
		values[i] = mActiveFiles[i]->readBinaryInt(offset, dft);
	}

	for (int i=0; i<mNumActives; i++)
	{
		OmnScreen << "Value from file '" << mActiveFiles[i]->getFileName().data()
			<< "': " << values[i] << endl;
	}

	// Repair it, if possible:
	// If all but one are in the range and all the ones that are in
	// the range are the same, the one that is not in the range is bad.
	int badidx = -1;
	for (int i=0; i<mNumActives; i++)
	{
		if (values[i] < min || values[i] > max)
		{
			if (badidx >= 0)
			{
				// There are more than one that is not in the range. 
				// We cannot determine which one is bad one. 
				OmnAlarm << "Unable to repair!" << enderr;
				mLock->unlock();
				return false;
			}
			badidx = i;
		}
	}

	aos_assert_rl(badidx < mNumActives, mLock, false);

	if (badidx < 0)
	{
		// All are in the range. 
		OmnAlarm << "Unable to repair!" << enderr;
		mLock->unlock();
		return false;
	}

	// All but 'badidx' is are in the range. Check whether all other values
	// are the same
	value = (badidx == 0)?values[1]:values[0];
	for (int i=0; i<mNumActives; i++)
	{
		if (i != badidx && value != values[i])
		{
			OmnAlarm << "Unable to repair!" << enderr;
			mLock->unlock();
			return false;
		}
	}

	// Identified the bad one
	OmnAlarm << "Identified the bad one: " 
		<< badidx << ":" << mLocations[mActiveLocs[badidx]]
		<< ":" << mFname << enderr;
	repairFile(badidx);
	mLock->unlock();
	return true;
}


bool
AosFileWBack::resetToEmpty()
{
	mLock->lock();
	for (int i=0; i<mNumActives; i++)
	{
		OmnString fname = mLocations[i];
		fname << "/" << mFname;
		OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(ff && ff->isGood(), false);
		mActiveFiles[i] = ff;
	}	
	mLock->unlock();
	return true;
}


OmnString
AosFileWBack::toString() const
{
	OmnString str;
	for (int i=0; i<mNumActives; i++)
	{
		str << mLocations[i] << "/" << mFname << "; ";
	}
	return str;
}

