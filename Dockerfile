FROM debian:9.9 AS build

RUN apt-get update -y && apt-get install -y libboost-program-options-dev libboost-date-time-dev libwt-dev libcppdb-dev libtool autoconf-archive libvmime-dev libncurses5-dev automake g++ make

RUN mkdir /elpla
COPY . /elpla

WORKDIR /elpla

RUN ./autogen.sh



FROM debian:9.9

RUN apt-get update -y && apt-get install -y libwt-common libcppdb-mysql0 libvmime0 locales

RUN localedef -i de_DE -c -f UTF-8 -A /usr/share/locale/locale.alias de_DE.UTF-8
ENV LANG de_DE.utf8

RUN groupadd -r elpla && useradd --no-log-init -r -m -g elpla elpla
USER elpla
WORKDIR /home/elpla


RUN mkdir -p .elpla statics/resources/themes/bootstrap presence holidays statistics

COPY --chown=elpla --from=build /elpla/defaults/* .elpla/

RUN chmod 400 .elpla/elpla.rc

COPY --from=build /elpla/css statics/css
COPY --from=build /usr/share/Wt/resources statics/resources

COPY --from=build /elpla/src/ed_notify .
COPY --from=build /elpla/src/ed_holidays .
COPY --from=build /elpla/src/ed_presence .
COPY --from=build /elpla/src/ed_statistics .
