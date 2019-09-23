FROM ubuntu:latest
WORKDIR /opt/httpd
COPY . .
RUN apt-get update && yes | \
    apt-get install "libevent-dev" "zlib1g-dev" "cmake" # "vim" "git" #TODO remove
RUN ./main.sh
EXPOSE 80
ENTRYPOINT ["./bin/HighloadServer", "/etc/httpd.conf", "2>&1"]
#ENTRYPOINT ["/bin/bash"]

