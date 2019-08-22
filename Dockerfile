FROM debian:9.9 AS build

RUN apt-get update -y && apt-get install -y libboost-program-options-dev libboost-date-time-dev libwt-dev libcppdb-dev libtool autoconf-archive libvmime-dev libncurses5-dev automake g++ make

RUN mkdir /elpla
COPY elpla /elpla
WORKDIR /elpla

RUN ./autogen.sh && ./configure && make



FROM debian:9.9

RUN apt-get update -y && \
    apt-get install -y \
        libwt-common libcppdb-mysql0 libvmime0\
        locales\
        supervisor\
        nginx-light\
        php-fpm php-mysqli php-mbstring php-xml


RUN localedef -i de_DE -c -f UTF-8 -A /usr/share/locale/locale.alias de_DE.UTF-8
ENV LANG de_DE.utf8


RUN groupadd -r elpla && useradd --no-log-init -r -m -g elpla elpla
WORKDIR /home/elpla

COPY statics statics

RUN mkdir -p .elpla statics/resources/themes/bootstrap presence holidays statistics && chown -R elpla: .elpla *

COPY --chown=elpla configs/elpla/* .elpla/

RUN chmod 400 .elpla/elpla.rc

COPY --from=build /usr/share/Wt/resources statics/resources

COPY --from=build /elpla/src/ed_notify .
COPY --from=build /elpla/src/ed_holidays .
COPY --from=build /elpla/src/ed_presence .
COPY --from=build /elpla/src/ed_statistics .

COPY configs/php-fpm /etc/php/7.0/fpm/
RUN sed -e 's/^pid\s*=.*/;pid=none/'  -i /etc/php/7.0/fpm/php-fpm.conf

COPY elpla/src/availability.php /var/www/availability.php

COPY configs/nginx/nginx.conf /etc/nginx/nginx.conf

EXPOSE 80


COPY configs/supervisord/supervisord.conf /etc/supervisord.conf

CMD ["/usr/bin/supervisord", "-c", "/etc/supervisord.conf"]
