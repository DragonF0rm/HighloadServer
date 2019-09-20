FROM alpine:latest
COPY ./bin/HighloadServer /bin/httpd
RUN mkdir /var/www
RUN mkdir /var/www/html
ADD /var/www/html /var/www/html
ADD /etc/httpd.conf /etc/httpd.conf
EXPOSE 80
ENTRYPOINT ["/bin/httpd", "/etc/httpd.conf"]




