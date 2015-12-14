create table user(
  user_id  int not null primary key,
  user_name 	varchar(30),
  first_name 	varchar(30),
  last_name 	varchar(30),
  email      	varchar(256),
  office_phone 	varchar(20),
  home_phone   	varchar(20),
  status       	char(1)
);

