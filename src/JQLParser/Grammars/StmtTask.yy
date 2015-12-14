stmt_create_task:
	CREATE TASK W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtTask *stmt = new AosJqlStmtTask;
		stmt->setTaskName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE TASK W_WORD 
	INPUT DATASET '('column_list ')' 
	OUTPUT DATASET '('column_list ')' 
	MAPTASK USING SINGLE RECORD DATA ENGING PROCS '('column_list ')'
	REDUCETASK USING ACTION DATA ENGING PROCS '('column_list ')'  ';'
	{
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setTaskName($3);
		task->setInputDataSetNames($7);
		task->setOutputDataSetNames($12);
		task->setDataProcsNames($22);
		task->setActionNames($31);
		gAosJQLParser.appendStatement(task);   
		$$ = task;                             
		$$->setOp(JQLTypes::eCreate);               
	}
    ;

stmt_drop_task:
	DROP TASK W_WORD ';'
	{
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setTaskName($3);
		gAosJQLParser.appendStatement(task);
		$$ = task;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_task:
	SHOW TASKS ';'
	{
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setOp(JQLTypes::eShow);
		gAosJQLParser.appendStatement(task);
		$$ = task;
	}
	;

stmt_describe_task:
	DESCRIBE TASK W_WORD ';'
	{
		AosJqlStmtTask *task = new AosJqlStmtTask;
		task->setTaskName($3);
		gAosJQLParser.appendStatement(task);
		$$ = task;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

