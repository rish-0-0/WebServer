#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT    "8080"
#define SERVER_MAX_CONNECTIONS 15
#define MAXMSG  32768
#define MAX_FILE_SIZE 32768


int read_from_client(int filedes);
void* get_in_addr(struct sockaddr* sa);

int
main (void)
{
  
  int sock;
  fd_set active_fd_set, read_fd_set;
  int i;
  struct sockaddr_in clientname;

  /* Create the socket and set it up to accept connections. */
  
  struct addrinfo server_addr_hints, *server_info, *p;
  struct sockaddr_storage client_addr;
  char client_addr_string[INET6_ADDRSTRLEN];
  socklen_t sin_size;
  int return_value;
  int allow_port_reuse = 1;

  memset(&server_addr_hints, 0, sizeof server_addr_hints);
  server_addr_hints.ai_family = AF_UNSPEC;
  server_addr_hints.ai_socktype = SOCK_STREAM;
  server_addr_hints.ai_flags = AI_PASSIVE;

  if ( (return_value = getaddrinfo(NULL, PORT, &server_addr_hints, &server_info)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
    exit(EXIT_FAILURE);
  }


  /* iterate the linked list stored in server_info */

  for ( p = server_info; p != NULL; p = p->ai_next)
  {
    if ( (sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("server: socket");
      continue;
    }

    if ( (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &allow_port_reuse, sizeof(int))) == -1)
    {
      perror("server: setsockopt");
      continue;
    }

    if ( (bind(sock, p->ai_addr, p->ai_addrlen)) == -1)
    {
      perror("server: bind");
      continue;
    }
    break;
  }

  freeaddrinfo(server_info);

  if (p == NULL)
  {
    fprintf(stderr,"Server failed to Initialize\n");
    exit(EXIT_FAILURE);
  }





  if (listen (sock, SERVER_MAX_CONNECTIONS) == -1)
  {
    perror ("listen");
    exit (EXIT_FAILURE);
  }

  printf("Waiting for connections ... \n");

  /* Initialize the set of active sockets. */
  FD_ZERO (&active_fd_set);
  FD_SET (sock, &active_fd_set);

  while (1)
    {
      /* Block until input arrives on one or more active sockets. */
      read_fd_set = active_fd_set;
      if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
          perror ("select");
          exit (EXIT_FAILURE);
        }

      /* Service all the sockets with input pending. */
      for (i = 0; i < FD_SETSIZE; ++i)
        if (FD_ISSET (i, &read_fd_set))
          {
            if (i == sock)
              {
                /* Connection request on original socket. */
                int new;
                sin_size = sizeof client_addr;
                new = accept (sock,
                              (struct sockaddr *) &client_addr,
                              &sin_size);
                if (new < 0)
                  {
                    perror ("accept");
                    exit (EXIT_FAILURE);
                  }
                
                inet_ntop(client_addr.ss_family,
                  get_in_addr((struct sockaddr* )&client_addr),
                  client_addr_string, sizeof client_addr_string);

                printf("Server: got connection from %s\n", client_addr_string);


                FD_SET (new, &active_fd_set);
              }
            else
              {
                /* Data arriving on an already-connected socket. */
                if (read_from_client (i) < 0)
                  {
                    close(i);
                    FD_CLR (i, &active_fd_set);
                  }
                else
                {
                  close(i);
                  FD_CLR (i, &active_fd_set);
                }
              }
          }
    }
}

void* get_in_addr(struct sockaddr* sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}






int
read_from_client (int filedes)
{
  char BUFFER[MAXMSG];
  char FILE_BUFFER[MAX_FILE_SIZE];
  int nbytes;

  char webpage[] = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8 \r\n\r\n";

  char stylesheet[] = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/css; charset=UTF-8 \r\n\r\n"; 

  char scriptType[] = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/javascript; charset=UTF-8 \r\n\r\n";

  char faviconType[] = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: image/x-icon; charset=UTF-8 \r\n\r\n";

  char serverError[] = 
    "HTTP/1.1 500 Internal Server Error\r\n";

  char badRequest[] = 
    "HTTP/1.1 400 Bad Request\r\n";

  nbytes = read (filedes, BUFFER, MAXMSG);
  if (nbytes < 0)
    {
      /* Read error. */
      perror ("read");
      exit (EXIT_FAILURE);
    }
  else if (nbytes == 0)
    /* End-of-file. */
    return -1;
  else
    {
      /* Data read. */
      // fprintf (stdout, "Server: got message: `%s'\n", BUFFER);

      /* ROUTES */

      if (!strncmp(BUFFER, "GET /favicon.ico", 16))
      {
        FILE* favicon_fp;

        if ( (favicon_fp = fopen("htdocs/favicon.ico","r")) == NULL)
        {
          perror("favicon open error\n");
          if ( (write(filedes, serverError, sizeof(serverError) - 1)) == -1)
          {
            perror("writing server error for favi\n");
          }
        }
        else
        {
          if ( (write(filedes, faviconType, sizeof(faviconType) - 1)) == -1)
          {
            perror("writing response header favicon error\n");
          }
          memset(FILE_BUFFER, 0, sizeof FILE_BUFFER);
          fread(FILE_BUFFER, sizeof(char), MAX_FILE_SIZE, favicon_fp);
          if ( (write(filedes, FILE_BUFFER, MAX_FILE_SIZE - 1)) == -1)
          {
            perror("writing file for favicon\n");
          }
          fclose(favicon_fp);
        }
      }
      else if(!strncmp(BUFFER, "GET / ", strlen("GET / ")))
      {
        if ( (write(filedes, webpage, sizeof(webpage) -1)) == -1)
        {
          perror("write\n");
        }
        FILE* index_fp = fopen("htdocs/index.html", "r");
        if (index_fp == NULL)
        {
          perror("fopen for index\n");
          if ( (write(filedes, serverError, sizeof(serverError) - 1)) == -1)
          {
            perror("responding for index.html with 500\n");
          }

        }
        else
        {
          memset(FILE_BUFFER, 0, sizeof FILE_BUFFER);
          fread(FILE_BUFFER, sizeof(char), MAX_FILE_SIZE, index_fp);
          if ( (write(filedes, FILE_BUFFER, strlen(FILE_BUFFER))) == -1)
          {
            perror("writing file for index\n");
          }
        }

        fclose(index_fp);
      }
      else if(!strncmp(BUFFER, "GET /style.css", strlen("GET /style.css")))
      {
        if ( (write(filedes, stylesheet, sizeof(stylesheet) - 1)) == -1)
        {
          perror("writing stylesheet request header error internal server\n");
        }
        FILE* style_fp = fopen("htdocs/style.css", "r");
        if (style_fp == NULL)
        {
          perror("fopen for style\n");
        }
        else
        {
          memset(FILE_BUFFER, 0, sizeof FILE_BUFFER);
          fread(FILE_BUFFER, sizeof(char), MAX_FILE_SIZE, style_fp);
          if ( (write(filedes, FILE_BUFFER, strlen(FILE_BUFFER))) == -1)
          {
            perror("writing file for style.css");
          }
        }
        fclose(style_fp);


      }
      else if (!strncmp(BUFFER, "GET /script.js", strlen("GET /script.js")))
      {
        if ( (write(filedes, scriptType, sizeof(scriptType) - 1)) == -1)
        {
          perror("writing response header for script error\n");
        }

        FILE* script_fp = fopen("htdocs/script.js", "r");
        if (script_fp == NULL)
        {
          perror("fopen for script\n");
          if ( (write(filedes, serverError, sizeof(serverError) - 1)) == -1)
          {
            perror("responding to bad file_pointer_error 500\n");
          }

        }
        else
        {
          memset(FILE_BUFFER, 0, sizeof FILE_BUFFER);
          fread(FILE_BUFFER, sizeof(char), MAX_FILE_SIZE, script_fp);
          // printf("SCRIPT.js\n%s\n", FILE_BUFFER);
          // fflush(stdout);
          if ( (write(filedes, FILE_BUFFER, strlen(FILE_BUFFER))) == -1)
          {
            perror("writing file for script.js\n");
          }
        }
        fclose(script_fp);
      }
      // BAD REQUEST ( 400 Response Code)
      else
      {
        if ( (write(filedes, badRequest, sizeof(badRequest) - 1)) == -1)
        {
          perror("writing bad request response message\n");
        }
      }
      return 0;
    }
}