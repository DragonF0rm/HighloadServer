FROM alpine:latest
RUN mkdir /var/www
RUN mkdir /var/www/html
COPY ./bin/HighloadServer /bin/httpd

ENTRYPOINT ["/bin/httpd", "/etc/httpd.conf"]




