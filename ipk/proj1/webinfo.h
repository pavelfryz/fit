/*
 * Soubor:   webinfo.h
 * Autor:    xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt:  Webovy klient
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

const int BUF_SIZE = 500;
const int MAX_REDIRECT = 10;
const int MAX_MATCHES = 4;
const int WAIT_TIME = 30000; /* milliseconds */

/* Array of regular expression */
const char *REGEX[] = {
  "^[hH][tT][tT][pP]://([^/?#]*)([^#]*)(#.*)?$",
  "^.*:([[:digit:]]+)$",
  "^Location:([[:space:]]*)(.*)$",
  "HTTP[^ ]* ([[:digit:]]{3})(.*)$",
  "^Content-Length:(.*)$",
  "^Server:(.*)$",
  "^Last-Modified:(.*)$",
  "^Content-Type:(.*)$",
};

const int URL = 0;
const int PORT = 1;
const int LOCATION = 2;
const int HTML_CODE = 3;
const int LENGTH = 4;
const int SERVER = 5;
const int MODIFIED = 6;
const int TYPE = 7;
const int REGEX_COUNT = 8;

const int HOST = 1;
const int PATH = 2;
const int CODE = 1;
const int CODE_MSG = 2;

const std::string usage("usage: webinfo [OPTION] URL\n"
                        "OPTIONS\n"
                        "  -l content length\n"
                        "  -s server\n"
                        "  -m last modified\n" "  -t content type\n");

/* Struction for url */
struct url_t {
  std::string host;
  std::string port;
  std::string path;
};

/* Struction for arguments */
struct args_t {
  std::string opts;
  std::string url;
};

/* Function declarations */
int getHead(std::string & response, url_t t);
int getArgs(args_t & args, int argc, char **argv);
int parseResponse(bool & redirect, args_t & args, std::string response,
                  regex_t regex[REGEX_COUNT]);
int parseUrl(url_t & parsedUrl, std::string url, regex_t regex[REGEX_COUNT]);
int compileRegEx(regex_t regex[REGEX_COUNT]);
void freeRexEx(regex_t regex[REGEX_COUNT]);
