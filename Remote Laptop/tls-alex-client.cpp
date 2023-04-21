
// Routines to create a TLS client
#include "make_tls_client.h"

// Network packet types
#include "netconstants.h"

// Packet types, error codes, etc.
#include "constants.h"

// Tells us that the network is running.
static volatile int networkActive=0;

void handleError(const char *buffer)
{
	switch(buffer[1])
	{
		case RESP_OK:
			printf("Command / Status OK\n");
			break;

		case RESP_BAD_PACKET:
			printf("BAD MAGIC NUMBER FROM ARDUINO\n");
			break;

		case RESP_BAD_CHECKSUM:
			printf("BAD CHECKSUM FROM ARDUINO\n");
			break;

		case RESP_BAD_COMMAND:
			printf("PI SENT BAD COMMAND TO ARDUINO\n");
			break;

		case RESP_BAD_RESPONSE:
			printf("PI GOT BAD RESPONSE FROM ARDUINO\n");
			break;

		default:
			printf("PI IS CONFUSED!\n");
	}
}

void handleStatus(const char *buffer)
{
	int32_t data[16];
	memcpy(data, &buffer[1], sizeof(data));

	printf("\n ------- ALEX STATUS REPORT ------- \n\n");
	printf("Left Forward Ticks:\t\t%d\n", data[0]);
	printf("Right Forward Ticks:\t\t%d\n", data[1]);
	printf("Left Reverse Ticks:\t\t%d\n", data[2]);
	printf("Right Reverse Ticks:\t\t%d\n", data[3]);
	printf("Left Forward Ticks Turns:\t%d\n", data[4]);
	printf("Right Forward Ticks Turns:\t%d\n", data[5]);
	printf("Left Reverse Ticks Turns:\t%d\n", data[6]);
	printf("Right Reverse Ticks Turns:\t%d\n", data[7]);
	printf("Forward Distance:\t\t%d\n", data[8]);
	printf("Reverse Distance:\t\t%d\n", data[9]);
	printf("\n---------------------------------------\n\n");
}

void handleMessage(const char *buffer)
{
	printf("MESSAGE FROM ALEX: %s\n", &buffer[1]);
}

void handleCommand(const char *buffer)
{
	// We don't do anything because we issue commands
	// but we don't get them. Put this here
	// for future expansion
}

void handleColour(const char *buffer)
{
	int32_t data[16];
	memcpy(data, &buffer[1], sizeof(data));
	//printf("COLOUR DECTECTED: %d\n", data[0]);
	if (data[0] == 1)
	{
		printf("COLOUR DETECTED: RED\n");
	}
	else if (data[0] == 2)
	{
		printf("COLOUR DETECTED: GREEN\n");
	}
	else
	{
		printf("COLOUR DETECTED: DUMMY\n");
	}
	printf("R: %d\n", data[1]);
	printf("G: %d\n", data[2]);
	printf("B: %d\n", data[3]);
}

void handleIMU(const char *buffer)
{
	int32_t data[16];
	memcpy(data, &buffer[1], sizeof(data));
	printf("IMU VALUE: %d\n", data[0]);
	printf("1=negative heading, 2=positive heading   curr heading: %d\n", data[1]);
}


void handleNetwork(const char *buffer, int len)
{
	// The first byte is the packet type
	int type = buffer[0];

	switch(type)
	{
		case NET_ERROR_PACKET:
			handleError(buffer);
			break;

		case NET_STATUS_PACKET:
			handleStatus(buffer);
			break;

		case NET_MESSAGE_PACKET:
			handleMessage(buffer);
			break;

		case NET_COMMAND_PACKET:
			handleCommand(buffer);
			break;
		case NET_COLOUR_PACKET:
			handleColour(buffer);
			break;
		case NET_IMU_PACKET:
			handleIMU(buffer);
	}
}

void sendData(void *conn, const char *buffer, int len)
{
	int c;
	printf("\nSENDING %d BYTES DATA\n\n", len);
	if(networkActive)
	{
		/* TODO: Insert SSL write here to write buffer to network */
		c =SSL_write((SSL*)conn, buffer, len);
		/* END TODO */	
		networkActive = (c > 0);
	}
}

void *readerThread(void *conn)
{
	char buffer[128];
	int len;

	while(networkActive)
	{
		/* TODO: Insert SSL read here into buffer */
		len = sslRead((SSL*)conn, buffer, sizeof(buffer));
		printf("read %d bytes from server.\n", len);

		/* END TODO */
		if (len < 0)
		{
			perror("Error reading");
		}
		if (len > 0)
		{
			printf("\n received something\n");
		}
		networkActive = (len > 0);

		if(networkActive)
			handleNetwork(buffer, len);
	}

	printf("Exiting network listener thread\n");

	/* TODO: Stop the client loop and call EXIT_THREAD */
	EXIT_THREAD(conn);
	/* END TODO */
}

void flushInput()
{
	char c;

	while((c = getchar()) != '\n' && c != EOF);
}

void getParams(int32_t *params)
{
	printf("Enter distance/angle in cm/degrees (e.g. 50) and power in %% (e.g. 75) separated by space.\n");
	printf("E.g. 50 75 means go at 50 cm at 75%% power for forward/backward, or 50 degrees left or right turn at 75%%  power\n");
	scanf("%d %d", &params[0], &params[1]);
	flushInput();
}

void *writerThread(void *conn)
{
	int quit=0;

	while(!quit)
	{
		char ch;
		printf("Command (f=forward, b=reverse, l=turn left, r=turn right, Q=STOP, c=clear stats, g=get stats, o=identify object, x=exit, wasd=imu control, i=getIMU)\n");
		scanf("%c", &ch);

		// Purge extraneous characters from input stream
		flushInput();

		char buffer[10];
		int32_t params[2];

		buffer[0] = NET_COMMAND_PACKET;
		switch(ch)
		{
			case 'f':
			case 'F':
			case 'b':
			case 'B':
			case 'l':
			case 'L':
			case 'r':
			case 'R':
			case 'a':
			case 'A':
			case 'd':
			case 'D':
				getParams(params);
				buffer[1] = ch;
				memcpy(&buffer[2], params, sizeof(params));
				sendData(conn, buffer, sizeof(buffer));
				break;
			case 'c':
			case 'C':
			case 'g':
			case 'G':
			case 'o':
			case 'O':
			case 'i':
			case 'I':
			case 'w':
			case 'W':
			case 's':
			case 'S':
			case 'q':
			case 'Q':
				params[0]=0;
				params[1]=0;
				memcpy(&buffer[2], params, sizeof(params));
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
			case 'x':
			case 'X':
				quit=1;
				break;
			// detect object command
			default:
				printf("BAD COMMAND\n");
		}
	}

	printf("Exiting keyboard thread\n");

	/* TODO: Stop the client loop and call EXIT_THREAD */
	EXIT_THREAD(conn);
	/* END TODO */
}

/* TODO: #define filenames for the client private key, certificatea,
   CA filename, etc. that you need to create a client */
#define KEY_FNAME "laptop.key"
#define CERT_FNAME "laptop.crt"
#define CA_CERT_NAME "signing.pem"
#define CLIENT_NAME "laptop.epp.com"

/* END TODO */
void connectToServer(const char *serverName, int portNum)
{
	/* TODO: Create a new client */
	createClient(serverName, portNum, 1, CA_CERT_NAME, CLIENT_NAME, 1, CERT_FNAME, KEY_FNAME, readerThread, writerThread);
	/* END TODO */
}

int main(int ac, char **av)
{
	if(ac != 3)
	{
		fprintf(stderr, "\n\n%s <IP address> <Port Number>\n\n", av[0]);
		exit(-1);
	}

	networkActive = 1;
	connectToServer(av[1], atoi(av[2]));

	/* TODO: Add in while loop to prevent main from exiting while the
	   client loop is running */
	while(client_is_running());
	/* END TODO */
	printf("\nMAIN exiting\n\n");
}
