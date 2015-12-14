stmt_create_doc:
	CREATE DOC STRING ';'
	{
		AosJqlStmtDoc *doc = new AosJqlStmtDoc;
		doc->setOp(JQLTypes::eCreate);               
		doc->setDocConf($3);
		gAosJQLParser.appendStatement(doc);   
		$$ = doc;                             
	};

stmt_show_doc:
	SHOW DOC INTNUM ';'
	{
		AosJqlStmtDoc *doc = new AosJqlStmtDoc;
		doc->setOp(JQLTypes::eShow);               
		doc->setDocDocid($3);
		gAosJQLParser.appendStatement(doc);   
		$$ = doc;                             
	}
	|
	SHOW DOC STRING ';'
	{
		AosJqlStmtDoc *doc = new AosJqlStmtDoc;
		doc->setOp(JQLTypes::eShow);               
		doc->setDocObjid($3);
		gAosJQLParser.appendStatement(doc);   
		$$ = doc;                             
	}
	;
