#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{

	pid_t proc = fork();

	if ( proc == 0)
	{
		// CHILD PROCESS
		// EXECVP
		char* args[] = {"./server",NULL};
		if (execvp("./server", args) == -1)
		{
			printf("\nFailed to launch server instance\n");
			exit(EXIT_FAILURE);
		}

	}
	else if (proc > 0) {
		printf("Daemonizing ...\n");
		// Exit the parent
		exit(EXIT_SUCCESS);
	}
	else {
		exit(EXIT_FAILURE);
	}

	printf("Ended \n");

	wait(NULL);



	return 0;
}