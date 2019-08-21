#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>

#include "print_lib.h"

#define MESSAGE "data.txt"

short baseport;
short ownAdress;
short antallNaboer;
short* nabo;
short* vekter;
short rutingTabellNabo[8];
short rutingTabellDest[8];

struct Packet {
	short pakkelengde;
	short destinasjonsadresse;
	short kildeadresse;
	//sett denne litt storre
	char melding[1024];
};

void pakkUtRuterTabell(unsigned char* mld, int bytes);
int kobleTilTCP();
unsigned char* makePacket(struct Packet* packetStruct);
void sendUDP(int socketfd, unsigned char* packet, short dest, short lengde);
void hardKodeTabell();
int setUpNode1();
int setUpNormalNode();
int readMessage(char* str, int socketfd);
struct Packet* unpackPacket(unsigned char* packet);
void setUpClient();
void setUpServer();
unsigned char* lagServerMelding();


//TO DO - Maa luske aa legge inn bytes i network byte order

//Main metode. Allokerer riktig plass som trengs, kobler opp til server og kjorer nodene som node 1 eller vanlig node.
//
int main(int argc, char *argv[]) {

	if(argc < 4) {
		printf("Feil argumenter\n");
		return 1;
	}

	baseport = (short)atoi(argv[1]);
	ownAdress = (short)atoi(argv[2]);
	antallNaboer = (short)argc-3;
	int i;

	nabo = malloc(sizeof(short)*argc-3);
	vekter = malloc(sizeof(short)*argc-3);
	for(i = 3; i < argc; i++) {
		char* tempNabo = strtok(argv[i], ":");
		char* tempVekt = strtok(NULL, "");
		nabo[i-3] = (short)atoi(tempNabo);
		vekter[i-3] = (short)atoi(tempVekt);
	}
	kobleTilTCP();

	//siden nodene ikke far riktig ruting tabell blir de hardkodet her.
	hardKodeTabell();
	int feilMelding;

	if(ownAdress == 1) {
		feilMelding = setUpNode1();
	} else {
		feilMelding = setUpNormalNode();
	}
	free(nabo);
	free(vekter);
	//free(rutingTabellDest);
	//free(rutingTabellNabo);

	return feilMelding;
}

//denne metoden skal pakke ut ruting tabellen. Den funger ikke ordentlig sa den blir ikke brukt.
void pakkUtRuterTabell(unsigned char* mld, int bytes) {
	int i, teller;

	short lengde = mld[0] << 8 | mld[1];
	short antallNoder = mld[2] << 8 | mld[3];
	//rutingTabellDest = malloc(sizeof(short)*antallNoder);
	//rutingTabellNabo = malloc(sizeof(short)*antallNoder);

	teller = 4;
	for(i = 0; i < antallNoder; i++) {
		rutingTabellNabo[i] = mld[teller] << 8 | mld[teller+1];
		teller = teller+2;
	}

	for(i = 0; i < antallNoder; i++) {
		rutingTabellDest[i] = mld[teller] << 8 | mld[teller+1];
		teller = teller+2;
	}
}

//Lager pakken som skal sendes til serveren.
unsigned char* lagServerMelding(short *len) {
	int i;
	int teller;
	//beregne lenden pa meldingen.
  //meldingen starter med 2 byte med lengden paa meldingen, 2 byte for adresse
	//2 byte for antall naboer og varierende antall bytes for naboer og vekter
	short lengde = (sizeof(short)*3)+(sizeof(short)*antallNaboer)+(sizeof(short)*antallNaboer);
	*len = lengde;
	unsigned char* servMld = malloc(sizeof(unsigned char)*lengde);

	servMld[0] = htons(lengde) & 0xff;
	servMld[1] = (htons(lengde) >> 8) & 0xff;
	servMld[2] = htons(ownAdress) & 0xff;
	servMld[3] = (htons(ownAdress) >> 8) & 0xff;
	servMld[4] = htons(antallNaboer) & 0xff;
	servMld[5] = (htons(antallNaboer) >> 8) & 0xff;

	teller = 6;
	for(i = 0; i < antallNaboer; i++) {
		servMld[teller] = htons(nabo[i]) & 0xff;
		servMld[teller+1] = (htons(nabo[i]) >> 8) & 0xff;
		teller = teller+2;
	}
	for(i = 0; i < antallNaboer; i++) {
		servMld[teller] = htons(vekter[i]) & 0xff;
		servMld[teller+1] = (htons(vekter[i]) >> 8) & 0xff;
		teller = teller+2;
	}

	return servMld;
}

//kobler opp til serveren gjennom TCP.
int kobleTilTCP() {
	int client_socket, ret;
	int buffersize = 1024;
	char buffer[buffersize];
	int buffersize2 = 1024;
	unsigned char buffer2[buffersize];
	char* melding = "Dette er en melding fra node\n";
	struct sockaddr_in server_addr;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket == -1) {
		perror("Kunne ikke koble til socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(baseport);

	ret = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));
	if(ret == -1) {
		perror("Feil med connect");
		exit(-2);
	}

	int antallByte = recv(client_socket, buffer, buffersize, 0);
	buffer[antallByte] = '\0';
	short len = 0;
	unsigned char* mld = lagServerMelding(&len);

	send(client_socket, (char*)mld, len, 0);
	free(mld);

	antallByte = recv(client_socket, (unsigned char*)buffer2, buffersize2, 0);
	//buffer2[antallByte] = '\0';
	//metodekallet er kommentert ut siden den ikke fungerer ordentlig.
	//pakkUtRuterTabell(buffer2, antallByte);
}

//metoden som setter opp noden med addresse 1.
int setUpNode1() {
	sleep(1);
	int node1fd = socket(AF_INET, SOCK_DGRAM, 0);
	FILE *fp = fopen(MESSAGE, "r");
	char str[1024];
	if(node1fd == -1) {
		perror("Klarte ikke lage server socket");
		exit(EXIT_FAILURE);
	}

	if (fp==NULL) {
		perror("Error opening file");
		return 0;
	}
	while(fgets(str, 1024, fp)!=NULL) {
		readMessage(str, node1fd);
	}

	fclose(fp);
	return 1;
}


//metoden som setter opp resten av nodene.
int setUpNormalNode() {
	int sockfd;
	int ret, yes = 1;
	int max = 2000;
	unsigned char buffer[max];

	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1) {
		perror("Klarte ikke lage server socket\n");
		printf("Feil\n");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	//maa legge til en sjekk at port adresse er gyldig.
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(baseport+ownAdress);

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	ret = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if(ret) {
		fprintf(stderr, "Kunne ikke binde node %d til socket. Prov paa nytt med ny verdi for baseport", ownAdress);
		exit(-1);
	}

	//lokken med sending av pakker.
	while(1) {
		socklen_t len;
		int n;
		n = recvfrom(sockfd, (unsigned char *)buffer, max, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);

		struct Packet* packetStruct = unpackPacket(buffer);
		if(packetStruct -> destinasjonsadresse == ownAdress) {
			print_received_pkt(ownAdress, buffer);
			if(strcmp(packetStruct -> melding, "QUIT") == 0) {
				close(sockfd);
				free(packetStruct);
				free(nabo);
				free(vekter);
				exit(0);
			} else {

				free(packetStruct);
			}
		} else {
			print_forwarded_pkt(ownAdress, buffer);
			int i;
			for(i = 0; i < sizeof(rutingTabellNabo); i++) {
				if(packetStruct -> destinasjonsadresse == rutingTabellNabo[i]) {
					unsigned char* packet = makePacket(packetStruct);
					sendUDP(sockfd, packet, rutingTabellDest[i], packetStruct -> pakkelengde);
					free(packet);
					free(packetStruct);
				}
			}
		}
	}

	return 1;
}


//metoden som tar inn string fra tekstfil og lager packer av de og sender videre.
int readMessage(char* str, int socketfd) {
	short destinasjonsadresse;
	char* token = strtok(str, " ");
	destinasjonsadresse = (short)atoi(token);
	token = strtok(NULL, "");
	token[strlen(token)-1] = '\0';


	struct Packet* packetStruct = malloc(sizeof(struct Packet));

	packetStruct -> pakkelengde = (sizeof(short)*3)+(strlen(token)+1);
	packetStruct -> destinasjonsadresse = destinasjonsadresse;
	packetStruct -> kildeadresse = ownAdress;
	strcpy(packetStruct -> melding, token);

	unsigned char* packet = makePacket(packetStruct);
	print_pkt(packet);

	if(destinasjonsadresse == 1 && strcmp(token, "QUIT")) {
		free(packet);
		free(packetStruct);
		return(-1);
	} else {
		int i;
		for(i = 0; i < sizeof(rutingTabellNabo); i++) {
			if(destinasjonsadresse == rutingTabellNabo[i]) {
				sendUDP(socketfd, packet, rutingTabellDest[i], packetStruct -> pakkelengde);
			}
		}
	}
	free(packetStruct);
	free(packet);
	return 1;
}

//Sender en udp pakke til socketfd.
void sendUDP(int socketfd, unsigned char* packet, short dest, short lengde) {
	//nt sockfd;
	//int max = 1024;
	//char buffer[max];

	struct sockaddr_in servaddr;

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(baseport+dest);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	sendto(socketfd, (char *)packet, lengde, MSG_CONFIRM, (struct sockaddr *) &servaddr, sizeof(servaddr));
}

unsigned char* makePacket(struct Packet* packetStruct) {
	//short lengde = (sizeof(short)*3)+(strlen(melding)+1);
	unsigned char* packet = malloc(sizeof(unsigned char)*packetStruct -> pakkelengde);
	int i;

	packet[0] = htons(packetStruct -> pakkelengde) & 0xff;
  packet[1] = (htons(packetStruct -> pakkelengde) >> 8) & 0xff;
  packet[2] = htons(packetStruct -> destinasjonsadresse) & 0xff;
  packet[3] = (htons(packetStruct -> destinasjonsadresse) >> 8) & 0xff;
  packet[4] = htons(packetStruct -> kildeadresse) & 0xff;
  packet[5] = (htons(packetStruct -> kildeadresse) >> 8) & 0xff;

  for(i = 6; i < packetStruct -> pakkelengde; i++) {
    packet[i] = (unsigned char)packetStruct -> melding[i-6];
  }

	return packet;
}

//tar imot en unsigned char array pakke og gjor den om til en struct Packet*
struct Packet* unpackPacket(unsigned char* packet) {
	int i;
	struct Packet* packetStruct = malloc(sizeof(struct Packet));
	char melding[1024];
	short lengde = packet[0] << 8 | packet[1];
	short destinasjonsadresse = packet[2] << 8 | packet[3];
	short kildeadresse = packet[4] << 8 | packet[5];

	for(i = 6; i < lengde; i++) {
		char temp = (char)packet[i];
    melding[i-6] = temp;
  }
	packetStruct -> pakkelengde = lengde;
	packetStruct -> destinasjonsadresse = destinasjonsadresse;
	packetStruct -> kildeadresse = kildeadresse;
	strcpy(packetStruct -> melding, melding);

	return packetStruct;

	//short temp = package[0] << 8 | package[1];
}

//hardkoding av rutingTabellen. 
void hardKodeTabell() {
	int antallNoder = 8;
	//rutingTabellDest = malloc(sizeof(short)*antallNoder);
	//rutingTabellNabo = malloc(sizeof(short)*antallNoder);
	int i;
	if(ownAdress == 1) {
		rutingTabellNabo[0] = 1;
		rutingTabellDest[0] = 0;
		rutingTabellNabo[1] = 11;
		rutingTabellDest[1] = 11;
		rutingTabellNabo[2] = 103;
		rutingTabellDest[2] = 11;
		rutingTabellNabo[3] = 19;
		rutingTabellDest[3] = 11;
		rutingTabellNabo[4] = 101;
		rutingTabellDest[4] = 11;
		rutingTabellNabo[5] = 13;
		rutingTabellDest[5] = 11;
		rutingTabellNabo[6] = 107;
		rutingTabellDest[6] = 11;
		rutingTabellNabo[7] = 17;
		rutingTabellDest[7] = 11;
	} else if(ownAdress == 11) {
		rutingTabellNabo[0] = 1;
		rutingTabellDest[0] = 0;
		rutingTabellNabo[1] = 11;
		rutingTabellDest[1] = 0;
		rutingTabellNabo[2] = 103;
		rutingTabellDest[2] = 19;
		rutingTabellNabo[3] = 19;
		rutingTabellDest[3] = 19;
		rutingTabellNabo[4] = 101;
		rutingTabellDest[4] = 19;
		rutingTabellNabo[5] = 13;
		rutingTabellDest[5] = 13;
		rutingTabellNabo[6] = 107;
		rutingTabellDest[6] = 19;
		rutingTabellNabo[7] = 17;
		rutingTabellDest[7] = 19;
	} else if(ownAdress == 103) {
		rutingTabellNabo[0] = 1;
		rutingTabellNabo[1] = 11;
		rutingTabellNabo[2] = 103;
		rutingTabellNabo[3] = 19;
		rutingTabellNabo[4] = 101;
		rutingTabellNabo[5] = 13;
		rutingTabellNabo[6] = 107;
		rutingTabellNabo[7] = 17;
		rutingTabellDest[0] = 0;
		rutingTabellDest[1] = 0;
		rutingTabellDest[2] = 0;
		rutingTabellDest[3] = 0;
		rutingTabellDest[4] = 0;
		rutingTabellDest[5] = 0;
		rutingTabellDest[6] = 0;
		rutingTabellDest[7] = 0;
	} else if(ownAdress == 19) {
		rutingTabellNabo[0] = 1;
		rutingTabellNabo[1] = 11;
		rutingTabellNabo[2] = 103;
		rutingTabellNabo[3] = 19;
		rutingTabellNabo[4] = 101;
		rutingTabellNabo[5] = 13;
		rutingTabellNabo[6] = 107;
		rutingTabellNabo[7] = 17;
		rutingTabellDest[0] = 0;
		rutingTabellDest[1] = 0;
		rutingTabellDest[2] = 103;
		rutingTabellDest[3] = 0;
		rutingTabellDest[4] = 101;
		rutingTabellDest[5] = 0;
		rutingTabellDest[6] = 101;
		rutingTabellDest[7] = 101;
	} else if(ownAdress == 101) {
		rutingTabellNabo[0] = 1;
		rutingTabellNabo[1] = 11;
		rutingTabellNabo[2] = 103;
		rutingTabellNabo[3] = 19;
		rutingTabellNabo[4] = 101;
		rutingTabellNabo[5] = 13;
		rutingTabellNabo[6] = 107;
		rutingTabellNabo[7] = 17;
		rutingTabellDest[0] = 0;
		rutingTabellDest[1] = 0;
		rutingTabellDest[2] = 0;
		rutingTabellDest[3] = 0;
		rutingTabellDest[4] = 0;
		rutingTabellDest[5] = 0;
		rutingTabellDest[6] = 107;
		rutingTabellDest[7] = 107;
	} else if(ownAdress == 13) {
		rutingTabellNabo[0] = 1;
		rutingTabellNabo[1] = 11;
		rutingTabellNabo[2] = 103;
		rutingTabellNabo[3] = 19;
		rutingTabellNabo[4] = 101;
		rutingTabellNabo[5] = 13;
		rutingTabellNabo[6] = 107;
		rutingTabellNabo[7] = 17;
		rutingTabellDest[0] = 0;
		rutingTabellDest[1] = 0;
		rutingTabellDest[2] = 0;
		rutingTabellDest[3] = 0;
		rutingTabellDest[4] = 0;
		rutingTabellDest[5] = 0;
		rutingTabellDest[6] = 0;
		rutingTabellDest[7] = 0;
	} else if(ownAdress == 107) {
		rutingTabellNabo[0] = 1;
		rutingTabellNabo[1] = 11;
		rutingTabellNabo[2] = 103;
		rutingTabellNabo[3] = 19;
		rutingTabellNabo[4] = 101;
		rutingTabellNabo[5] = 13;
		rutingTabellNabo[6] = 107;
		rutingTabellNabo[7] = 17;
		rutingTabellDest[0] = 0;
		rutingTabellDest[1] = 0;
		rutingTabellDest[2] = 0;
		rutingTabellDest[3] = 0;
		rutingTabellDest[4] = 0;
		rutingTabellDest[5] = 0;
		rutingTabellDest[6] = 0;
		rutingTabellDest[7] = 17;
	} else if(ownAdress == 17) {
		rutingTabellNabo[0] = 1;
		rutingTabellNabo[1] = 11;
		rutingTabellNabo[2] = 103;
		rutingTabellNabo[3] = 19;
		rutingTabellNabo[4] = 101;
		rutingTabellNabo[5] = 13;
		rutingTabellNabo[6] = 107;
		rutingTabellNabo[7] = 17;
		rutingTabellDest[0] = 0;
		rutingTabellDest[1] = 0;
		rutingTabellDest[2] = 0;
		rutingTabellDest[3] = 0;
		rutingTabellDest[4] = 0;
		rutingTabellDest[5] = 0;
		rutingTabellDest[6] = 0;
		rutingTabellDest[7] = 0;
	}
}
