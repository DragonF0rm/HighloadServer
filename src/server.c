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
#include "../include/http.h"

static void respond(struct evbuffer* output, struct http_response_t* resp) {
    if (output == NULL || resp == NULL) {
        log(ERROR, "Invalid function arguments");
        return;
    }
    switch (resp->http_version) {
        case HTTPv1_0: {
            evbuffer_add(output, STR_HTTPv1_0, strlen(STR_HTTPv1_0));
            break;
        }
        case HTTPv1_1: {
            evbuffer_add(output, STR_HTTPv1_1, strlen(STR_HTTPv1_1));
            break;
        }
        default: {
            evbuffer_add(output, STR_HTTPv1_0, strlen(STR_HTTPv1_0));
            break;
        }
    }
    evbuffer_add(output, " ", 1);
    switch (resp->code) {
        case OK: {
            evbuffer_add(output, STR_200_OK, strlen(STR_200_OK));
            break;
        }
        case BAD_REQUEST: {
            evbuffer_add(output, STR_400_BAD_REQUEST, strlen(STR_400_BAD_REQUEST));
            break;
        }
        case FORBIDDEN: {
            evbuffer_add(output, STR_403_FORBIDDEN, strlen(STR_403_FORBIDDEN));
            break;
        }
        case NOT_FOUND: {
            evbuffer_add(output, STR_404_NOT_FOUND, strlen(STR_404_NOT_FOUND));
            break;
        }
        case METHOD_NOT_ALLOWED: {
            evbuffer_add(output, STR_405_METHOD_NOT_ALLOWED, strlen(STR_405_METHOD_NOT_ALLOWED));
            break;
        }
        case INTERNAL_SERVER_ERROR: {
            evbuffer_add(output, STR_500_INTERNAL_SERVER_ERROR, strlen(STR_500_INTERNAL_SERVER_ERROR));
            break;
        }
        default: {
            evbuffer_add(output, STR_500_INTERNAL_SERVER_ERROR, strlen(STR_500_INTERNAL_SERVER_ERROR));
            //TODO change request or trigger some flag?
            break;
        }
    }
    evbuffer_add(output, "\r\n", 2);
    //TODO evbuffer add connection
    const size_t buffer_len = 64;
    char buffer[buffer_len];
    size_t header_len = get_date_header(buffer, buffer_len);
    if (header_len == 0) {
        log(ERROR, "Unable to get Date header");
    } else {
        evbuffer_add(output, buffer, header_len);
    }

    evbuffer_add(output, STR_HEADER_SERVER, strlen(STR_HEADER_SERVER));
    //TODO evbuffer add additional headers
    //TODO evbuffer add response body
}

static void respond_with_400(struct evbuffer* output, enum http_version_t version) {
    if (output == NULL) {
        log(ERROR, "Invalid function arguments");
        return;
    }
    struct http_response_t response = HTTP_RESPONSE_INITIALIZER;
    response.code = BAD_REQUEST;
    response.http_version = version;
    respond(output, &response);
}

static void conn_read_cb(struct bufferevent *bev, void *ctx) {
   /* This callback is invoked when there is data to read on bev */
   struct evbuffer* input = bufferevent_get_input(bev);
   struct evbuffer* output = bufferevent_get_output(bev);

   struct evbuffer_ptr req_headers_end = evbuffer_search(input, "\r\n\r\n", 4, NULL);
   if (req_headers_end.pos < 0) {
       log(WARNING, "Unable to find headers end, input buffer len %d bytes",evbuffer_get_length(input));
       //TODO respond with 400
       return;
   }
   if (evbuffer_ptr_set(input, &req_headers_end, 3, EVBUFFER_PTR_ADD) < 0) {
       log(ERROR, "Unable to move req_headers_end evbuffer_ptr");
       //TODO respond with 500
       return;
   }
   //req_headers_end is now points to the last byte of CRLFCRLF

   char* req_str = malloc((size_t)req_headers_end.pos + 1);
   if (req_str == NULL) {
       log(ERROR, "Unable to allocate memory");
       //TODO respond with 500
       return;
   }
   req_str[req_headers_end.pos] = '\0';
   if (evbuffer_remove(input, req_str, (size_t)req_headers_end.pos) < 0) {
       log(ERROR, "Unable to copy data from input evbuffer");
       //TODO respond with 500
       return;
   }
   log(DEBUG, "req_str before parsing: <%s>", req_str);

   struct http_request_t req = HTTP_REQUEST_INITIALIZER;

   int parse_result = parse_http_request(req_str, &req);
   log(DEBUG, "parse_request_result: <%d>", parse_result);
   switch (parse_result) {
       case 0: {
           log(INFO, "HTTP Request was parsed! METHOD: <%d>; URI: <%s>; VERSION: <%d>", req.method, req.URI, req.http_version);
           break;
       }
       case -1: {
           log(INFO, "HTTP Request was not parsed: req_str contained only empty lines");
           free(req_str);
           return;
       }
       case BAD_REQUEST: {
           log(INFO, "HTTP Request was not parsed: BAD_REQUEST");
           //TODO respond 400
           break;
       }
       case METHOD_NOT_ALLOWED: {
           log(INFO, "HTTP Request was not parsed: METHOD_NOT_ALLOWED");
           //TODO respond 405
           break;
       }
       case INTERNAL_SERVER_ERROR: {
           log(ERROR, "HTTP Request was not parsed: INTERNAL_SERVER_ERROR");
           //TODO respond 500
           break;
       }
       default: {
           log(ERROR, "Unexpected http request parsing return code: %d", parse_result);
           free(req_str);
           return;
       }
   }

   //TODO busyness logic

   free(req_str);
}

static void conn_event_cb(struct bufferevent *bev, short events, void *ctx) {
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

   bufferevent_setcb(bev, conn_read_cb, NULL, conn_event_cb, NULL);

   bufferevent_enable(bev, EV_READ|EV_WRITE);
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx) {
   int err = EVUTIL_SOCKET_ERROR();
   log(ERROR, "Got an error %d (%s) on the listener while accepting", err, evutil_socket_error_to_string(err));
}

int listen_and_serve(u_int16_t port) {
   //TODO use <int event_config_set_num_cpus_hint(struct event_config *cfg, int cpus)> ???
   //TODO make events with priority?
   //TODO set read low watermark?
   //TODO create custom listener?
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
