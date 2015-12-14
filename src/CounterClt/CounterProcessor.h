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
// This is a utility to select docs.
//
// Modification History:
// 03/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_CounterClt_CounterProcessor_h
#define AOS_CounterClt_CounterProcessor_h

using namespace std;

class AosCounterProcessor : public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosCounterProcessor();
	~AosCounterProcessor();


	inline static OmnString composeTimeCounterName(const OmnString &cname)
	{
		OmnString name = AOS_TIMED_COUNTER_PREFIX;
		name << "_" << cname ;
		return name;
	}
	static u64 getTimeIILID(const OmnString &cname, const AosRundataPtr &rdata);


	bool retrieveCounterSet(const AosXmlTagPtr &request,
							const AosRundataPtr &rdata);

	bool queryCounters(const AosXmlTagPtr &request,
							   const AosRundataPtr &rdata);
	bool queryCountersByRanges(const AosXmlTagPtr &request,
								  const AosRundataPtr &rdata);

	bool createEntry(AosBuffPtr &buffPtr,
					 const OmnString &cname,
					 const OmnString &iilName,
					 const int64_t &cvalue,
					 const OmnString &type,
					 const OmnString &statType,
					 const AosCounterTimeInfo &time,
					 u64 &prevCounterId,
					 bool &isFirstiil,
					 const AosRundataPtr &rdata);

private:
	inline OmnString createCounterKey(
				const OmnString &cname, 
				const OmnString &member,
				const AosStatType::E type)
	{
		OmnString key = cname;
		key << "_" << member << "_" << (char)type;
		return key;
	}

	bool constructResponse(
            const OmnString &cname,
            AosXmlTagPtr &record,
			const AosTimeCond &timeCond,
			const AosRundataPtr &rdata);

	bool procStatCounter();
	
	bool constructStatResp(
			const AosCounterRecordPtr &rcd,
			const OmnString &statType,
			const AosRundataPtr &rdata);

private:
	inline OmnString composeCounterKey(const OmnString &cname, const AosStatType::E type)
	{
		OmnString key = cname;
		key << AOS_TIMED_COUNTER_PREFIX << (char)type;
		return key;
	}

	bool retrieveCounterValues(
					const u64 &iilid,
					const OmnString &cname,
					const OmnString &start_value,
					const OmnString &end_value,
					const bool order_by_name,
					OmnString &contents,
					const AosXmlTagPtr &request,
        			const AosRundataPtr &rdata);

	bool retrieveCounterValuesByRanges(
					const u64 &iilid,
					const OmnString &cname,
					const OmnString &ranges, 
					const OmnString &names, 
					const bool order_by_name,
					OmnString &contents,
					const AosXmlTagPtr &request,
        			const AosRundataPtr &rdata);

	bool retrieveCounterValuesByTime(
					const u64 &iilid,
					const OmnString &cname,
					const bool order_by_time,
        			const AosTimeCond &timeCond,
					OmnString &contents,
					const AosXmlTagPtr &request,
        			const AosRundataPtr &rdata);

	bool isValidCounterName(const OmnString &cname, const AosRundataPtr &rdata);
	bool setEmptyContents(const AosRundataPtr &rdata) const;
};
#endif
#endif
