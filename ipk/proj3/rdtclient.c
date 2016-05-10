/*
 * Soubor:   rdtclient.c
 * Autor:    xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt:  Klient/Server - RDT
 * Popis:    Klient cte data ze vstupu a posila je serveru.
 *           Komunikace probiha nad udp.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/time.h>
#include <signal.h>
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
void createPacket(int id,char *data,int length);
int newWindow();
void freeWindow();
void sendAll();
void sigalrm_handler(int sig);

in_addr_t dest_addr = 0x7f000001;
in_port_t source_port = 0;
in_port_t dest_port = 0;
bool cont = true;

#define BUFFERSIZE 512    //velikost bufferu
#define PROTOCOLSIZE 90   //velikost protokolu bez dat(s malou rezervou)
#define DATASIZE 300      //velikost dat pred prekodaovanim do base64(s cca 400)
#define WINDOWSIZE 5      //velikost okna
#define TIMEOUT 750000    //casovac v mikrosekundach

char buffer[BUFFERSIZE];  //buffer
char *window[WINDOWSIZE]; //okno
int windowpos = 0;        //pozice okna
int packetpos = 0;        //pozice posledniho paketu
int udt;

/**
 * Hlavni program
 */
int main(int argc, char **argv ) {
  /* zpracovani parametru */
  if(parseArgs(argc,argv))
    return 1;

  /* inicializace udt */
  udt = udt_init(source_port);

  /* vytvoreni okna */
  if(newWindow())
    return 2;

  /* nastaveni casovace */
  signal(SIGALRM, sigalrm_handler);
  struct itimerval itv;
  itv.it_interval.tv_sec = 0;
  itv.it_interval.tv_usec = TIMEOUT;
  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = TIMEOUT;
  sigset_t sigmask;
  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGALRM);

  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(udt, &readfds);
  FD_SET(STDIN_FILENO, &readfds);
  int n;

  while ((cont || windowpos!=packetpos) && (n=select(udt+1, &readfds, NULL, NULL, NULL))) {
    /* Nacteni dat ze vstupu a poslani na server */
    if (FD_ISSET(STDIN_FILENO, &readfds) && packetpos-windowpos<WINDOWSIZE) {
      if((n=fread(buffer, sizeof(char), DATASIZE-1, stdin))>0){
        sigprocmask(SIG_BLOCK, &sigmask, NULL);
        createPacket(packetpos,buffer,n);
        sendAll();
        setitimer(ITIMER_REAL, &itv, NULL);
        packetpos++;
        sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
      }
      else{
        if(feof(stdin))
          cont = false;
      }
    }

    /* Obsluha potvrzovaciho paketu */
    if (FD_ISSET(udt, &readfds)) {
      sigprocmask(SIG_BLOCK, &sigmask, NULL);
      n = udt_recv(udt, buffer, BUFFERSIZE-1, NULL, NULL);
      buffer[n] = 0;
      parsePacket();
      sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
    }

    FD_ZERO(&readfds);
    FD_SET(udt, &readfds);
    FD_SET(STDIN_FILENO, &readfds);
  }

  xmlCleanupParser();
  close(udt);
  return EXIT_SUCCESS;
}

/**
 * Posle vsechny pakety v okne
 */
void sendAll(){
  for(int i=windowpos;i<packetpos;++i){
    if (!udt_send(udt, dest_addr, dest_port, window[i%WINDOWSIZE], strlen(window[i%WINDOWSIZE]))) {
      perror("rdtclient: ");
    }
  }
}

/**
 * Obslouzi signal casovace
 */
void sigalrm_handler(int sig){
  sendAll();
  signal(SIGALRM, sigalrm_handler);
}

/**
 * Vytvori paket
 * @param id pozice paketu
 * @param data data paketu
 * @param length delka dat
 */
void createPacket(int id,char *data,int length){
  char target[BUFFERSIZE-PROTOCOLSIZE];
  int n = b64_ntop (data, length, target, BUFFERSIZE-PROTOCOLSIZE-1);
  target[n]=0;
  sprintf(window[id%WINDOWSIZE], "%s%d%s%s%s",
  "<rdt-segment id=\"xfryzp00\">"
   "<header>"
    "<id>",id,"</id>"
   "</header>"
   "<data>",target,
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
    cur = cur->next;
  }

  /* Posune okno */
  windowpos=id;
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
 * Vytvori nove okno
 * @return error code
 */
int newWindow()
{
  for (int i = 0; i < WINDOWSIZE; i++) {
    window[i]=malloc(BUFFERSIZE*sizeof(char));
    if(window[i]==NULL){
      fprintf(stderr, "rdtclient: Could not create window\n");
      for(;i>0;i--) free(window[i-1]);
      return -1;
    }
  }
  if(atexit(freeWindow)){
    freeWindow();
    fprintf(stderr, "rdtclient: Could not create window\n");
    return -1;
  }
  return 0;
}

/**
 * Uvolni okno
 */
void freeWindow()
{
  for (int i = 0; i < WINDOWSIZE; i++)
    free(window[i]);
}


/**
 * Zpracuje argumenty prikazove radky
 * @param argc Pocet argumentu
 * @param argv Pole s argumenty
 */
int parseArgs(int argc, char **argv)
{
  char usage[] = "rdtclient -s source_port -d dest_port\n";
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

