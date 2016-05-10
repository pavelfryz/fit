/*
 * Soubor:   server.h
 * Autor:    xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt:  Klient/Server - preklad DNS
 */

#include <iostream>
#include <string>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <regex.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>

const int BUF_SIZE = 300;
const int MAX_MATCHES = 3;
const int WAIT_TIME = 30000; /* in milliseconds */

/* Array of regular expression */
const char *REGEX[] = {
  "^TRANSLATE[[:space:]]*([^[:space:]]*)[[:space:]]*$",
  "^(TYPE (IPV4|IPV6)|END)$",
};

const int TR = 0;
const int DOM = 1;
const int TYPE = 1;
const int REGEX_COUNT = 2;
const int MAX_CONN = 10;

const std::string usage("usage: server -p PORT\n");

/* Function declarations */
int init(std::string &port,int argc, char**argv);
void run(int mainSocket);
int createMainSocket(int &mainSocket,std::string port);
void prepareAnswer(std::string &answer,std::string &request);
void *processClient(void *socket);
int compileRegEx();
void freeRexEx();
void handler(int signum);
