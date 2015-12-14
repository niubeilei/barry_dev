create table charset (
  name			varchar(64) not null primary key,
  keywords		varchar(256),
  description	text,
  xml  			text,
  status       	char(1)
);

