/*
 * DataFieldBitmap.h
 *
 *  Created on: Aug 12, 2015
 *      Author: yang
 */

#ifndef DATAFIELDBITMAP_H_
#define DATAFIELDBITMAP_H_




#include "DataField/DataField.h"


class AosDataFieldBitmap : virtual public AosDataField
{
	OmnDefineRCObject;

	AosBuffPtr		mBitmap;

	u64 mBitmapLen;


public:
	AosDataFieldBitmap();
	~AosDataFieldBitmap();


	virtual AosDataType::E getDataType(
						AosRundata *rdata,
						AosDataRecordObj *record) const
	{
		return AosDataType::eBool;
	}

	virtual AosDataFieldObjPtr clone(AosRundata *rdata) const
	{
		return NULL;
	}

	virtual bool	setValueToRecord(
						AosDataRecordObj* record,
						const AosValueRslt &value,
						bool &outofmem,
						AosRundata* rdata);

	virtual bool	getValueFromRecord(
						AosDataRecordObj* record,
						const char * data,
						const int len,
						int &idx,
						AosValueRslt &value,
						const bool copy_flag,
						AosRundata* rdata){return true;};

	virtual AosDataFieldObjPtr create(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata) const
	{
		return NULL;
	}

	virtual bool    clear()
	{
		mBitmapLen = 0;
		return true;
	}

	virtual bool setNullFlagToRecord(
						AosRundata *rdata,
						const int& idx,
						AosDataRecordObj *record,
						int bitmap_offset);

	virtual u64 getBitmapLen()
	{
		return mBitmapLen;
	}

};



#endif /* DATAFIELDBITMAP_H_ */
