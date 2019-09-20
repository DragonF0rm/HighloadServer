FROM alpine:latest
COPY ./bin/HighloadServer /bin/httpd
VOLUME /var/www/html
EXPOSE 80
ENTRYPOINT ["/bin/httpd", "/etc/httpd.conf"]




