stmt_create_job:
	CREATE JOB W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtJob *stmt = new AosJqlStmtJob;
		stmt->setJobName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE JOB W_WORD 
	USING TASKS '(' column_list ')' ';'
    {
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		job->setTaskNames($7);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eCreate);               
	}
    ;

//stmt_run_job:
//	RUN JOB W_WORD ';'
//	{
//		AosJqlStmtJob *job = new AosJqlStmtJob;
//		job->setJobName($3);
//		gAosJQLParser.appendStatement(job);   
//		$$ = job;                             
//		$$->setOp(JQLTypes::eRun);               
//	}
//	;

stmt_run_job:
	RUN JOB W_WORD FILENAME STRING ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		job->setJobFileName($5);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eRun);               
	}
	;

stmt_stop_job:
	STOP JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eStop);               
	}
	;

stmt_drop_job:
	DROP JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eDrop);               
	}
	;

stmt_show_job:
	SHOW JOBS ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eShow);               
	}
	;

stmt_describe_job:
	DESCRIBE JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eDescribe);               
	}
	;

stmt_show_job_status:
	SHOW JOB STATUS opt_limit';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		/*job->setJobName($3);*/
		job->setLimit($4);
		job->setOp(JQLTypes::eShowStatus);               
		gAosJQLParser.appendStatement(job);   
		$$ = job; 
	};

stmt_show_job_log:
	SHOW JOB LOG W_WORD opt_limit';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setLimit($5);
		job->setIsLog(true);
		job->setJobName($4);
		job->setOp(JQLTypes::eShow);               
		gAosJQLParser.appendStatement(job);   
		$$ = job; 
	};


stmt_wait_job:
	WAIT JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		job->setOp(JQLTypes::eWait);               
		gAosJQLParser.appendStatement(job);   
		$$ = job; 
	};

stmt_restart_job:
	RESTART JOB W_WORD ';'
	{
		AosJqlStmtJob *job = new AosJqlStmtJob;
		job->setJobName($3);
		gAosJQLParser.appendStatement(job);   
		$$ = job;                             
		$$->setOp(JQLTypes::eRestart);               
	}
	;


