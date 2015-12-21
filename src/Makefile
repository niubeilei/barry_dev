#----------------------------------------------------------------------------
#	Makefile for SPNR
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Local macros
#----------------------------------------------------------------------------
DIRS  = \
		PreMake \
		Accesses \
		AccessManager \
		Actions \
		AdminTrans \
		AggrFuncs \
		Alarm \
		alarm_c \
		aos \
		AosConf \
		AosConf/Stat \
		aosDLL \
		aosUtil \
		API \
		AppIdGens \
		AppMgr \
		AppVar \
		Base64 \
		BatchQuery \
		BitmapMgr \
		BitmapTrans \
		BSON \
		BlobSE \
		ClusterMgr \
		CompressUtil \
		Conds \
		Config \
		ConfigMgr \
		CounterClt \
		CounterTime \
		CounterServer \
		CounterUtil \
		CubeComm \
		CustomizedProcs/INotify \
		DataAssembler \
		Database \
		DataBlob \
		DataCacher \
		DataCalc \
		DataColComp \
		DataCollector \
		DataCube \
		DataCube/Jimos \
		DataCube/ReadDocUtil \
		DataEngine \
		DataField \
		DataFormat \
		DataGenSeqno \
		DataGenUtil \
		DataProc \
		DataReceiver \
		DataRecord \
		DataRecord/Jimos \
		DataScanner \
		DataScanner/Jimos \
		Dataset \
		Dataset/Jimos \
		DatasetConverters \
		DataSort \
		DataSplitter \
		DataSplitter/Jimos \
		DataStore \
		DataTypes \
		DbQuery \
		Debug \
		Dictionary \
		DfmSnapshot \
		DfmUtil \
		DocClient \
		DocClientWrapper \
		DocFileMgr \
		DocFileMgrNew \
		DocidIdGen \
		DocLock \
		DocMgr \
		DocProc \
		DocSelector \
		DocServer \
		DocTrans \
		DocUtil \
		DfmWrapper \
		EmailClt \
		ErrorMgr \
		EventMgr \
		FmtMgr \
		Funcs \
		Funcs/Jimos \
		GenericObjs \
		GenericVerbs \
		GetLine \
		GICs \
		Groupby \
		GroupQuery \
		HBase/HBaselib \
		HealthCheck \
		HtmlLayoutMgr \
		HtmlModules \
		HtmlServer \
		HtmlUtil \
		IdGen \
		IDO \
		IDTransMap \
		IILClient \
		IILClientWrapper \
		IILEntryMap \
		IILIDIdGen \
		IILMgr \
		IILScanner \
		IILSelector \
		IILTrans \
		IILTransServer \
		IILUtil \
		InstantMsg \
		Jimo \
		JimoAgent \
		JimoAgentClient \
		JimoAgentMonitor \
		JimoAgentPackage \
		JimoAgentUtil \
		JimoAPI \
		JimoCall \
		JimoCallServer \
		JimoDataProc \
		JimoDataset \
		JimoJob \
		JimoLogic \
		JimoProg \
		JimoProgCheckers \
		JimoParser \
		JimoLogicNew \
		JimoRaft \
		Job \
		JobTrans \
		JQLExpr \
		JQLParser \
		JQLParserWrapper \
		JQLStatement \
		JSON\
		LogTrans \
		LogUtil \
		MediaData \
		Message \
		MetaData \
		Microblog \
		MsgClient \
		MsgProc \
		MsgService \
		MsgServer \
		MultiLang \
		MySQL \
		MySqlAdapter \
		NetFile \
		NetworkMgrUtil \
		NetworkMgr \
		Obj \
		parser \
		Porting \
		porting_c \
		Proggie/ProggieUtil \
		Proggie/ReqDistr \
		ProgramAid \
		Query \
		QueryFormat \
		QueryInto \
		Query/Jimos \
		QueryClient \
		QueryCond \
		QueryFuncs \
		QueryRslt \
		QueryUtil \
		QueryVars/Jimos \
		Random \
		RawSE \
		random_c \
		ReliableFile \
		RemoteBackupClt \
		RemoteConvertData \
		ReplicMgr \
		Rundata \
		Schema \
		Schema/Jimos \
		SchemaPicker \
		SchemaPicker/Jimos \
		SearchEngine \
		SearchEngineAdmin \
		SEBase \
		SEClient \
		Security \
		SEInterfaces \
		SeLogClient \
		SeLogSvr \
		SeLogUtil \
		SEModules \
		Sequence \
		SeReqProc \
		SEServer \
		SeSiteMgr \
		SeSysLog \
		SEUtil \
		SEUtilServer \
		ShortMsgClt \
		ShortMsgSvr \
		ShortMsgUtil \
		SingletonClass \
		SiteMgr \
		SmartDoc \
		SmartReply \
		Sorter \
		SQLClient \
		SQLDatabase \
		Statemachine \
		StatEngine \
		StatServer \
		StatTrans \
		StatUtil \
		StatUtil/Jimos \
		StorageEngine \
		StorageMgr \
		StorageMgrUtil \
		StoreQuery \
		StreamFS \
		StreamEngine \
		SvrProxyMgr \
		SvrProxyUtil \
		SyBaseMgr \
		SystemMonitor \
		SyncEngineClient \
		Synchers \
		SysMsg \
		SysTrans \
		TaskMgr \
		TaskScheduler \
		TaskUtil \
		Tester \
		Thread \
		thread_c \
		Timer \
		TimeSelector \
		TinyXml \
		Tools \
		TransBasic \
		TransClient \
		TransMgr \
		Tracer \
		TransServer \
		TransUtil \
		TypeConversion\
		UserMgmt \
		Util \
		Util1 \
		util2 \
		util_c \
		UtilComm \
		UtilData \
		UtilHash \
		UtilTime \
		ValueSel \
		Vector2D\
		Vector2DQryRslt \
		Vector2DUtil \
		VersionServer \
		VirtualFile \
		WordClient \
		WordMgr \
		WordParser \
		XmlInterface \
		XmlLib \
		XmlParser \
		XmlUtil \
		../SyncEngine/SyncEngine \
		HtmlServer/Exe \
		SEServer/FrontEnd \
		SEServer/Cube \
		SEServer/Task \
		SEServer/Admin \
		SEServer/JQLClient \
		SvrProxyExe \


INSDIR = ../install

#----------------------------------------------------------------------------
#	Include macros and targets
#----------------------------------------------------------------------------

include	../makeinclude/wrapper_macros.GNU
include	../makeinclude/macros.GNU
include	../makeinclude/rules.common.GNU
include	../makeinclude/rules.nested.GNU
include	../makeinclude/rules.nolocal.GNU


#----------------------------------------------------------------------------
#  package target
#----------------------------------------------------------------------------
package:
	cd ..; rm -rf install.tar.gz; tar -zcvf install.tar.gz install; cd src

realclean:
	rm -rf ../obj/*; make clean

install:
	cd SvrProxyExe; make install;
	cd ..
	cd etc; make install;
	cd ..

exe:
	cd SEServer/FrontEnd && make clean && make -j && cd ../.. && cd SEServer/Task && make clean && make -j && cd ../.. && cd SEServer/Cube && make clean && make -j && cd ../..

allexe:
	cd SvrProxyExe && make clean&&make -j2 && cd .. && cd SEServer/FrontEnd && make clean && make -j2 && cd ../.. && cd SEServer/Task && make clean && make -j2 && cd ../.. && \
		cd SEServer/Cube && make clean&&make -j2 && cd ../.. && cd SEServer/Admin && make clean&&make -j2 && cd ../.. && cd SEServer/JQLClient && make clean&&make -j2 && cd ../.. && \
		cd HtmlServer/Exe/ && make clean && make -j2 && cd ../..

3.2:
	cd ../JimoCallPackages && make clean && make -j && cd ../IILPackage && make clean && make -j && cd ../DocPackage && make clean && make -j 
