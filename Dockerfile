FROM alpine:latest
COPY ./tmp/bin/HighloadServer /bin/httpd
COPY ./etc/httpd.conf /etc/httpd.conf

ENTRYPOINT ["/bin/httpd", "/etc/httpd.conf"]




