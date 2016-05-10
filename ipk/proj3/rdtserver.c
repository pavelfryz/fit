/*
 * Soubor:   rdtserver.c
 * Autor:    xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt:  Klient/Server - RDT
 * Popis:    Server prijima a potvrzuje prijate pakety, data
 *           paketu vypisuje na standardní výstup
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "udt.h"

#define b64_ntop __b64_ntop
#define b64_pton __b64_pton

/* Deklarace funkci */
int b64_ntop(unsigned char const *src, size_t srclength, char *target, size_t targsize);
int b64_pton(char const *src, unsigned char *target, size_t targsize);
void parsePacket();
void parseHeader(xmlDocPtr doc, xmlNodePtr cur, int *id);
int parseArgs(int argc, char **argv);
void createAckPacket();
int initSignals();
void handler(int signum);

in_addr_t dest_addr = 0x7f000001;
in_port_t source_port = 0;
in_port_t dest_port = 0;
bool cont = true;

#define BUFFERSIZE 512   //velikost bufferu

char buffer[BUFFERSIZE]; //buffer
int expected = 0;        //id ocekavaneho paketu

/**
 * Hlavni program
 */
int main(int argc, char **argv ) {
  /* zpracovani parametru */
  if(parseArgs(argc,argv))
    return 1;

  /* nastaveni signalu */
  if(initSignals())
    return 2;

  /* inicializace udt */
  int udt = udt_init(source_port);

  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(udt, &readfds);
  int ret;

  while (cont && (ret=select(udt+1, &readfds, NULL, NULL, NULL))) {
    /* Zpracovani prijateho paketu */
    if (FD_ISSET(udt, &readfds)) {
      int n = udt_recv(udt, buffer, BUFFERSIZE-1, NULL, NULL);
      buffer[n] = 0;
      parsePacket();
      createAckPacket();
      if (!udt_send(udt, dest_addr, dest_port, buffer, strlen(buffer))) {
        perror("rdtserver: ");
      }
    }

    FD_ZERO(&readfds);
    FD_SET(udt, &readfds);
  }

  xmlCleanupParser();
  close(udt);
  fflush(stdout);
  return EXIT_SUCCESS;
}

/**
 * Funkce na obsluhu signalu
 * @param signum cislo signalu
 */
void handler(int signum){
  cont = false;
}

/**
 * Nastavi obsluhu signalu
 */
int initSignals(){
  struct sigaction sa = { };
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = handler;

  if (sigaction(SIGHUP, &sa, NULL) || sigaction(SIGINT, &sa, NULL) ||
      sigaction(SIGQUIT, &sa, NULL) || sigaction(SIGABRT, &sa, NULL) ||
      sigaction(SIGTERM, &sa, NULL)) {
    perror("rdtserver: ");
    return -1;
  }
  return 0;
}

/**
 * Vytvori odpoved na paket
 */
void createAckPacket(){
  sprintf(buffer, "%s%d%s",
  "<rdt-segment id=\"xfryzp00\">"
    "<header>"
      "<id>",expected,"</id>"
    "</header>"
    "<data>"
    "</data>"
  "</rdt-segment>");
}

/**
 * Zparcuje paket
 */
void parsePacket(){
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;
  xmlNodePtr cur = NULL;
  xmlChar *data;
  int id;

  /* Nacte paket do xml dokumentu */
  doc = xmlReadMemory(buffer, strlen(buffer), NULL, NULL, 0);
  if (doc == NULL) return;
  root = xmlDocGetRootElement(doc);
  cur = root->xmlChildrenNode;
  /* Zpracuje hlavicku a data paketu */
  while(cur != NULL){
    if (!xmlStrcmp(cur->name, "header")){
      parseHeader(doc, cur, &id);
    }
    if (!xmlStrcmp(cur->name, "data")){
      data = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
    }
    cur = cur->next;
  }

  /* Ocekavany paket */
  if(id==expected){
    /* Prevod z base 64*/
    char databuffer[BUFFERSIZE];
    int n = b64_pton (data, databuffer, BUFFERSIZE-1);
    /* Vypis na standartni vystup */
    fwrite(databuffer, sizeof(char),  n, stdout);
    expected++;
  }
  xmlFree(data);
  xmlFreeDoc(doc);
}

/**
 * Zpracuje hlavicku paketu
 * @param doc Packet
 * @param cur Hlavicka paketu
 * @param id Id paketu
 */
void parseHeader(xmlDocPtr doc, xmlNodePtr cur, int *id){
  xmlChar *data;
  cur = cur->xmlChildrenNode;
  while (cur != NULL) {
    if ((!xmlStrcmp(cur->name, "id"))) {
      data = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      *id=atol(data);
      xmlFree(data);
    }
    cur = cur->next;
  }
}


/**
 * Zpracuje argumenty prikazove radky
 * @param argc Pocet argumentu
 * @param argv Pole s argumenty
 */
int parseArgs(int argc, char **argv)
{
  char usage[] = "rdtserver -s source_port -d dest_port\n";
  int ch;
  while ((ch = getopt(argc,argv,"s:d:")) != -1) {
    switch(ch) {
    case 's':
      source_port = atol(optarg);
      break;
    case 'd':
      dest_port = atol(optarg);
      break;
    default:
      fprintf(stderr, "%s", usage);
      return -1;
    }
  }
  if (argc - optind || source_port == 0 || dest_port == 0) {
    fprintf(stderr, "%s", usage);
    return -1;
  }
  return 0;
}
