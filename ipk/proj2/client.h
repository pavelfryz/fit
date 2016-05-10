/*
 * Soubor:   client.h
 * Autor:    xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt:  Klient/Server - preklad DNS
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <regex.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>

const int BUF_SIZE = 100;
const int MAX_MATCHES = 3;
const int WAIT_TIME = 30000; /*in milliseconds*/

/* Array of regular expression */
const char *REGEX[] = {
  "^(IP |ERROR |END)(.*)$",
  "^.*:([[:digit:]]+)$",
};

const int IPE = 0;
const int PORT = 1;
const int REGEX_COUNT = 2;

const int IPERR = 1;
const int IP = 2;
const int ERR = 2;

const std::string usage("usage: client HOST:PORT [OPTION] DOMAINNAME\n"
                        "OPTIONS\n"
                        "  -4 IPv4 adress\n"
                        "  -6 IPv6 adress\n");

/* Struction for arguments */
struct args_t {
  std::string host;
  std::string port;
  std::string domain;
  std::string type;
};

/* Function declarations */
int getArgs(args_t & args, int argc, char **argv, regex_t regex[REGEX_COUNT]);
int getIP(std::string & response, args_t args);
int parseResponse(std::string response, regex_t regex[REGEX_COUNT]);
int compileRegEx(regex_t regex[REGEX_COUNT]);
void freeRexEx(regex_t regex[REGEX_COUNT]);
