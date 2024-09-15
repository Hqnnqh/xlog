#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define FREQUENCY_MS 50000
#define PORT 8888

void create_connection(int *server_socket, int *client_socket, int port) {
  struct sockaddr_in server, client;

  // create socket
  *server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (*server_socket == -1) {
    perror("Could not create socket.");
    exit(EXIT_FAILURE);
  }

  // Set SO_REUSEADDR to allow immediate reuse of the socket after it is closed
  int opt = 1;
  if (setsockopt(*server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
      0) {
    perror("Could not set SO_REUSEADDR.");
    exit(EXIT_FAILURE);
  }

  // initialize server
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);

  // bind server socket
  if (bind(*server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("Could not bind server socket.");
    exit(EXIT_FAILURE);
  }

  // listen for incoming connections; backlog 3 indicates 3 clients can connect
  // simultaniously in queue.
  listen(*server_socket, 3);

  // accept incoming connection
  int connection_size = sizeof(struct sockaddr_in);
  *client_socket = accept(*server_socket, (struct sockaddr *)&client,
                          (socklen_t *)&connection_size);

  if (*client_socket < 0) {
    perror("Server could not accept client connection.");
    exit(EXIT_FAILURE);
  }
}

void clean(Display *display, int server, int client) {
  XCloseDisplay(display);
  close(client);
  close(server);
}

int main(int argc, char *argv[]) {

  int server_socket, client_socket;
  // create tcp connection
  create_connection(&server_socket, &client_socket, PORT);

  // connect to X server
  Display *display = XOpenDisplay(NULL);

  if (display == NULL) {
    perror("Could not get X Display.");
    exit(EXIT_FAILURE);
  }

  Screen *screen = XDefaultScreenOfDisplay(display);
  int height = screen->height;
  int width = screen->width;
  int depth = screen->depths->depth;
  // bytes per pixel
  int bpp = depth / 8;

  printf("Height: %dpx, Width: %dpx, Depth: %d\n", height, width, depth);

  Window root = XDefaultRootWindow(display);

  XImage *image;
  ssize_t bytes_sent;
  while (1) {

    // capture screen
    image = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);

    if (image == NULL) {
      perror("Could not capture screen.");
      clean(display, client_socket, server_socket);
      exit(EXIT_FAILURE);
    }

    // send image width
    bytes_sent = send(client_socket, &width, sizeof(width), 0);

    if (bytes_sent < 0) {
      perror("Error sending width to client.");
      clean(display, server_socket, client_socket);
      exit(EXIT_FAILURE);
    }

    // send image height
    bytes_sent = send(client_socket, &height, sizeof(height), 0);

    if (bytes_sent < 0) {
      perror("Error sending height to client.");
      clean(display, server_socket, client_socket);
      exit(EXIT_FAILURE);
    }

    // send image depth
    bytes_sent = send(client_socket, &depth, sizeof(depth), 0);

    if (bytes_sent < 0) {
      perror("Error sending depth to client.");
      clean(display, server_socket, client_socket);
      exit(EXIT_FAILURE);
    }

    // send image data one row at a time
    for (int row = 0; row < height; row++) {
      bytes_sent =
          send(client_socket, image->data + row * image->bytes_per_line,
               width * bpp, 0);

      if (bytes_sent < 0) {
        perror("Error sending image row to client.");
        clean(display, server_socket, client_socket);
        exit(EXIT_FAILURE);
      }
    }

    printf("sent data to client.\n");

    XDestroyImage(image);
    usleep(FREQUENCY_MS);
  }

  clean(display, server_socket, client_socket);

  return 0;
}
