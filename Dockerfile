FROM ubuntu:latest AS build
WORKDIR /HigholadServer
COPY . .
RUN apt-get install cmake
RUN apt-get install "libevent-2.1-6"
RUN apt-get install libevent-dev
RUN apt-get install "libevent-pthreads-2.1-6"
RUN ./main.sh

FROM alpine:latest
WORKDIR /opt/HighloadServer
COPY --from=build /HighloadServer/tmp/bin/HighloadServer ./HoghloadServer
COPY --from=build /HighloadServer/etc/httpd.conf ./httpd.conf
ENTRYPOINT ["/opt/HighloadServer", "/opt/HighloadServer/httpd.conf"]




