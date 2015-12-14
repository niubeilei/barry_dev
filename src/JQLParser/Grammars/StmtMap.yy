stmt_create_map:
	CREATE MAP W_WORD ON TABLE W_WORD
	KEY '(' expr_list ')'
	VALUE '(' expr ')'
	TYPE data_field_data_type
	data_field_max_len 
	opt_where ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName($3);
		map->setTableName($6);
		map->setKeys($9);
		map->setValue($13);
		map->setDataType($16);
		map->setMaxLen($17);
		if ($18)
			map->setWhereCond($18->getWhereExpr().getPtr());
		map->setOp(JQLTypes::eCreate);               
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
	};

stmt_drop_map:
	DROP MAP W_WORD ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eDrop);               
	}
	;

stmt_show_map:
	SHOW MAPS ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eShow);               
	}
	;

stmt_describe_map:
	DESCRIBE MAP W_WORD ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eDescribe);               
	}
	;


stmt_list_map:
	LIST MAP W_WORD ';'
	{
		AosJqlStmtMap *map = new AosJqlStmtMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eList);               
	}
	;

