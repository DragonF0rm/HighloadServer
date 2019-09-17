FROM ubuntu:latest AS build
WORKDIR /HigholadServer
COPY . .
RUN ./main.sh

FROM alpine:latest
WORKDIR /opt/HighloadServer
COPY --from=build /HighloadServer/tmp/bin/HighloadServer ./HoghloadServer
COPY --from=build /HighloadServer/etc/httpd.conf ./httpd.conf
ENTRYPOINT ["/opt/HighloadServer", "/opt/HighloadServer/httpd.conf"]




