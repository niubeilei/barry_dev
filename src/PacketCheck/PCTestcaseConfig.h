#ifndef _Aos_PCTESTCASECONFIG
#define _Aos_PCTESTCASECONFIG

#include <iostream>
#include <vector>
#include <string>

enum TCData_Type
{
	eTCDATA_HTTP = 1,
	eTCDATA_AM = 2
};

enum TCData_Content_Type
{
	eTCDATA_OTHER = -1, 
	eTCDATA_TEXT = 0, 
	eTCDATA_BINARY = 1
};

bool byteStringToStream(std::string byteString, char*byteStream, unsigned int &len);
bool byteStreamToString(char* byteStream, unsigned int len, std::string &byteString);

class TCData
{
private:
	int mType;
	int mId;
	int mSendLength;
	char* mSendContent;
	int mRecvLength;
	char* mRecvContent;

public:
	TCData();
	TCData(const TCData& tcData);
	TCData& operator=(const TCData& tcData);
	~TCData();

	int getType();
	std::string getTypeString();
	void setType(int type);
	int getID();
	void setID(int id);
	int getSendLength();
	void setSendLength(int send);
	int getRecvLength();
	void setRecvLength(int Recv);
	void setSendBuf(const char *buf);
	void getSendBuf(char* &buf);
	void setRecvBuf(const char *buf);
	void getRecvBuf(char* &buf);
	int getContentType(); // binary or text
};

typedef std::vector<TCData*> TCDataPtrVector;

class AosPCTestcaseConfig
{
public:
		AosPCTestcaseConfig();
		AosPCTestcaseConfig(const char* configFileName);
		~AosPCTestcaseConfig();

		void setConfigFileName(const char* configFileName);
		int readFile();
		int saveFile();

		TCData* getNextTCData();
		void getTCData(int tcID, TCData* &tcData);
		void getTCData(int type, TCDataPtrVector tcDataVector);
		void addTCData(const TCData& tcData);
		void deleteTCData(int tcID);

		int getVectorSize();
public:
		TCDataPtrVector mTCDataPtrVector;
private:
		std::string mConfigFileName;

};

#endif

