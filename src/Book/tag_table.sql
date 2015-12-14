create table tag_table (
	tag         varchar(64),
	dataid      varchar(64),
	type		varchar(10),
	name        text,
	description text,
	creator     varchar(64),
	createtime  datetime,
	primary key (tag, dataid)
);

