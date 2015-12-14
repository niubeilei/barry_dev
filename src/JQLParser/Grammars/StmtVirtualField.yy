stmt_create_virtual_field:
	CREATE VIRTUAL FIELD W_WORD
		stmt_infofield
		TYPE STRING
		stmt_data_type
		data_field_max_len 
		data_field_offset
		data_field_default_value
		ON W_WORD 
		USING MAP W_WORD '(' expr_list ')' ';'
	{
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		virtual_field->setName($4);
		virtual_field->setIsInfoField($5);
		virtual_field->setType($7);
        virtual_field->setDataType($8);
		virtual_field->setMaxLen($9);
		virtual_field->setOffset($10);
		virtual_field->setDefault($11);
		virtual_field->setTable($13);
		virtual_field->setMapName($16);
		virtual_field->setFields($18);
		gAosJQLParser.appendStatement(virtual_field);
		$$ = virtual_field;
		$$->setOp(JQLTypes::eCreate);
	}
    ;

stmt_create_virtual_field_expr:
    CREATE VIRTUAL FIELD W_WORD
        TYPE STRING                                                    
		stmt_data_type
        data_field_max_len
        data_field_offset
        EXPRESSION expr
        ON W_WORD ';'
    {
        AosJqlStmtFieldExpr *stmt = new AosJqlStmtFieldExpr;
        stmt->setName($4);
        stmt->setType($6);
        stmt->setDataType($7);
        stmt->setMaxLen($8);
        stmt->setOffset($9);
        stmt->setExpr($11);
        stmt->setTable($13);
        gAosJQLParser.appendStatement(stmt);
        $$ = stmt;
        $$->setOp(JQLTypes::eCreate);
    };


stmt_drop_virtual_field:
	DROP VIRTUAL FIELD W_WORD ';'
	{
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		virtual_field->setName($4);
		gAosJQLParser.appendStatement(virtual_field);
		$$ = virtual_field;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_virtual_field:
	SHOW VIRTUAL FIELDS ';'
	{
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		gAosJQLParser.appendStatement(virtual_field);
		$$ = virtual_field;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_virtual_field:
	DESCRIBE VIRTUAL FIELD W_WORD ';'
	{
		AosJqlStmtVirtualField *virtual_field = new AosJqlStmtVirtualField;
		virtual_field->setName($4);
		gAosJQLParser.appendStatement(virtual_field);
		$$ = virtual_field;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

stmt_data_type:
	{
		$$ = NULL;
	}
	|
	DATA TYPE data_field_data_type
	{
		$$ = $3;
	}
	;

stmt_infofield:
	{
		$$ = false;
	}
	|
	INFOFIELD
	{
		$$ = true;
	};
