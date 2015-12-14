stmt_create_sequence:
	CREATE SEQUENCE W_WORD sequence_info ';'
	{
		AosJqlStmtSequence* stmt = new AosJqlStmtSequence;
		stmt->setName($3);
		stmt->setSequenceInfo($4);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	};

sequence_info:
	create_sequence_increment_by
	create_sequence_start_with
	create_sequence_max_value
	create_sequence_min_value
	create_sequence_cycle
	create_sequence_cache
	create_sequence_order
	{
		$$ = new AosJQLSequenceInfo;
		$$->setIncrementBy($1);
		$$->setStartWith($2);
		$$->setMaxValue($3);
		$$->setMinValue($4);
		$$->setIsCycle($5);
		$$->setCacheNum($6);
		$$->setIsOrder($7);
	};

create_sequence_increment_by:
	{
		$$ = 1;
	}
	|
	INCREMENT BY INTNUM
	{
		$$ = $3; 
	};

create_sequence_start_with:
	{
		$$ = 1;
	}
	|
	START WITH INTNUM
	{
		$$ = $3;	
	};

create_sequence_max_value:
	{
		$$ = ~(1 << 31);
	}
	|
	NOMAXVALUE
	{
		$$ = ~(1 << 31);
	}
	|
	MAXVALUE INTNUM
	{
		$$ = $2;
	};

create_sequence_min_value:
	{
		$$ = 1 << 31;
	}
	|
	NOMINVALUE
	{
		$$ = 1 << 31;
	}
	|
	MINVALUE INTNUM
	{
		$$ = $2;
	};

create_sequence_cycle:
	{
		$$ = false;
	}
	|
	CYCLE
	{
		$$ = true;
	}
	|
	NOCYCLE
	{
		$$ = false;
	};

create_sequence_cache:
	{
		$$ = 20;
	}
	|
	CACHE INTNUM
	{
		$$ = $2;
	}
	|
	NOCACHE
	{
		$$ = 20;
	};

create_sequence_order:
	{
		$$ = false;
	}
	|
	ORDER
	{
		$$ = true;
	}
	|
	NOORDER
	{
		$$ = false;
	};
