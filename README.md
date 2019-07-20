Software prerequisites
======================

In Debian 9
apt install libboost-program-options-dev libboost-date-time-dev witty-dev libcppdb-dev libtool autoconf-archive libvmime-dev  libncurses5-dev automake

Compilation
===========
./autogen.sh
./configure
make --prefix=installation_dir
make install

Installation
============
mkdir statistics


Start software
==============
./src/ed_statistics --docroot statistics --http-address 0.0.0.0 --http-port 7070  --deploy-path /statistics &

./src/ed_holidays --docroot holidays --http-address 0.0.0.0 --http-port 9090 --deploy-path /holidays &

./src/ed_presence --docroot presence --http-address 0.0.0.0 --http-port 8080 --deploy-path /presence &

