stmt_run_stat:
	RUN STAT W_WORD W_WORD  stmt_max_threads ';'
	{
		/*
		Kttttt
		AosJqlStmtStat *stat = new AosJqlStmtStat;
		stat->setDataSetName($3);
		stat->setStatDocName($4);
		stat->setMaxThread($5);
		gAosJQLParser.appendStatement(stat);   
		$$ = stat;                             
		$$->setOp(JQLTypes::eRun);              
		*/
	}
	|
	RUN STAT W_WORD W_WORD
	KEY FIELDS '(' column_list ')' stmt_max_threads ';'
	{
		/*
		Kttttt
		AosJqlStmtStat *stat = new AosJqlStmtStat;
		stat->setDataSetName($3);
		stat->setStatDocName($4);
		stat->setKeyFields($8);
		stat->setMaxThread($10);
		gAosJQLParser.appendStatement(stat);   
		$$ = stat;                             
		$$->setOp(JQLTypes::eRun);               
		*/
	}
	;
