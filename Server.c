#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5000
int extract_command(char *);
void main()
{

	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[256];
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("Error in binding.\n");
		exit(1);
	}
	printf("Bind to port %d\n", 4444);

	if (listen(sockfd, 10) == 0)
	{
		printf("ServerListening....\n");
	}
	else
	{
		printf("Error in binding.\n");
	}

	while (1)
	{
		newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
		if (newSocket < 0)
		{
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if ((childpid = fork()) == 0)
		{
			close(sockfd);

			while (1)
			{
				bzero(buffer, sizeof(buffer));
				recv(newSocket, buffer, 1024, 0);
				int command = extract_command(buffer);

				switch (command)
				{
				case 1:
				{
					printf("\n\t------Addpatient Command-------\n\n");
					if (strcmp(buffer, ":exit") == 0)
					{
						printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					}
					else
					{

						FILE *filePtr = fopen("enroll.txt", "a+");
						//printf("[+]Writing to file");
						fprintf(filePtr, "%s\n", buffer);
						//printf("[+]wrote to file");
						printf("RecievedPatient: %s\n", buffer);
						send(newSocket, "ReceivedSuccessfully", strlen(buffer), 0);
						//fputs(buffer,filePtr);
						bzero(buffer, sizeof(buffer));
					}
					printf("\n-------------------------------------------------------------\n");
					return;
				}

				case 2:

				{
					printf("\n\t--------Check_status Command-------\n");
					FILE *filePtr = fopen("enroll.txt", "r");
					int totalRecords = 0;
					while (fgets(buffer, 256, filePtr) != NULL)
					{
						totalRecords++;
					}
					bzero(buffer, sizeof(buffer));
					if (totalRecords > 1)
					{
						printf("\nTotal cases are %d in the file", totalRecords);
						sprintf(buffer, "Total cases stored in the file  are %d\n", totalRecords);
						send(newSocket, buffer, strlen(buffer), 0);
					}
					else if (totalRecords == 0)
					{
						printf("\nThere are no cases stored yet!!ADD");
						sprintf(buffer, "No cases !! stored Yet");
						send(newSocket, buffer, strlen(buffer), 0);
					}
					else
					{
						printf("\nThere is  %d case stored in a file", totalRecords);
						sprintf(buffer, "There is %d case stored in the file", totalRecords);
						send(newSocket, buffer, strlen(buffer), 0);
					}

					printf("\n-----------------------------------------------------\n");
					return;
				}

				case 3:
				{
					printf("\n\t---------Search Command------------------------\n");
					FILE *filePtr = fopen("enroll.txt", "r");
					// char search_name[50];
					char store[200];
					int records = 0;
					int total_records = 0;
					int number = 1;

					printf("\n");
					printf("\nThe search parameter is %s\n", buffer);
					printf("\n\n");

					printf("\n\n\t----FoundResults---------\n\n");
					while (fgets(store, 100, filePtr) != NULL)
					{
						total_records++;

						if (strstr(store, buffer) != NULL)
						{

							puts(store);
							send(newSocket, store, 1024, 0);

							records++;
						}
					}
					//bzero(buffer, sizeof(buffer));
					if (records == 1 && total_records == 1)
					{
						printf("\nWe found %d result out of %d\n", records, total_records);
						sprintf(buffer, "\n%d Result found  out of %d records\n", records, total_records);
					}
					else if (records == 0 && total_records > 0)
					{
						printf("\nWe found %d cases  out of %d\n", records, total_records);
						sprintf(buffer, "\n%d cases foundout of %d records\n", records, total_records);
						// send(newSocket, buffer, 100, 0);
						// send(newSocket , "STOP" ,10,0);
					}
					else if (total_records == 0)
					{
						printf("\nThe file is empty\n");
						sprintf(buffer, "No search results found!Empty file");
						// send(newSocket, buffer, 200, 0);
						// send(newSocket , "STOP" ,10,0);
					}
					else
					{
						printf("\nWe found %d result out of %d\n", records, total_records);
						sprintf(buffer, "\n%d Results found  out of %d records\n", records, total_records);
						// send(newSocket, buffer, 200, 0);
						// send(newSocket , "STOP" ,10,0);
					}
					//send(newSocket , "STOP",5,0);
					send(newSocket, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));
					strcpy(buffer, "STOP");
					send(newSocket, buffer, strlen(buffer), 0);

					//strcpy()

					printf("\n-------------------------------------------------------------\n");
					return;
				}

				// case 4:
				// {
				// 	break;
				// }
				case 5:
				{
					if (strcmp(buffer, ":exit") == 0)
					{
						printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					}
					else
					{

						printf("\n\t---------Addpatient filename.txt Command-------------------\n");
						FILE *filePtr = fopen("enroll.txt", "a+");
						//printf("\n\tWriting to file\n\n");
						fprintf(filePtr, "%s\n", buffer);
						//printf("\n\twrote to file\n\n");
						printf("\nClient: %s\n", buffer);
						send(newSocket, buffer, strlen(buffer), 0);
						//fputs(buffer,filePtr);
						bzero(buffer, sizeof(buffer));
						printf("\n---------------------------------------------------------\n");
					}
					return;
				}
				}
			}
		}
	}

	close(newSocket);

	//return 0;
}

int extract_command(char *buffer)
{
	if (strncmp(buffer, "Addpatient", 10) == 0)
	{
		strcpy(buffer, buffer + 11);
		// printf("\nBuffer now is %s\n", buffer);
		return 1;
	}
	else if (strncmp(buffer, "Check_status", 12) == 0)
	{
		printf("\n check status\n");
		return 2;
	}
	else if (strncmp(buffer, "Search", 6) == 0)
	{
		//printf("Search");
		strcpy(buffer, buffer + 7);
		return 3;
	}
	else if (strncmp(buffer, "Addpatientlist", 14) == 0)
	{
		printf("[+]Addpatientlist");
		return 4;
	}
	else
	{
		//printf("Adding from file no command ");
		return 5;
	}
}