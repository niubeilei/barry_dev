create table containers (
	tname		varchar(20) not null primary key,
  	name		text,
	tags		text,
  	description	text,
	creator		varchar(32),
	create_time	datetime 
);

