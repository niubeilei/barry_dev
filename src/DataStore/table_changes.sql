create table table_changes (
	seqno		int auto_increment not null primary key,
	change_type	char(1),
	table_name	varchar(64),
	xml			text
);
