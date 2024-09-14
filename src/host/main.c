#include <X11/Xlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define FREQUENCY_MS 50000
#define PORT 8888

struct Sockets {
  int server_socket;
  int client_socket;
};

struct Sockets create_connection(int port) {
  struct Sockets sockets;
  struct sockaddr_in server, client;

  // create socket
  sockets.server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (sockets.server_socket == -1) {
    perror("Could not create socket.");
    exit(EXIT_FAILURE);
  }

  // Set SO_REUSEADDR to allow immediate reuse of the socket after it is closed
   int opt = 1;
   if (setsockopt(sockets.server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
     perror("Could not set SO_REUSEADDR.");
     exit(EXIT_FAILURE);
   }

  // initialize server
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);

  // bind server socket
  if (bind(sockets.server_socket, (struct sockaddr *)&server, sizeof(server)) <
      0) {
    perror("Could not bind server socket.");
    exit(EXIT_FAILURE);
  }

  // listen for incoming connections; backlog 3 indicates 3 clients can connect
  // simultaniously in queue.
  listen(sockets.server_socket, 3);

  // accept incoming connection
  int connection_size = sizeof(struct sockaddr_in);
  sockets.client_socket =
      accept(sockets.server_socket, (struct sockaddr *)&client,
             (socklen_t *)&connection_size);

  if (sockets.client_socket < 0) {
    perror("Server could not accept client connection.");
    exit(EXIT_FAILURE);
  }

  return sockets;
}

int main(int argc, char *argv[]) {

  // create tcp connection
  struct Sockets sockets = create_connection(PORT);

  // connect to X server
  Display *display = XOpenDisplay(NULL);

  if (display == NULL) {
    perror("Could not get X Display.");
    exit(EXIT_FAILURE);
  }

  Screen *screen = XDefaultScreenOfDisplay(display);
  int height = screen->height;
  int width = screen->width;

  printf("Height: %dpx, Width: %dpx\n", height, width);

  // todo: capture screen, keyboard input

  while(1) {}

  // clean up
  XCloseDisplay(display);
  close(sockets.client_socket);
  close(sockets.server_socket);

  return 0;
}
