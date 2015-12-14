#ifndef Aos_MultiCondQuery_MCField_h
#define Aos_MultiCondQuery_MCField_h

#include"Util/String.h"
#include"XmlUtil/XmlTag.h"
#include"Util/Opr.h"
#include<vector>

using namespace std;

class AosMCField: public OmnRCObject
{
	OmnDefineRCObject;
	enum ActionType
	{
		eSwitch,
		eRotate,
		eAdd,
		eRemove
	};
	
	struct Action
	{
		ActionType mType;
		int	mPos1;
		int mPos2;
		u64 mDelta;
	};


	private:
		vector<Action>		mActions;
		vector<Action>		mRevActions;
		OmnString			mZeroStr;
		u64					mValue1;
		u64					mValue2;
		AosOpr				mOpr;
	public:
		bool operator = (AosMCField field)
		{
			int i = field.mActions.size();
			mActions.resize(i);
			for(int i=0; i<field.mActions.size(); i++)
			{
				mActions[i].mType = field.mActions[i].mType;
				mActions[i].mPos1 = field.mActions[i].mPos1;
				mActions[i].mPos2 = field.mActions[i].mPos2;
				mActions[i].mDelta = field.mActions[i].mDelta;
			}
		}
	public:
		AosMCField(const AosXmlTagPtr &config);
		AosMCField();
		~AosMCField();
		u64 	getValueByTID(const u64 &tid);
		u64 	getTIDByValue(const u64 &tid);
		void	setQueryConds();
		OmnString getWhereStr(const OmnString);
		bool 	matchCond(const u64&);
	private:
		bool	config(const AosXmlTagPtr &config);
		void bitSwitch(
				u64&,
				const u64&,
				const u64&);
		void bitRotate(
				u64&,
				const u64&);
		void tidAdd(
				u64&,
				const u64&);
		void tidRemove(
				u64&,
				const u64&);
		OmnString 	toString(u64);
		u64			strToU64(OmnString);
		AosOpr 		selectOpr();
		u64			getValue();
		void 		changeValue();
};
#endif
