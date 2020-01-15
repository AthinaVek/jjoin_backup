create table if not exists medium_relB(
	join_key bigint(20) unsigned,
    payload bigint(20) unsigned
);
    
create table if not exists medium_relA(
	join_key bigint(20) unsigned,
    payload bigint(20) unsigned);
    
create table if not exists small_relB(
	join_key bigint(20) unsigned,
    payload bigint(20) unsigned);

create table if not exists small_relA(
	join_key bigint(20) unsigned,
    payload bigint(20) unsigned);

create table if not exists tiny_relB(
	join_key bigint(20) unsigned,
    payload bigint(20) unsigned);

create table if not exists tiny_relA (
	join_key bigint(20) unsigned,
    payload bigint(20) unsigned);