create table sflog (
	log_id          varchar(64) not null primary key,
    logid_type      char(1),
    app_name        varchar(128), 
    app_instance    varchar(128),
    level           int,
    status          boolean,
    reset           int, 
    security_level  int,
    reliability     int, 
    archive         int,
    file_name       varchar(64),
    max_size        bigint, 
    file_size       bigint,
    truncate_method int,
    total_entries   bigint 
);

