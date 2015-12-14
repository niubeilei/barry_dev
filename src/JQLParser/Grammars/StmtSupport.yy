stmt_complete_iil:
	COMPLETE IIL W_WORD ';'
	{
		AosJqlStmtCompleteIIL *statement = new AosJqlStmtCompleteIIL;
		OmnString iilname = $3;
		statement->setIILName(iilname);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
	};
