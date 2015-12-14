stmt_create_jimo_logic:
	CREATE JIMOLOGIC W_WORD ON W_WORD 
	ENDPOINT W_WORD
	TIME W_WORD
	CHECKPOINT W_WORD
	USING MATRIX W_WORD
	RESULTS W_WORD ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName($3);
		logic->setTableName($5);
		logic->setEndPoint($7);
		logic->setTime($9);
		logic->setCheckPoint($11);
		logic->setMatrixName($14);
		logic->setResultsName($16);
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE JIMOLOGIC W_WORD ON W_WORD 
	ENDPOINT W_WORD
	TIME W_WORD
	CHECKPOINT W_WORD
	USING MATRIX W_WORD
	USING IILNAME W_WORD
	RESULTS W_WORD ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName($3);
		logic->setTableName($5);
		logic->setEndPoint($7);
		logic->setTime($9);
		logic->setCheckPoint($11);
		logic->setMatrixName($14);
		logic->setSecondName($17);
		logic->setResultsName($19);
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eCreate);
	}
    ;

stmt_drop_jimo_logic:
	DROP JIMOLOGIC W_WORD ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName($3);
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_jimo_logic:
	SHOW JIMOLOGIC ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_jimo_logic:
	DESCRIBE JIMOLOGIC W_WORD ';'
	{
		AosJqlStmtJimoLogic *logic = new AosJqlStmtJimoLogic;
		logic->setName($3);
		gAosJQLParser.appendStatement(logic);
		$$ = logic;
		$$->setOp(JQLTypes::eDescribe);
	}
	;
