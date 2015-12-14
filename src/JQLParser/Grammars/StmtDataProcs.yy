stmt_create_data_proc:
	CREATE DATA PROC STRING
	INPUT '(' column_list ')'
	OUTPUT STRING ';'
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($4);
		data_proc->setType("compose");
		data_proc->setInputs($7);
		data_proc->setOutput($10);

		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE INDEX REDUCE TASK DATA PROC W_WORD 
	DATASET W_WORD 
	DATA RECORD W_WORD
	OPERATOR W_WORD 
	IILNAME W_WORD ';' 
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($7);
		data_proc->setType("index");
		data_proc->setDatasetName($9);
		data_proc->setDataRecordName($12);
		data_proc->setOperator($14);
		data_proc->setIILName($16);
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DOC REDUCE TASK W_WORD 
	DATASET W_WORD 
	DATA RECORD W_WORD
	DOC TYPE W_WORD ';'
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($5);
		data_proc->setType("doc");
		data_proc->setDatasetName($7);
		data_proc->setDataRecordName($10);
		data_proc->setDocType($13);
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA PROC W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataProc *stmt = new AosJqlStmtDataProc;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	};


stmt_drop_data_proc:
	DROP DATA PROC W_WORD ';'
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($4);
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_data_proc:
	SHOW DATA PROCS ';' 
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_data_proc:
	DESCRIBE DATA PROC W_WORD ';'  
	{
		AosJqlStmtDataProc *data_proc = new AosJqlStmtDataProc;
		data_proc->setName($4);
		gAosJQLParser.appendStatement(data_proc);
		$$ = data_proc;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

