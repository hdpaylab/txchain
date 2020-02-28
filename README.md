# txchain
tx speed up processing

sudo apt install libzmqpp-dev -y

sudo apt install postgresql -y

sudo apt-get install libpq-dev -y


$ sudo -u postgres psql postgres
psql (9.3.9)
Type "help" for help.

postgres=# \password postgres


sudo -u postgres createdb testdb

create table transactions (
        id              bigserial primarykey,
        txid            varchar(1024) NOT NULL,
        recv_index      int,
        file_path       varchar(1024),
        send_flag       int,
        hash            varchar(1024),
        time            timestamp
);
