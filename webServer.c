#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include "webServerInclude.h"

int main()
{

	/*


		WILL BE MAKING A MULTI-THREADED WEB SERVER

		HAVE TO SET UP A THREAD POOL (around 100 threads will be fine)

		HAVE TO SET UP A LOCK SYSTEM: PREFERABLY MUTEX LOCKS BECAUSE SIMPLICITY

		HAVE TO SUSPEND THREADS WHEN THEY FINISH WORKING (MAKING THREADSAFE FUNCTION)

		EACH THREAD WILL BE DOING FILE I/O FOR A CLIENT.

		ROUTES: GET / HTTP/1.1\r\n -----> index.html
				GET /img.jpg HTTP/1.1\r\n -----> img.jpg
				GET /about HTTP/1.1\r\n -----> about.html
				GET /favicon.ico HTTP/1.1\r\n -----> favicon.ico



	*/

	return 0;
}