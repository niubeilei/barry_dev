stmt_debug:
	DEBUG ON ';'
	{
		AosJqlStmtDebug* statement = new AosJqlStmtDebug;
		statement->setSwitch("on");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
	}
	|
	DEBUG OFF ';'
	{
		AosJqlStmtDebug* statement = new AosJqlStmtDebug;
		statement->setSwitch("off");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
	}
	;

