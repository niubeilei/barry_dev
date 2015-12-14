stmt_select_into:
	jql_select_stmt INTO_OUTFILE STRING set_job_name record_format stmt_max_threads ';'
	{
		AosJqlStmtSelectInto * selectinto = new AosJqlStmtSelectInto();
		selectinto->setSelect($1);
		selectinto->setFilePath($3);
		selectinto->setJobName($4);
		selectinto->setFormat($5);
		selectinto->setMaxThreads($6);
		gAosJQLParser.appendStatement(selectinto);             
		$$ = selectinto;   
	}
	;


record_format:
	{	
		$$ = 0;
	}
	|
 	FIELDS TERMINATED BY STRING ENCLOSED BY STRING  ESCAPED BY STRING LINES STARTING BY STRING TERMINATED BY STRING	
	{
		vector<OmnString> *v = new vector<OmnString>; 
		v->push_back($4);
		v->push_back($7);
		v->push_back($10);
		v->push_back($14);
		v->push_back($17);
		$$ = v;
	}
	;

set_job_name:
	{
		$$ = 0;
	}
	|
	JOB W_WORD 
	{
		$$ = $2;
	}
	;
