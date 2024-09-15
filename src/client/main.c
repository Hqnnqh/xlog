#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFAULT_PORT 8888
#define BUFFER_SIZE 4096

void cli(int argc, char *argv[], char **host_ip, int *host_port) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <host_ip> <host_port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (argc == 3) {
    *host_port = atoi(argv[2]);
    if (*host_port <= 0 || *host_port > 65535) {
      fprintf(stderr, "Invalid port number. Please provide a valid port "
                      "between 1 and 65535.\n");
      exit(EXIT_FAILURE);
    }
  } else {
    *host_port = DEFAULT_PORT;
  }
}

int main(int argc, char *argv[]) {

  // parse cli arguments to get ip address of host
  char *host_ip;
  int port;
  cli(argc, argv, &host_ip, &port);

  // create tcp connection to host
  int client_socket = 0;
  struct sockaddr_in server_address;

  // create socket
  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Could not create client socket.");
    exit(EXIT_FAILURE);
  }

  server_address.sin_addr.s_addr = AF_INET;
  server_address.sin_port = htons(port);

  // convert IPv4 address from dec to bin.
  if (inet_pton(AF_INET, host_ip, &server_address.sin_addr) <= 0) {
    perror("Invalid address/Address not supported");
    exit(EXIT_FAILURE);
  }

  // connect to server
  if (connect(client_socket, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    perror("Connection to server failed.");
    exit(EXIT_FAILURE);
  }

  printf("Connected to host: %s on port: %d\n", host_ip, port);

  // receive screen dimensions
  int width, height, depth;
  ssize_t bytes_received;

  // receive width
  bytes_received = recv(client_socket, &width, sizeof(width), 0);

  if (bytes_received <= 0) {
    perror("Error receiving width or connection closed");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  // receive height
  bytes_received = recv(client_socket, &height, sizeof(height), 0);

  if (bytes_received <= 0) {
    perror("Error receiving height or connection closed");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  // receive depth
  bytes_received = recv(client_socket, &depth, sizeof(depth), 0);

  if (bytes_received <= 0) {
    perror("Error receiving depth or connection closed");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  printf("Received screen dimensions: %d x %d, depth: %d\n", width, height,
         depth);

  // allocate buffer for image data

  // bytes per pixel
  int bpp = depth / 8;
  int image_size = width * height * bpp;

  char *image_data = (char *)malloc(image_size);

  if (image_data == NULL) {
    perror("Failed to allocate memory for the image");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  // receive image data line by line
  int total_bytes_received = 0;
  while (total_bytes_received < image_size) {
    bytes_received =
        recv(client_socket, image_data + total_bytes_received, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      perror("Error receiving image data or connection closed");
      free(image_data);
      close(client_socket);
      return -1;
    }
    total_bytes_received += bytes_received;
  }

  printf("Received %d bytes of image data\n", total_bytes_received);

  // clean up
  free(image_data);
  close(client_socket);
  return 0;
}
