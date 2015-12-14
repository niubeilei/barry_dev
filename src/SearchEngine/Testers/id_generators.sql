create table id_generators (
	name		varchar(64) not null primary key,
	crtid		varchar(32),
	def			varchar(128),
	description	varchar(256),
	maxlen		int,
	crtbsize	int,
	bsize		int,
	nextblock	varchar(32)
);

