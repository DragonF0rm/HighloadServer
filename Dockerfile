FROM ubuntu:latest
WORKDIR /opt/httpd
COPY . .
RUN apt-get update && yes | \
    apt-get install "libevent-dev" "zlib1g-dev" "cmake"
RUN ./main.sh
EXPOSE 80
ENTRYPOINT ["/opt/httpd/bin/httpd", "/etc/httpd.conf"]




