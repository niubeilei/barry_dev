load_data_stmt:
	LOAD DATA W_WORD '(' stmt_expr_name_values ')' ';' 
	{
		AosJqlStmtLoadData *stmt = new AosJqlStmtLoadData();
		stmt->setLoadDataName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	LOAD DATA W_WORD INPUT DATASET W_WORD INTO W_WORD stmt_max_threads stmt_max_task load_data_fields_opts ';'
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setVersionNum(2);
		load_data->setLoadDataName($3);
		load_data->setFromDataSet($6);
		load_data->setToDataSet($8);
		load_data->setMaxThread($9);
		load_data->setTaskNum($10);
		load_data->setOptionFields($11);
		gAosJQLParser.appendStatement(load_data); 
		$$ = load_data;                           
		$$->setOp(JQLTypes::eCreate);          
    }
	|
	LOAD DATA W_WORD INPUT DATASET W_WORD INTO W_WORD 
	SOURCE TYPE HBASE RAWKEY '(' expr_list ')' stmt_max_threads stmt_max_task  load_data_fields_opts ';'
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setVersionNum(2);
		load_data->setLoadDataName($3);
		load_data->setFromDataSet($6);
		load_data->setToDataSet($8);
		load_data->setIsHbase(true);
		load_data->setRawkeyList($14);
		load_data->setMaxThread($16);
		load_data->setTaskNum($17);
		load_data->setOptionFields($18);
		gAosJQLParser.appendStatement(load_data); 
		$$ = load_data;                           
		$$->setOp(JQLTypes::eCreate);
    }
	|
	DROP DATA W_WORD '(' stmt_expr_name_values ')' ';' 
	{
		AosJqlStmtLoadData *stmt = new AosJqlStmtLoadData();
		stmt->setLoadDataName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eDrop);
	}
	|
	DROP DATA W_WORD INPUT DATASET W_WORD INTO W_WORD stmt_max_threads stmt_max_task load_data_fields_opts ';'
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setLoadDataName($3);
		load_data->setFromDataSet($6);
		load_data->setToDataSet($8);
		load_data->setMaxThread($9);
		load_data->setOptionFields($11);
		load_data->setTaskNum($10);
		gAosJQLParser.appendStatement(load_data); 
		$$ = load_data;                           
		$$->setOp(JQLTypes::eDrop);
    }
	|
	DROP DATA W_WORD INPUT DATASET W_WORD INTO W_WORD 
	SOURCE TYPE HBASE RAWKEY '(' expr_list ')' stmt_max_threads stmt_max_task  load_data_fields_opts ';'
    {
		AosJqlStmtLoadData *load_data = new AosJqlStmtLoadData();
		load_data->setLoadDataName($3);
		load_data->setFromDataSet($6);
		load_data->setToDataSet($8);
		load_data->setIsHbase(true);
		load_data->setRawkeyList($14);
		load_data->setMaxThread($16);
		load_data->setOptionFields($18);
		load_data->setTaskNum($17);
		gAosJQLParser.appendStatement(load_data); 
		$$ = load_data;                           
		$$->setOp(JQLTypes::eDrop);
    }
	;

	
stmt_hbase:
	EXPORT DATA FROM HBASE CONFIG STRING ';'
	{
		AosJqlStmtHBase *hbase = new AosJqlStmtHBase();
		hbase->setConfig($6);
		$$ = hbase;                           
		gAosJQLParser.appendStatement(hbase); 
		$$->setOp(JQLTypes::eRun);          
	}
    ;
	

stmt_max_threads:
	{
		$$ = 1;
	}
	|
	MAX THREAD INTNUM
	{
		$$ = $3;
	}
	;

stmt_max_task:
	{
		$$ = 1;
	}
	|
	USE TASKS INTNUM
	{
		$$ = $3;
	}
	;

load_data_fields_opts:
	{
		$$ = new AosExprList; 
	}
	|
	FIELDS '(' expr_list ')'
	{
		$$ = $3;
	};

