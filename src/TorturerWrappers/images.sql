create table images(
	dataid varchar(32) not null primary key,
	name varchar(64),
	tnail varchar(64),
	container varchar(32),
	tags text,
	vvpdname varchar(64),
	evpdname varchar(64),
	type varchar(64),
	subtype varchar(64),
	creator varchar(64),
	createtime datetime,
	xml text
);

