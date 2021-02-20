#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5000

int clientSocket, ret;
struct sockaddr_in serverAddr;
char buffer[1024];

void connection()
{

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0)
	{
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("Error in connection.\n");
		exit(1);
	}
	printf("serverConnected.\n");
}
void instructions()
{

	printf("\n\t\tINSTRUCTIONS ON HOW TO USE THE SYSTEM\n");
	printf("\n\tUse the commands below as provided by the system\n");
	printf("\nAddPatient  >>>> Usage Addpatient patient_name  date(y-m-d) gender\n");
	printf("\nAddpatient filename.txt >>>>Usage Addpatient specific the file name(only text based files)\n");
	printf("\nCheck_status  >>>>>usage Check_status\n");
	printf("\nSearch >>>>usage Search enter what to search enter by date(y-m-d) or name\n");
	printf("\n");
	printf("\n*********************************************************************************\n\n");
}
int invalidCommand(char *command)
{
	int a = strncmp(command, "Addpatient", 10);
	int b = strncmp(command, "Check_status", 12);
	int c = strncmp(command, "Search", 6);
	int d = strncmp(command, "Addpatientlist", 14);
	int e = strncmp(command, "clear", 5);

	if (a == 0 || b == 0 || c == 0 || d == 0 || e == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void main()
{
	system("clear");
	printf("\n\t\t **THE  COVID 19 SOCKET AND REPORTING SYSTEM**\n");
	printf("\n");
	int keep = -10;
	char districtName[100];
	printf("\n>>>>EnterDistrictName:");
	//scanf("%s",districtName);
	fgets(districtName, 100, stdin);
	char officerUserName[100];
	printf("\n>>>>Enter the officerUserName:");
	//scanf("%s", officerUserName);
	fgets(officerUserName, 100, stdin);

	while (1)
	{
		char command[50];
		char name[30];
		char date[30];

	entry:
		instructions();
		printf("\n>>>>OfficerUserName:%-10s \n>>>>District\t:%-20s\n", officerUserName, districtName);
		printf("\n----------------------------------------\n");
		bzero(command, sizeof(command));
		bzero(name, sizeof(name));
		bzero(date, sizeof(date));
		printf(">>>>EnterCommand:");
		fgets(buffer, 1024, stdin);
		if (!invalidCommand(buffer))
		{
			printf("\n\n>>>>!!Oops invalid command Please try again\n\n");
			goto entry;
		}
		buffer[strlen(buffer) - 1] = '\0';
		sscanf(buffer, "%s %s %s", command, name, date);
		printf("%s, %s, %s \n", command, name, date);

		//strncmp()
		if ((strlen(date) == 0) && (!strncmp(buffer, "Addpatient", 10)))
		{

			char *txtExt, *b4null;

			txtExt = strtok(buffer, ".");
			while (txtExt != NULL)
			{
				b4null = txtExt;
				txtExt = strtok(NULL, ".");
			}

			if (strcmp(b4null, "txt") == 0)
			{
				FILE *filePtr = fopen(name, "r+");
				if (filePtr == NULL)
				{
					printf(">>>>file read error!\n");
				}
				else
				{

					while (fgets(buffer, 100, filePtr) != NULL)
					{
						buffer[strlen(buffer) - 1] = '\0';

						connection();
						send(clientSocket, buffer, strlen(buffer), 0);
						bzero(buffer, sizeof(buffer));
					}
					fclose(fopen(name, "w"));
				}

				goto entry;
			}
			else
			{
				printf(">>>>Wrong file format\n");
				goto entry;
			}
		}
		else if ((strlen(date) != 0) && (!strncmp(buffer, "Addpatient", 10)))
		{
			//printf("%u\n\n", (int)strlen(buffer));

			//concat
			strcat(buffer, " ");
			strcat(buffer, officerUserName);
			buffer[strlen(buffer) - 1] = '\0';
			connection();
			send(clientSocket, buffer, strlen(buffer), 0);
			//printf("patients added");
		}
		else if (strncmp(buffer, "Search", 6) == 0 && strlen(date) == 0)
		{
			connection();
			printf("\n\tThe search parameter is %s\n", buffer + 7);
			int number = 1;
			int total = 0;
			while (1)
			{

				send(clientSocket, buffer, strlen(buffer), 0);
				printf("\n");
				int returned = read(clientSocket, buffer, 1024);
				//puts(buffer);

				if (strstr(buffer, "TOP"))
				{

					printf("\n\tWe found %d results\n\n", total);
					goto entry;
				}
				else
				{

					printf("\n#%d>>%s\n", number, buffer);
					number++;
					total++;
				}
				bzero(buffer, 1024);
			}
			goto entry;
		}
		else if (strcmp(buffer, "clear") == 0)
		{
			printf("clear");
			system("clear");
			goto entry;
		}
		else
		{

			connection();
			send(clientSocket, buffer, strlen(buffer), 0);
		}

		if (strcmp(buffer, ":exit") == 0)
		{
			close(clientSocket);
			printf("Disconnected from server.\n");
			exit(1);
		}

		if (recv(clientSocket, buffer, 1024, 0) < 0)
		{
			printf("Error in receiving data.\n");
		}
		else
		{
			printf("\n\nserver:%s\n\n", buffer);
		}
		goto entry;
		//close(clientSocket);
	}

	return;
}
