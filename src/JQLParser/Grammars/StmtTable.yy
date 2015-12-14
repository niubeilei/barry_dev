stmt_create_table:
	CREATE TABLE stmt_table_if_not_exists table_name '(' create_table_definition_group ')' table_options using_dataschema ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		AosJqlTableOption *opt = $8;
		AosJqlCreateDefineGroup *defineGroup = $6;
		
		statement->setName($4);
		statement->setExists($3);
		if (opt) statement->setOption(opt);

		/*add indexes and columns to the table*/
		if (defineGroup) 
		{
			/*statement->setIndexes(defineGroup->indexes);*/
			statement->setColumns(defineGroup->columns);
		}

		gAosJQLParser.appendStatement(statement);
		delete opt;
		delete defineGroup;
		statement->setSchemaName($9);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create Table:" << " name " << $4 << endl;
	}
	|
	CREATE TABLE stmt_table_if_not_exists table_name table_options using_dataschema ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName($4);
		AosJqlTableOption *opt = $5; 
		if (opt) statement->setOption(opt);
		gAosJQLParser.appendStatement(statement);
		statement->setSchemaName($6);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE TABLE stmt_table_if_not_exists table_name LIKE table_name ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;

		statement->setName($4);
		statement->setLikeTable($6);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create Table Statement like found: " << $3 << " from " << $5 << endl;
	}
	|
	CREATE VIRTUAL TABLE stmt_table_if_not_exists table_name using_dataschema 
	FIELDS '(' stmt_virtual_table_field_list ')' ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName($5);
		statement->setVirtual(true);
		statement->setSchemaName($6);
		statement->mVirtualColumns = $9;
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(statement);
	}
	;

stmt_virtual_table_field_list:
	W_WORD '(' expr_list ')' TYPE STRING 
	{
		$$ = new vector<AosJqlTableVirtulFieldDefPtr>;
		AosJqlTableVirtulFieldDefPtr vv = OmnNew AosJqlTableVirtulFieldDef();
		vv->mName = $1;
		vv->mValues = $3;
		vv->mType= $6;
		$$->push_back(vv);
	}
	|
	stmt_virtual_table_field_list ',' W_WORD '(' expr_list ')' TYPE STRING 
	{
		AosJqlTableVirtulFieldDefPtr vv = OmnNew AosJqlTableVirtulFieldDef();
		vv->mName = $3;
		vv->mValues = $5;
		vv->mType = $8;
		$$->push_back(vv);
	}
	;


using_dataschema:
	{
		$$ = 0;
	}
	|
	USING W_WORD
	{
		$$ = $2;
	};

table_name:
	W_WORD
	{
		$$ = $1;
	}
	|
	W_WORD '.' W_WORD
	{
		AosJqlStmtDatabase* db1 = new AosJqlStmtDatabase;
		db1->setName($1);
		db1->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(db1);
		AosJqlStmtDatabase* db2= new AosJqlStmtDatabase;
		db2->setName($1);
		db2->setOp(JQLTypes::eUse); 
		gAosJQLParser.appendStatement(db2);
		$$ = $3;
	}
	|
	STRING
	{
		$$ = $1;
	}
	|
	STRING '.' STRING
	{
		AosJqlStmtDatabase* db1 = new AosJqlStmtDatabase;
		db1->setName($1);
		db1->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(db1);
		AosJqlStmtDatabase* db2= new AosJqlStmtDatabase;
		db2->setName($1);
		db2->setOp(JQLTypes::eUse); 
		gAosJQLParser.appendStatement(db2);
		$$ = $3;
	}
	;

create_table_definition_group:
	column_definition
	{
		AosJqlCreateDefineGroup* defineGroup = new AosJqlCreateDefineGroup;
		defineGroup->columns = new vector<AosJqlColumnPtr>;
		/*defineGroup->indexes = new vector<AosJqlIndex*>;*/

		$$ = defineGroup;
		//cout << "Init table definition group" << endl;

		$$->columns->push_back($1);
		//cout << "Add a column definition to table definition group" << endl;
	};
	|
	create_table_definition_group ',' column_definition
	{
		$$ = $1;

		$$->columns->push_back($3);
		//cout << "Add a column definition to table definition group" << endl;
	};

column_definition:
	W_WORD data_type
	{
		AosJqlColumn* column = new AosJqlColumn;

		column->name = $1;
		column->type = $2->type;
		column->size = $2->size;
		column->format = $2->format;
		column->defaultVal = NULL;
		column->nullable = true; //default permits null value

		$$ = column;
		delete $2;
		//cout << "Create Table column found:" << $1 << endl;
	};
	|
	column_definition DEFAULT expr
	{
		AosJqlColumn* column = $1;
		AosValueRslt value;

		$3->getValue(0, 0, value);
		OmnString str = value.getStr();

		column->defaultVal = str;

		$$ = column;
		//cout << "Set column default: " << str << endl;
	};	
	|
	column_definition NULLX
	{
		AosJqlColumn* column = $1;
		AosValueRslt value;

		column->nullable = true;

		$$ = column;
		//cout << "Set column default: " << str << endl;
	};	
	|	
	column_definition NOT NULLX
	{
		AosJqlColumn* column = $1;
		AosValueRslt value;

		column->nullable = false;

		$$ = column;
		//cout << "Set column default: " << str << endl;
	};	
	|	
	column_definition COMMENTS STRING
	{
		AosJqlColumn* column = $1;
		AosValueRslt value;

		column->comment = $3;

		$$ = column;
		//cout << "Set column comments: " << $$->comment << endl;
	};	

data_type:
    BIT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    TINYINT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    SMALLINT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    MEDIUMINT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    INT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	NUMBER '(' INTNUM  ',' INTNUM ')'
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eNumber;
		$$->size = AosDataType::getValueSize($$->type); 
		$$->v1 = $3;
		$$->v2 = $5;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	NUMBER '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eNumber;
		$$->size = AosDataType::getValueSize($$->type); 
		$$->v1 = $3;
		$$->v2 = 0;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	BIGINT
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt64;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|

    BIGINT '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eInt64;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    SMALLINT UNSIGNED
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eU32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    MEDIUMINT UNSIGNED
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eU32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    INT UNSIGNED
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eU32;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	BIGINT UNSIGNED
	{
		$$ = new AosJqlColumn;
		$$->type = AosDataType::eU64;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
    FLOAT	
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eDouble;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	DOUBLE
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eDouble;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	DATE
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eDate;
		$$->size = AosDataType::getValueSize($$->type); 
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	CHAR 
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eChar;
		$$->size = 1;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	CHAR '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eChar;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	CHAR '(' INTNUM BYTE')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eChar;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	VARCHAR '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	VARCHAR '(' INTNUM  BYTE ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	VARCHAR2 '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	VARCHAR2 '(' INTNUM  BYTE ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	}
	|
	TEXT
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eCharStr;
		$$->size = 1024;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
	|
	BINARY
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eBuff;
		$$->size = 1024;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};
    |
	DATE '(' STRING ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eDate;
		string tmp = $3;
	    $$->format = $3;
		$$->size = DATE_SIZE;
	}
	|
	BINARY '(' INTNUM ')'
	{
		$$ = new AosJqlColumn;
	    $$->type = AosDataType::eBuff;
		$$->size = $3;
		//cout << "Get datatype " << AosDataType::getTypeStr($$->type) <<  endl;
	};

table_options:
	/*nil*/		 
	{
		$$ = new AosJqlTableOption;
		//cout << "Init a new table option struct" <<  endl;
	};
	|
	table_options COMMENTS STRING 
	{
		$$ = $1;
		$$->comment = $3;
		//delete [] $2;
		//cout << "Table comment" << $$->comment <<  endl;
	};
	|
	table_options CHECKSUM '0'
	{
		$$ = $1;
		$$->checksum = 0;
		//delete [] $2;
		//cout << "Table checksum" << $$->checksum <<  endl;
	};
	|
	table_options CHECKSUM '1'
	{
		$$ = $1;
		$$->checksum = 1;
		//delete [] $2;
		//cout << "Table checksum" << $$->checksum <<  endl;
	};
	|
	table_options AUTO_INCREMENT UNSIGNED_INT 
	{
		$$ = $1;
		$$->autoInc = $3;
		//delete [] $2;
		//cout << "Table auto increment" << $$->autoInc <<  endl;
	}
	|
	table_options DATA MODE FIXED 
	{
		$$ = $1;
		$$->inputDataFormat = "fixed";
	}
	|
	table_options DATA MODE CSV
	{
		$$ = $1;
		$$->inputDataFormat = "csv";
	}
	|
	table_options SOURCE TYPE HBASE DATABASE W_WORD ADDRESS STRING PORT INTNUM TABLE W_WORD RAWKEY '(' expr_list ')'
	{
		$$ = $1;
		$$->data_source = "hbase";
		$$->db_name = $6, 
		$$->db_addr = $8, 
		$$->db_port = $10, 
		$$->db_t_name = $12, 
		$$->raw_keys = $15;
	}
	;

stmt_drop_table:
	DROP TABLE stmt_table_if_exists table_name ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName($4);
		statement->setExists($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop); 
	}
	;

stmt_show_tables:
	SHOW opt_full TABLES opt_from_dbname opt_like ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setFullOption($2);
		statement->setFromOption($4);
		statement->setLikeOption($5);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eShow); 
	}
	;

opt_full:
	{
		$$ = 0;
	}
	|
	FULL
	{
		$$ = 1;
	}
	;

opt_from_dbname:
	{
		$$ = NULL;
	}
	|
	FROM W_WORD
	{
		$$ = $2;
	}
	;

opt_like:
	{
		$$ = NULL;
	}
	|
	LIKE W_WORD
	{
		$$ = $2;
	}
	;


stmt_table_if_exists:
	{
		$$ = false;
	}
	|
	IF EXISTS
	{
		$$ = true;
	}
	;

stmt_table_if_not_exists:
	{
		$$ = false;
	}
	|
	IF NOT EXISTS
	{
		$$ = true;
	}
	;

stmt_describe:
	DESCRIBE TABLE table_name ';'
	{
		AosJqlStmtTable* statement = new AosJqlStmtTable;
		statement->setName($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDescribe); 
	}
	;
