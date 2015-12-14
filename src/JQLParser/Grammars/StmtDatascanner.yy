stmt_create_datascanner:
	CREATE DATA SCANNER W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDatascanner* stmt = new AosJqlStmtDatascanner;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA SCANNER W_WORD DATA CONNECTOR W_WORD with_order_opts';'
	{
	 	AosJqlStmtDatascanner* statement = new AosJqlStmtDatascanner;
		statement->setName($4);
		//OmnString conn_name = "";
		//conn_name << $4 << "_conn";
		//statement->setConnectorName(conn_name);
		statement->setConnectorName($7);

		if ($8) statement->setOrder();
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
  		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA SCANNER W_WORD 
	DATA CONNECTOR TYPE W_WORD 
	SERVER ID INTNUM
	FILENAME STRING 
	CHAR CODING W_WORD 
	split_size_opts ';'
	{
		AosJqlStmtDatascanner* statement = new AosJqlStmtDatascanner;
		statement->setName($4);
		OmnString conn_name = "";
		conn_name << $4 << "_conn";
		statement->setConnectorName(conn_name);
		statement->setConnectorType($8);
		statement->setSvrId($11);	
		statement->setFileName($13);
		statement->setEncoding($16);
		statement->setSplitSize($17);

		statement->setOrder();
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create Datascanner: " << $4 << endl;

	}
	|
	CREATE DATA SCANNER W_WORD 
	DATA CONNECTOR TYPE DIRECTORY 
	'(' stmt_data_connector_file_list ')'
	CHAR CODING W_WORD 
	split_size_opts ';'
	{
		AosJqlStmtDatascanner* statement = new AosJqlStmtDatascanner;
		statement->setName($4);
		OmnString conn_name = "";
		conn_name << $4 << "_conn";
		statement->setConnectorName(conn_name);
		statement->setConnectorType("DIRECTORY");
		statement->setDirList($10);
		statement->setEncoding($14);
		statement->setSplitSize($15);

		//statement->setOrder();
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;


with_order_opts:
	{
		$$ = false;
	}
	|
	WITH ORDER
	{
		$$ = true;	
	};
stmt_drop_datascanner:
	DROP DATA SCANNER W_WORD ';'
	{
		AosJqlStmtDatascanner *data_datascanner = new AosJqlStmtDatascanner;
		data_datascanner->setName($4);
		gAosJQLParser.appendStatement(data_datascanner);
		$$ = data_datascanner;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_datascanner:
	SHOW DATA SCANNERS ';'
	{
		AosJqlStmtDatascanner *data_datascanner = new AosJqlStmtDatascanner;
		gAosJQLParser.appendStatement(data_datascanner);
		$$ = data_datascanner;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_datascanner:
	DESCRIBE DATA SCANNER W_WORD ';'
	{
		AosJqlStmtDatascanner *data_datascanner = new AosJqlStmtDatascanner;
		data_datascanner->setName($4);
		gAosJQLParser.appendStatement(data_datascanner);
		$$ = data_datascanner;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

