Software prerequisites
======================

In Debian 9:

$ apt install libboost-program-options-dev libboost-date-time-dev witty-dev libcppdb-dev libtool autoconf-archive libvmime-dev  libncurses5-dev automake

Compilation
===========
```shell
$ ./autogen.sh
$ ./configure
$ make --prefix=installation_dir
$ make install
```

Installation
============

A mysql (or mariadb) database must be configured beforehand.

Create configuration file at $HOME/.elpla/elpla.rc with the following content:
```ini
[database.mysql]
host=my_database_server
user=my_database_user
password=xxxxxxxxxx
database=my_database_name

[notify.mail]
availability_body=notify_availability.txt
assignment_body=notify_assignment.txt
```

The [database.mysql] entries contain the configuration to connect to the database. The [notify.mail] entries point to files that contain the default text used when sending emails to the kindergarten members.

```shell
$ mkdir statistics`
```

Start software
==============
```shell
$ ./src/ed_statistics --docroot statistics --http-address 0.0.0.0 --http-port 7070  --deploy-path /statistics &
$ ./src/ed_holidays --docroot holidays --http-address 0.0.0.0 --http-port 9090 --deploy-path /holidays &
$ ./src/ed_presence --docroot presence --http-address 0.0.0.0 --http-port 8080 --deploy-path /presence &
```
