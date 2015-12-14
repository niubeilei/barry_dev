stmt_create_schedule:
	CREATE SCHEDULE W_WORD 
	ON JOB W_WORD ';'
    {
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		schedule->setJobName($6);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eCreate);               
	}
	|
	CREATE SCHEDULE W_WORD 
	ON JOB W_WORD 
	SCHEDULE STRING RUN AT STRING STRING 
	STOP AT RUN STRING TIMES
	schedule_on_priority ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		schedule->setJobName($6);
		schedule->setDateType($8);
		schedule->setDate($11);
		schedule->setRunTime($12);
		schedule->setStopTime($16);
		schedule->setPriority($18);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eCreate);               
	}
	|
	CREATE SCHEDULE W_WORD 
	ON JOB W_WORD 
	SCHEDULE STRING RUN AT STRING 
	STOP AT TIME STRING
	schedule_on_priority ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		schedule->setJobName($6);
		schedule->setDateType($8);
		schedule->setDate($12);
		schedule->setStopTime($15);
		schedule->setPriority($16);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eCreate);               

	}
	|
	CREATE SCHEDULE W_WORD 
	ON JOB W_WORD 
	SCHEDULE STRING RUN AT STRING 
	schedule_on_priority ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		schedule->setJobName($6);
		schedule->setDateType($8);
		schedule->setDate($11);
		schedule->setPriority($12);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eCreate);               
	}
    ;

schedule_on_priority:
	{
		$$ = 0;
	}
	|
	ON PRIORITY INTNUM 
	{
		$$ = $3;
	}
	;

stmt_run_schedule:
	RUN SCHEDULE W_WORD ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eRun);               
	}
	;

stmt_stop_schedule:
	STOP SCHEDULE W_WORD ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eStop);               
	}
	;

stmt_drop_schedule:
	DROP SCHEDULE W_WORD ';'
	{
		AosJqlStmtSchedule *schedule = new AosJqlStmtSchedule;
		schedule->setScheduleName($3);
		gAosJQLParser.appendStatement(schedule);   
		$$ = schedule;                             
		$$->setOp(JQLTypes::eDrop);               
	}
	;

