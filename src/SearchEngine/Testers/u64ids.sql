create table u64ids (
	name		varchar(64) not null primary key,
	crtid		bigint,
	description	varchar(256),
	crtbsize	int,
	nextblock	bigint
);

