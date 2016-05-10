/*
 * Soubor: sip_monitor.h  
 * Autor: xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt: Programovani sitove sluzby 
 */

#include <map>
#include <set>
#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cctype>

#include <signal.h>
#include <regex.h>

#include <sys/types.h>

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include <pcap/pcap.h>

#define reg_substr(match) substr((match).rm_so,(match).rm_eo - (match).rm_so)
#define BUF_SIZE 64
#define MAX_MATCHES 10

enum RegexType {
  RESPONSE,
  INVITE,
  BYE,
  CALLID,
  CSEQ,
  TO,
  FROM,
  USR,
  USR2,
  REGEX_COUNT,
};

const char *REGEX[] = {
  "^SIP/[^ ]* (([[:digit:]]{3}) (.*))\r$",
  "^INVITE [^ ]* SIP/.*\r$",
  "^BYE [^ ]* SIP/.*\r$",
  "^(Call-ID|i):[ ]*([^ ]*)(.*)\r$",
  "^CSeq:[^I]*INVITE\r$",
  "^(To|t):[ ]*([^;]*)(;.*)?\r$",
  "^(From|f):[ ]*([^;]*)(;.*)?\r$",
  "([^<]*)(<sip:([^>]*)>)",
  "sip:([^ ]*)",
};

const char *usage =
    "usage: sip_monitor -i <interface> [-a|-c] [-f <id>] [-t <id>] [-u <id>]\n"
    "OPTIONS\n"
    "  -i specify <interface> to listen on\n"
    "  -a prints information about missed calls\n"
    "  -c prints only completed calls\n"
    "  -f prints calls from <id>\n"
    "  -t prints calls to <id>\n"
    "  -u prints calls to or from <id>\n"
    "EXAMPLE\n"
    "  sip_monitor -i em0 -c -t bob@example.com\n"
    "    Listen on interface em0 and print information about\n"
    "    completed calls to user bob@example.com\n";

enum SessionState {
  INVITED,
  INPROGRESS,
  TERMINATED,
  CANCELED,
};

/* structure for sessions */
struct Session {
  SessionState state;
   std::string msg;
  timeval start;
  timeval end;
   std::string from;
   std::string to;
};

/* structure for program arguments */
struct Arguments {
  std::string device;
  bool receivedCalls;
  bool refusedCalls;
  std::set < std::string > toFilter;
  std::set < std::string > fromFilter;
};

/* function declarations */
void parsePacket(u_char * user, const pcap_pkthdr * h, const u_char * bytes);
void parseData(const u_char * data, unsigned int length, const timeval & time);
void parseInvite(std::string & msg, const timeval & time);
void parseResponse(std::string & msg, const timeval & time);
void parseBye(std::string & msg, const timeval & time);
int getArgs(int argc, char **argv);
void printSession(const Session & s);
std::string time2Str(const timeval & t);
std::string timeDiff2Str(const timeval & start, const timeval & end);
int compileRegEx();
void freeRegEx();
void handleSignal(int signum);
int initSignalHandler();
