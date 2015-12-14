stmt_create_data_connector:
	CREATE W_WORD DATA CONNECTOR W_WORD 
	SERVER ID INTNUM
	FILENAME STRING 
	CHAR CODING W_WORD 
	split_size_opts ';'
	{
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setType($2);
		data_connector->setName($5);
		data_connector->setSvrId($8);
		data_connector->setFileName($10);
		data_connector->setCoding($13);
		data_connector->setSplitSize($14);
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DIRECTORY DATA CONNECTOR W_WORD 
	'(' stmt_data_connector_file_list ')'
	CHAR CODING W_WORD  
	split_size_opts ';'
	{
		//	[FILENAME MATCH PATTERN pattern]
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setType("DIRECTORY");
		data_connector->setName($5);
		data_connector->setDirList($7);
		data_connector->setCoding($11);
		data_connector->setSplitSize($12);
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA CONNECTOR W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataConnector *stmt = new AosJqlStmtDataConnector;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	};

split_size_opts:
	{
		$$ = NULL;
	}
	|
	SPLIT BY SIZE STRING
	{
		$$ = $4;	
	};
	

stmt_data_connector_file_list:
	 DIRECTORY STRING SERVER ID INTNUM 
	 {
	 	typedef AosJqlStmtDataConnector::AosDirList DirList; 
		vector<DirList*> *dls = new vector<DirList*>;
	 	DirList *dl = new DirList();  
		dl->mFileName = $2;
		dl->mSvrId = $5;
		$$ = dls;
		$$->push_back(dl);
	 }
	 |
	 stmt_data_connector_file_list ',' DIRECTORY STRING SERVER ID INTNUM 
	 {
	 	typedef AosJqlStmtDataConnector::AosDirList DirList; 
	 	DirList *dl = new DirList();  
		dl->mFileName = $4;
		dl->mSvrId = $7;
		$$->push_back(dl);
	 }
	 ;


stmt_drop_data_connector:
	DROP DATA CONNECTOR W_WORD ';'
	{
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setName($4);
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_data_connector:
	SHOW DATA CONNECTORS ';'
	{
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_data_connector:
	DESCRIBE DATA CONNECTOR W_WORD ';'
	{
		AosJqlStmtDataConnector *data_connector = new AosJqlStmtDataConnector;
		data_connector->setName($4);
		gAosJQLParser.appendStatement(data_connector);
		$$ = data_connector;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

