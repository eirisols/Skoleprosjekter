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

//Beskrivelse av protkoll staar foran implementasjon av settOppTCP() og i README filen.

#define ANTALL_NODER 8
#define BACKLOG_SIZE 15

struct Node*** node_array;
short serverPort;
int antallNoder;
int kobledeNoder = 0;

int sjekkSti(struct Node* node, short adress);
void sjekkEdges();
unsigned char* lagRuterTabellPacket(struct Node* node, int *len);
int pakkUtMld(unsigned char* clientMld, struct Node*);
int haandterForbindelse(int server_socket, fd_set *fds);
void haandterMelding(int client_socket, fd_set *fds);
int settOppSocket();
void settOppTCP();
void fyllRuter();
void fyllInnRuterTabell(short naaverende, short destinasjon);
void lagRuterTabell();
struct Node* finnNode(short adresse);
void finnVei(short adresse);
void skrivUtAlleStier();
int djikstra();
int freeMinne();

struct Node {
  int fd;
  short ownAdress;
  short vekter[5];
  short naboer[5];
  int pathlen;
  short antallNaboer;
  short forrige;
  short ruter_tabell_noder[8];
  short ruter_tabell_nabo[8];
};


int main(int argc, char *argv[]) {

  if(argc != 3) {
    perror("Feil mengde argumenter");
    exit(EXIT_FAILURE);
  }

  serverPort = (short)atoi(argv[1]);
  antallNoder = atoi(argv[2]);

  struct Node** node_array1 = malloc(sizeof(struct Node*) * antallNoder);
  memset(node_array1, '\0', sizeof(struct Node* )*antallNoder);

  node_array = &node_array1;

  settOppTCP();
  freeMinne();
  free(node_array1);

  return 1;
}
void sjekkEdges() {
  struct Node** node_pointer = *node_array;
  int i, j, pathlen;
  for(i = 0; i < antallNoder; i++) {
    for(j = 0; j < antallNoder; j++) {
      int ret = sjekkSti(node_pointer[j], node_pointer[i] -> ownAdress);
      if(ret == 1) {
        pathlen = node_pointer[i] -> pathlen;
      } else {
        pathlen = -1;
      }
      print_weighted_edge(node_pointer[i] -> ownAdress, node_pointer[j] -> ownAdress, pathlen);
    }
  }
}

//hjelpemetode til sjekkEdges som ser om en node er paa en sti fra 1 til adress.
int sjekkSti(struct Node* node, short adress) {
  if(node -> ownAdress == adress) {
    return 1;
  } else if(node -> forrige == NULL) {
   return -1;
 }
 struct Node* forrigeNode = finnNode(node -> forrige);
 sjekkSti(forrigeNode, adress);
}

//PROTOKOLL!
//Protokollen til ruting serveren er som følger. Serveren venter på at nodene skal tilkoble seg og hver gang en node kobler seg til serveren sender serveren en liten beskjed om at
//koblingen er opprettet.
//Etter at noden mottar beskjed om at koblingen er opprettet sender den en pakke til serveren i form av et unsigned char array.
//Dette arrayet inneholder først to bytes som beskriver lengden på arrayet. Så to bytes som inneholder adressen til noden som pakken er sendt fra.
//Så to bits med antall naboer noden har. Etter dette kommer et variabelt antall bytes basert på antall naboer.Her kommer først variabelt antall bytes som har addressen til naboene.
//Så kommer variabelt antall bytes med vektene til kantene mellom node og nabo.
//Etter at nabo har sendt pakke til serveren venter den på en pakke med rutingtabell tilbake.

//Serveren holder orden på hvor mange pakker den har motatt. Når serveren har mottat pakker fra alle nodene, vil den så gå gjennom alle forbindelsene og
//sende passende rutingtabell tilbake til nodene. Denne pakken er et unsigned char arrray, hvor to første bytes beskriver lengde på pakke, neste to bytes er antall noder, så
//følger varierende antall bytes med først adresse til nodene, så følger varierende antall bytes med nabonoder pakkene rutes gjennom for å nå destinasjon.

//hovedprogrammet som setter opp TCP forbindelsen.
void settOppTCP() {
    int server_socket, addrlengde, ny_socket, client_socket[20], max_clients = 20,
    activity, i, valread, sd;
    int max_sd;

    struct Node** node_pointer = *node_array;

    char buffer [1025];

    fd_set readfds, fds;

    for(i = 0; i < antallNoder; i++) {
      struct Node* node = malloc(sizeof(struct Node));
      //node -> ruter_tabell_nabo = malloc(sizeof(short)*antallNoder);
      //node -> ruter_tabell_noder = malloc(sizeof(short)*antallNoder);
      memset(node, '\0', sizeof(struct Node));
      //memset(node->ruter_tabell_nabo, '\0', sizeof(short)*antallNoder);
      //memset(node->ruter_tabell_noder, '\0', sizeof(short)*antallNoder);
      node -> fd = -1;
      node_pointer[i] = node;
    }

    server_socket = settOppSocket();

    int largest_fd = server_socket;

    FD_ZERO(&fds);
    FD_SET(server_socket, &fds);

    while(1) {
      if(kobledeNoder >= antallNoder) {
        break;
      }
      readfds = fds;

      activity = select(largest_fd+1, &readfds, NULL, NULL, NULL);

      if((activity < 0)) {
        perror("Feil med select\n");
        return;
      }

      for(i = 0; i <= largest_fd; i++) {
        if(FD_ISSET(i, &readfds)) {

          if (i == server_socket) {

            int client_socket = haandterForbindelse(i, &fds);

            if(client_socket == -1) {
              continue;
            } else if(client_socket > largest_fd) {
              largest_fd = client_socket;
            }
          } else {
            haandterMelding(i, &fds);
            if(kobledeNoder >= antallNoder) {
              break;
            }
          }
        }
      } //for
    } //while
    djikstra();
    sjekkEdges();
    lagRuterTabell();
    fyllRuter();
    //skrivUtAlleStier();

    for(i = 0; i < antallNoder; i++) {
      //char* mld = "Her skal rutertabell komme";
      int client_socket = node_pointer[i] -> fd;
      int len = 0;
      unsigned char* mld = lagRuterTabellPacket(node_pointer[i], &len);
      send(client_socket, (char*)mld, len, 0);
      free(mld);
    }
}

//metode som lager en packet med rutingTabell som skal sendes til node.
unsigned char* lagRuterTabellPacket(struct Node* node, int *len) {
  int i, teller;

  short lengde = (sizeof(short)*2)+(sizeof(short)*antallNoder)+(sizeof(short)*antallNoder);
  *len = lengde;
  unsigned char* clientMld = malloc(sizeof(unsigned char)*lengde);

	clientMld[0] = htons(lengde) & 0xff;
	clientMld[1] = (htons(lengde) >> 8) & 0xff;
	clientMld[2] = htons(antallNoder) & 0xff;
	clientMld[3] = (htons(antallNoder) >> 8) & 0xff;

	teller = 4;
	for(i = 0; i < antallNoder; i++) {
		clientMld[teller] = htons(node -> ruter_tabell_nabo[i]) & 0xff;
	  clientMld[teller+1] = (htons(node -> ruter_tabell_nabo[i]) >> 8) & 0xff;
		teller = teller+2;
	}
	for(i = 0; i < antallNoder; i++) {

		clientMld[teller] = htons(node -> ruter_tabell_noder[i]) & 0xff;
		clientMld[teller+1] = (htons(node -> ruter_tabell_noder[i]) >> 8) & 0xff;
		teller = teller+2;
	}

	return clientMld;
}

//pakker ut meldingen med naboer og vekter fra node.
int pakkUtMld(unsigned char* clientMld, struct Node* node) {
  int i, teller;


  short lengde = clientMld[0] << 8 | clientMld[1];
	node->ownAdress = clientMld[2] << 8 | clientMld[3];
	node ->antallNaboer = clientMld[4] << 8 | clientMld[5];
  teller = 6;
  for(i = 0; i<node->antallNaboer; i++) {
    node -> naboer[i] = clientMld[teller] << 8 | clientMld[teller+1];
    teller = teller+2;
  }

  for(i = 0; i < node->antallNaboer; i++) {
    node -> vekter[i] = clientMld[teller] << 8 | clientMld[teller+1];
    teller = teller+2;
  }
  return 1;
}

void haandterMelding(int client_socket, fd_set *fds) {
  int i, buffersize = 1024;
	unsigned char buffer[buffersize];
  struct Node** node_pointer = *node_array;
  struct Node* node;

  int antallByte = recv(client_socket, (unsigned char*)buffer, buffersize, 0);
  for(i = 0; i < antallNoder; i++) {
    if(node_pointer[i] -> fd == client_socket) {
      node = node_pointer[i];
      break;
    }
  }
  pakkUtMld(buffer, node);
  kobledeNoder++;
}

int haandterForbindelse(int server_socket, fd_set *fds) {
  int client_socket;
  char* melding = "Forbindelse opprettet";
  struct sockaddr_in clientAddr;
  socklen_t addrlen;
  int i;
  short tempAddr;
  struct Node** node_pointer = *node_array;


  addrlen = sizeof(struct sockaddr_in);

  client_socket = accept(server_socket, (struct sockaddr*)&clientAddr, &addrlen);
  if(client_socket == -1) {
    perror("Feil med accept");
    return -1;
  }

  for(i = 0; i < antallNoder; i++) {
    if(node_pointer[i] -> fd == -1) {
      node_pointer[i] -> fd = client_socket;
      tempAddr = (short)(ntohs(clientAddr.sin_port) - serverPort);
      node_pointer[i] -> ownAdress = tempAddr;

      send(client_socket, melding, strlen(melding), 0);
      break;
    }
  }
  FD_SET(client_socket, fds);
  return client_socket;
}

int settOppSocket() {
  int server_socket = 0;
  int yes = 1;
  int ret;

  struct sockaddr_in serverAdresse;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(server_socket == 0) {
    perror("Kunne ikke opprette socket");
    exit(EXIT_FAILURE);
  }

  serverAdresse.sin_family = AF_INET;
  serverAdresse.sin_port = htons(serverPort);
  serverAdresse.sin_addr.s_addr = INADDR_ANY;

  setsockopt(server_socket, SOL_SOCKET,  SO_REUSEADDR, &yes, sizeof(int));

  ret = bind(server_socket, (struct sockaddr*)&serverAdresse, sizeof(serverAdresse));
  if(ret) {
    perror("kunne ikke binde socket til port");
    exit(EXIT_FAILURE);
  }

  ret = listen(server_socket, BACKLOG_SIZE);
  if(ret == -1) {
    perror("Feil ved listen");
    exit(EXIT_FAILURE);
  }

  return server_socket;
}

void fyllRuter() {
  struct Node** node_pointer = *node_array;
  int i;

  for(i = 0; i < antallNoder; i++) {
    fyllInnRuterTabell(node_pointer[i] -> ownAdress, node_pointer[i] -> ownAdress);
  }
}

void fyllInnRuterTabell(short naaverende, short destinasjon) {
  struct Node* node = finnNode(naaverende);
  if(node -> forrige == NULL) {
    return;
  }
  struct Node* forrigeNode = finnNode(node -> forrige);
  int i;

  for(i = 0; i < antallNoder; i++) {
    if(forrigeNode -> ruter_tabell_noder[i] == destinasjon) {
      forrigeNode -> ruter_tabell_nabo[i] = naaverende;
      break;
    }
  }

  if(node -> forrige != NULL) {
    fyllInnRuterTabell(node -> forrige, destinasjon);
  }
}

void lagRuterTabell() {
   struct Node** node_pointer = *node_array;
   int i, j;

   for(i = 0; i < antallNoder; i++) {
    for(j = 0; j < antallNoder; j++) {
      node_pointer[i] -> ruter_tabell_noder[j] = node_pointer[j] -> ownAdress;
      node_pointer[i] -> ruter_tabell_nabo[j] = 0;
    }
  }
}



void skrivUtAlleStier() {
  struct Node** node_pointer = *node_array;
  int i, j;
  for(i = 0; i<ANTALL_NODER; i++) {
    finnVei(node_pointer[i] -> ownAdress);
    printf("RuterTabell: \n");
    for(j = 0; j < ANTALL_NODER; j++) {
      printf("%d - %d\n\n", node_pointer[i] -> ruter_tabell_noder[j], node_pointer[i] -> ruter_tabell_nabo[j]);
    }
  }
}

struct Node* finnNode(short adresse) {
  int i;
  struct Node** node_pointer = *node_array;
  for(i = 0; i<ANTALL_NODER; i++) {
    if(node_pointer[i] -> ownAdress == adresse) {
      return node_pointer[i];
    }
  }
  return NULL;
}

void finnVei(short adresse) {
  struct Node* node = finnNode(adresse);

  printf("Slutt node - %d\n", node -> ownAdress);

  while(node->forrige!=NULL) {
    node = finnNode(node->forrige);

    printf("Gar bakover til - %d\n\n", node ->ownAdress);
  }
}

int djikstra() {
  //maa hente inn local pointer
  struct Node** node_pointer = *node_array;
  int antall = ANTALL_NODER;
  //dette er listen over tilgjengelige noder
  short listeNode[ANTALL_NODER];
  //variabler som brukes av algrotitmen
  int i;
  int j;
  //int tempDist;
  struct Node* node;
  //int dist = 0;
  //initialisering av algoritmen
  for(i = 0; i<ANTALL_NODER; i++) {
    if(node_pointer[i] -> ownAdress==1) {
      node_pointer[i] -> pathlen = 0;
    } else {
      node_pointer[i] -> pathlen = 100;
    }
    listeNode[i] = node_pointer[i] -> ownAdress;
    node_pointer[i] -> forrige = NULL;
  }
  //setter node med ownAdress 1 som forste node
  //hovedloop til algoritmen
  while(antall>0) {
    int tempDist = 1000;
    for(i = 0; i<ANTALL_NODER; i++) {
      if(node_pointer[i] -> pathlen < tempDist && listeNode[i] != 0) {
        tempDist = node_pointer[i] -> pathlen;
        node = node_pointer[i];
      }
    }

    for(i = 0; i<ANTALL_NODER; i++) {
      if(listeNode[i] == node -> ownAdress) {
        listeNode[i] = 0;
        antall--;
      }
    }
    //gaar gjennom naboer til node
    for(i = 0; i<node -> antallNaboer; i++) {
      for(j = 0; j<ANTALL_NODER; j++) {
        if(node->naboer[i] == node_pointer[j]->ownAdress && listeNode[j] != 0) {
          tempDist = node->pathlen + node->vekter[i];
          if(tempDist<node_pointer[j] -> pathlen) {
            node_pointer[j] -> pathlen = tempDist;
            node_pointer[j] -> forrige = node->ownAdress;
          }
        }
      }
    }
  }
  return 1;
}

int freeMinne() {
  struct Node** node_pointer = *node_array;
  int i;
  for(i = 0; i < ANTALL_NODER; i++) {
    //free(node_pointer[i]->ruter_tabell_nabo);
    //free(node_pointer[i]->ruter_tabell_noder);
    free(node_pointer[i]);
  }
  return 1;
}
