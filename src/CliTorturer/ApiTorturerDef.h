#ifndef AosDefineApiTorturer
#define AosDefineApiTorturer(classname, instptr, name) 	\
classname::classname()									\
:														\
AosApiTorturer(name)									\
{														\
}														\
														\
														\
bool													\
classname::registInst()									\
{														\
	instptr inst = OmnNew classname();					\
	AosApiTorturer::addApiTorturer(inst);				\
	return true;										\
}														\
														\
														\
AosApiTorturerPtr										\
classname::createInstance(								\
					const OmnXmlItemPtr &def,			\
					const AosCliTorturerPtr &cliTorturer)\
{														\
	instptr inst = OmnNew classname();					\
	if (!inst->config(def, cliTorturer))				\
	{													\
		OmnAlarm << "Failed to config the object" << enderr;\
		return 0;										\
	}													\
														\
	return inst;										\
}										
#endif
