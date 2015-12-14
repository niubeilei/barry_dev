create table webbook (
	bookid		varchar(32) not null primary key,
	type		char(1),
	name		varchar(64),
	keywords	varchar(128),
	objid		bigint,
	descrip		text,
	xml			text
);
