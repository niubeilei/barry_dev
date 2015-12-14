create table remote_backup_svr (
	transid BIGINT NOT NULL,
	type SMALLINT NOT NULL,
	length INT	NOT NULL,
	xml MEDIUMBLOB,
	primary key (transid)
);

