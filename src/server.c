#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "../include/server.h"
#include "../include/log.h"

static void echo_read_cb(struct bufferevent *bev, void *ctx) {
   /* This callback is invoked when there is data to read on bev */
   struct evbuffer *input = bufferevent_get_input(bev);
   struct evbuffer *output = bufferevent_get_output(bev);

   /* Copy all the data from the input buffer to the output buffer */
   evbuffer_add_buffer(output, input);
}

static void echo_event_cb(struct bufferevent *bev, short events, void *ctx) {
   if (events & BEV_EVENT_ERROR)
      log(ERROR, "Got some error on bufferevent, ERRNO: %d",errno);
   if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
      bufferevent_free(bev);
   }
}

static void accept_conn_cb(struct evconnlistener *listener,
                           evutil_socket_t fd, struct sockaddr *address, int socklen,
                           void *ctx)
{
   /* We got a new connection! Set up a bufferevent for it */
   struct event_base *base = evconnlistener_get_base(listener);
   struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

   bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);

   bufferevent_enable(bev, EV_READ|EV_WRITE);
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx) {
   struct event_base *base = evconnlistener_get_base(listener);
   int err = EVUTIL_SOCKET_ERROR();
   log(ERROR, "Got an error %d (%s) on the listener while accepting", err, evutil_socket_error_to_string(err));
}

int listen_and_serve(u_int16_t port) {
   //TODO use <int event_config_set_num_cpus_hint(struct event_config *cfg, int cpus)> ???
   //TODO log <event_base_get_method()>
   //TODO make events with priority?
   struct event_base *base;
   struct evconnlistener *listener;
   struct sockaddr_in sin;

   base = event_base_new();
   if (!base) {
       log(FATAL, "Unable to open event base");
       return EXIT_FAILURE;
   }
   log(INFO, "libevent backend: %s", event_base_get_method(base));

   memset(&sin, 0, sizeof(sin));
   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = htonl(0); //listen on 0.0.0.0
   sin.sin_port = htons(port);

   listener = evconnlistener_new_bind(
           base,
           accept_conn_cb,
           NULL,
           LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,
           -1,
           (struct sockaddr*)&sin, sizeof(sin));
   if (!listener) {
      log(FATAL, "Couldn't create listener, ERRNO: %d", errno);
      return EXIT_FAILURE;
   }
   evconnlistener_set_error_cb(listener, accept_error_cb);

   event_base_dispatch(base);
   return 0;
}
