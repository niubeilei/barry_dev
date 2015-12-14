create table remote_backup_clt(
	seqno INT NOT NULL AUTO_INCREMENT,
	length INT  NOT NULL,
	contents MEDIUMBLOB,
	primary key (seqno)
);

