stmt_run_script_file:
	SOURCE STRING parameters suppress_flag ';'
    {
		AosJqlStmtRunScriptFile *stmt = new AosJqlStmtRunScriptFile;
		stmt->setFileName($2);
		stmt->setParameters($3);
		stmt->setSuppress($4);
		gAosJQLParser.appendStatement(stmt);   
		$$ = stmt;                             
	}
    ;

parameters:
	{
		$$ = NULL;
	}
	|
	expr_list
	{
		$$ = $1;
	}
	;

suppress_flag:
	{
		/* nil  */
		$$ = false;
	}
	|
	SUPPRESS ON
	{
		$$ = true;
	}
	|
	SUPPRESS OFF 
	{
		$$ = false;
	}
	;
