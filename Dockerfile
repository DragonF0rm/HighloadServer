FROM alpine:latest
COPY ./bin/HighloadServer /bin/httpd
RUN mkdir /var/www
RUN chmod 777 /var/www
RUN mkdir /var/www/html
RUN chmod 777 /var/www/html
ENTRYPOINT ["/bin/httpd", "/etc/httpd.conf"]




