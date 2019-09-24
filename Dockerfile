FROM ubuntu:latest
WORKDIR /opt/httpd
COPY . .
RUN apt-get update && yes | \
    apt-get install "libevent-dev" "zlib1g-dev" "cmake"
RUN ./main.sh
USER httpd:httpd
EXPOSE 80
ENTRYPOINT ["./bin/HighloadServer", "/etc/httpd.conf", "2>&1"]

