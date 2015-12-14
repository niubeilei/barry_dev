#ifndef AosDefineFuncCmd
#define AosDefineFuncCmd(classname, cmdName, productName) 	\
classname::classname()									\
:														\
AosFuncCmd(cmdName, productName)							\
{														\
}														\
														\
														\
bool													\
classname::registInst()									\
{														\
	AosFuncCmdPtr inst = OmnNew classname();			\
	if (!inst->readFromFile())							\
	{													\
		OmnAlarm << "Failed to config the object" << enderr;\
		return 0;										\
	}													\
	AosFuncTorturer::addRunTimeFunc(inst);				\
	return true;										\
}														\
														\
														\
AosFuncCmdPtr											\
classname::createInstance(								\
					const std::string& configFile)		\
{														\
	AosFuncCmdPtr inst = OmnNew classname();			\
	if (!inst->readFromFile(configFile))				\
	{													\
		OmnAlarm << "Failed to config the object" << enderr;\
		return 0;										\
	}													\
														\
	return inst;										\
}										
#endif
