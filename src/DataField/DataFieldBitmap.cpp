/*
 * DataFieldBitmap.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: root
 */


#include "DataField/DataFieldBitmap.h"


AosDataFieldBitmap::AosDataFieldBitmap()
:
AosDataField((AosDataFieldType::E)-1, "bitmap", true)
{
	//mBitmap = OmnNew AosBuff;
	mBitmapLen = 0;
}

AosDataFieldBitmap::~AosDataFieldBitmap()
{

}


bool
AosDataFieldBitmap::setValueToRecord(
	AosDataRecordObj* record,
	const AosValueRslt &value,
	bool &outofmem,
	AosRundata* rdata)
{
	return false;
}



bool
AosDataFieldBitmap::setNullFlagToRecord(
	AosRundata *rdata,
	const int& idx,
	AosDataRecordObj *record,
	int bitmap_offset)
{
	aos_assert_r(record, false);

	int rcd_buf_len = record->getMemoryLen();
	int rcd_data_len = record->getRecordLen();


	//int bitmap_offset;
	//record->determineBitmapOffset(bitmap_offset);
	
	int old_bitmap_len = mBitmapLen;

	int new_bitmap_len = std::max(old_bitmap_len,idx / 8 + 1);
	
	if(old_bitmap_len < new_bitmap_len)
	{
		//bitmap should enlarge
		int delta = new_bitmap_len - old_bitmap_len;

		//check record memory length
		if(rcd_buf_len-rcd_data_len < delta)
		{
			//record memory not big enough
			AosBuffPtr buf = OmnNew AosBuff(rcd_data_len + delta AosMemoryCheckerArgs);
			buf->setBuff(record->getData(rdata),rcd_buf_len);//copy the record's old buf
			record->setMemory(buf->data(),buf->buffLen());//set the record new buf
		}
	}

	unsigned char flag = ((unsigned char)(0b00000001 << (7 - (idx % 8))));

	//update record's bitmap buff
	unsigned char *ddd = (unsigned char*)record->getData(rdata);
	unsigned char* bitmap = (unsigned char*)(ddd + bitmap_offset);
	bitmap[idx/8] |= flag;
	mBitmapLen = new_bitmap_len;

	return true;
}
