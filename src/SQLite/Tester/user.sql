create table user (
  user_id  int not null primary key,
  user_name 	varchar2(30),
  first_name 	varchar2(30),
  last_name 	varchar2(30),
  email      	varchar2(256),
  office_phone 	varchar2(20),
  home_phone   	varchar2(20),
  desc  		varchar(256),
  status       	char(1)
);

