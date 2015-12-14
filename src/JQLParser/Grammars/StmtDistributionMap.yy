stmt_create_distributionmap:
	CREATE DISTRIBUTIONMAP W_WORD ON W_WORD
	KEY '(' expr_list ')'
	NUMBER INTNUM
	SPLIT INTNUM
	TYPE STRING
	data_field_max_len 
	opt_where ';'
	{
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		map->setMapName($3);
		map->setTableName($5);
		map->setKeys($8);
		map->setMaxNums($11);
		map->setSpliteNum($13);
		map->setDataType($15);
		map->setMaxLen($16);
		if ($17)
			map->setWhereCond($17->getWhereExpr().getPtr());
		map->setOp(JQLTypes::eCreate);               
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
	};

stmt_drop_distributionmap:
	DROP DISTRIBUTIONMAP W_WORD ';'
	{
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eDrop);               
	}
	;

stmt_show_distributionmap:
	SHOW DISTRIBUTIONMAPS ';'
	{
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eShow);               
	}
	;

stmt_describe_distributionmap:
	DESCRIBE DISTRIBUTIONMAP W_WORD ';'
	{
		AosJqlStmtDistributionMap *map = new AosJqlStmtDistributionMap;
		map->setMapName($3);
		gAosJQLParser.appendStatement(map);   
		$$ = map;                             
		$$->setOp(JQLTypes::eDescribe);               
	}
	;


