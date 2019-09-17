FROM alpine:latest
RUN chmod 777 /var/iwww/html
COPY ./bin/HighloadServer /bin/httpd
COPY ./etc/httpd.conf /etc/httpd.conf

ENTRYPOINT ["/bin/httpd", "/etc/httpd.conf"]




