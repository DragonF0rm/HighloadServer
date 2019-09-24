FROM ubuntu:latest
MAINTAINER "Uymin Maksim"
WORKDIR /opt/httpd
COPY . .
RUN apt-get update && yes | \
    apt-get install "libevent-dev" "zlib1g-dev" "cmake"
RUN ./main.sh
RUN useradd httpd
USER httpd
EXPOSE 80
ENTRYPOINT ["id httpd | sed 's/uid=//; s/(.*$//g' |", "./bin/HighloadServer", "/etc/httpd.conf"]

