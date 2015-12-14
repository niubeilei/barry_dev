create table remote_bksvr_mgmt(
	clientid INT NOT NULL,
	last_transid BIGINT NOT NULL,
	primary key (clientid)
);

