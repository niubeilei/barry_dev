create table datastore_test (
	id		varchar(64) not null primary key,
	type		char(1),
	name		varchar(64),
	keywords	varchar(128),
	descrip		text,
	xml			text,
	status		integer
);
