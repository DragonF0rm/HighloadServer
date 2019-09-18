FROM ubuntu:latest
COPY ./bin/HighloadServer /bin/httpd

ENTRYPOINT ["/bin/httpd", "/etc/httpd.conf"]




