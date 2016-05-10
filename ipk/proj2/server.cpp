/*
 * Soubor:   server.cpp
 * Autor:    xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt:  Klient/Server - preklad DNS
 * Popis:    Server nasloucha na zadanem portu, a odpovida na pozadavky
 *           klientu o preklad domenovych jmen.
 */

#include "server.h"

using namespace std;

regex_t regex[REGEX_COUNT];
bool cont = true;

/**
 * Main program.
 */
int main(int argc, char **argv)
{
  int mainSocket;
  string port;

  /* Process arguments and setup signal handling*/
  if (init(port,argc,argv))
    return 1;

  /* Prepare regular expressions */
  if (compileRegEx())
    return 2;

  /* Create main socket */
  if (createMainSocket(mainSocket,port))
    return 3;

  /* Accept connections*/
  run(mainSocket);

  /* close connection, clean up socket */
  if (close(mainSocket) == -1) {
    cerr << "error on close: " << strerror(errno) << endl;
  }
  pthread_exit(NULL);
}

/**
 * Accept connections
 * @param mainSocket listening socket
 */
void run(int mainSocket){
  int s,rc;
  pthread_t t;
  while(cont){
    /* Accept connections */
    s = accept(mainSocket,NULL,NULL);
    if(s == -1){
      if(errno == EINTR)
        continue;
      cerr << "error on accept: " << strerror(errno) << endl;
      continue;
    }

    /* Create thread for handling client */
    rc = pthread_create(&t, NULL, processClient, (void *)&s);
    if (rc){
      cerr << "error on pthread_create: " << strerror(rc) << endl;
      continue;
    }
    rc = pthread_detach(t);
    if (rc){
      cerr << "error on pthread_detach: " << strerror(rc) << endl;
    }
  }
}

/**
 * Process one client.
 * @param socket client socket
 */
void *processClient(void *socket){
  int flags,e,rv;
  int s=*(int *)socket;
  char buf[BUF_SIZE];
  struct pollfd ufds;
  string request;
  string answer;

  /* Set socket to be non-blocking */
  flags = fcntl(s, F_GETFL, 0);
  if(flags == -1){
    cerr << "error on fcntl: " << strerror(errno) << endl;
    if (close(s) == -1)
      cerr << "error on close: " << strerror(errno) << endl;
    return NULL;
  }
  if(fcntl(s, F_SETFL, flags | O_NONBLOCK) == -1){
    cerr << "error on fcntl: " << strerror(errno) << endl;
    if (close(s) == -1)
      cerr << "error on close: " << strerror(errno) << endl;
    return NULL;
  }

   /* Read request */
  request.clear();
  ufds.fd = s;
  ufds.events = POLLIN;
  while ((rv = poll(&ufds, 1, WAIT_TIME)) > 0 && (e = recv(s, buf, BUF_SIZE - 1, 0)) > 0) {
    buf[e] = '\0';
    request += buf;
    size_t pos = request.find("\n\n");
    if (pos!=string::npos){
      request.erase(pos+1);
      break;
    }
  }
  if (rv == 0 && request.empty()) {
    cerr << "error: Timeout occurred!  No data after " << WAIT_TIME/1000 << " seconds"<<endl;
    if (close(s) == -1)
      cerr << "error on close: " << strerror(errno) << endl;
    return NULL;
  }
  if (e == -1 || rv == -1) {
    cerr << "error: " << strerror(errno) << endl;
    if (errno != EBADF && close(s) == -1)
      cerr << "error on close: " << strerror(errno) << endl;
    return NULL;
  }

  prepareAnswer(answer,request);

  /* Send answer */
  ufds.events = POLLOUT;
  rv = poll(&ufds, 1, WAIT_TIME);
  if(rv <= 0){
    if(rv == 0)
      cerr << "error: Timeout occurred! Could not send data after " << WAIT_TIME/1000 << " seconds" << endl;
    else
      cerr << "error on poll: " << strerror(errno) << endl;
    if (close(s) == -1)
      cerr << "error on close: " << strerror(errno) << endl;
    return NULL;
  }
  if ((e = send(s, answer.c_str(), answer.length(), 0)) != answer.length()) {
    if (e == -1)
      cerr << "error on send: " << strerror(errno) << endl;
    else
      cerr << "error on send: Could not send all data" << endl;
    if (close(s) == -1)
      cerr << "error on close: " << strerror(errno) << endl;
    return NULL;
  }

  /* close connection, clean up socket */
  if (close(s) == -1){
    cerr << "error on close: " << strerror(errno) << endl;
    return NULL;
  }

  return NULL;
}

/**
 * Prepare answer to request.
 * @param answer prepared answer
 * @param request request
 */
void prepareAnswer(string &answer,string &request){
  regmatch_t match_url[MAX_MATCHES];
  int start=0,e;
  bool contin = true;
  string domain;
  struct addrinfo hints = { };
  struct addrinfo *result;

  /* Get domainname from request */
  if (regexec(&regex[TR], request.c_str(), MAX_MATCHES, match_url, 0) != 0) {
    answer = "ERROR Bad request\nEND\n\n";
    return;
  }
  domain = request.substr(match_url[DOM].rm_so,match_url[DOM].rm_eo-match_url[DOM].rm_so);
  start += match_url[0].rm_eo+1;

  do{
    if (regexec(&regex[TYPE], request.substr(start).c_str(), MAX_MATCHES, match_url, 0) != 0) {
      answer = "ERROR Bad request\nEND\n\n";
      return;
    }

    string type=request.substr(match_url[TYPE].rm_so+start,match_url[TYPE].rm_eo-match_url[TYPE].rm_so);
    if(type=="TYPE IPV4"){
      hints.ai_family = AF_INET;
      if ((e = getaddrinfo(domain.c_str(), NULL, &hints, &result)) != 0) {
        answer.append("ERROR Err4: Nenalezeno.\n");
      }else{
        char hostname[BUF_SIZE] = "";
        if(getnameinfo(result->ai_addr, result->ai_addrlen, hostname, BUF_SIZE, NULL, 0, NI_NUMERICHOST) == -1){
          answer.append("ERROR Err4: Nenalezeno.\n");
          freeaddrinfo(result);
        }else{
          freeaddrinfo(result);
          answer.append("IP ");
          answer.append(hostname);
          answer.append("\n");
        }
      }
    }else if(type=="TYPE IPV6"){
      hints.ai_family = AF_INET6;
      if ((e = getaddrinfo(domain.c_str(), NULL, &hints, &result)) != 0) {
        answer.append("ERROR Err6: Nenalezeno.\n");
      }else{
        char hostname[BUF_SIZE] = "";
        if((e = getnameinfo(result->ai_addr, result->ai_addrlen, hostname, BUF_SIZE, NULL, 0, NI_NUMERICHOST)) == -1){
          answer.append("ERROR Err6: Nenalezeno.\n");
          freeaddrinfo(result);
        }else{
          freeaddrinfo(result);
          answer.append("IP ");
          answer.append(hostname);
          answer.append("\n");
        }
      }
    }else{
      contin = false;
    }
    start += match_url[0].rm_eo+1;
  }while(contin);
  answer.append("END\n\n");
}

/**
 * Process the program arguments and setup signal handler.
 * @param port target port
 * @param argc count of arguments
 * @param argv array of arguments
 * @return error code
 */
int init(string &port,int argc, char**argv){
  int ch;
  bool p = true;
  while ((ch = getopt(argc, argv, "p:")) != -1) {
    switch (ch) {
    case 'p':
      if(p){
        p=false;
        port=optarg;
      }
      break;
    default:
      cerr << usage;
      return -1;
    }
  }
  if(argc - optind||port.empty()){
    cerr << usage;
    return -1;
  }

  struct sigaction sa = { };
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = handler;

  if (sigaction(SIGHUP, &sa, NULL) || sigaction(SIGINT, &sa, NULL) ||
      sigaction(SIGQUIT, &sa, NULL) || sigaction(SIGABRT, &sa, NULL) ||
      sigaction(SIGTERM, &sa, NULL)) {
    cerr << "Error on sigaction: "<< strerror(errno) << endl;
    return -1;
  }
  return 0;
}

/**
 * Create main socket, which can accept connections
 * and will listen on given port
 * @param mainSocket created socket
 * @param port target port
 * @return error code
 */
int createMainSocket(int &mainSocket,string port){
  struct addrinfo hints = { };
  struct addrinfo *result;
  int s,e;

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  if ((e = getaddrinfo(NULL, port.c_str(), &hints, &result)) != 0) {
    cerr << "error on getaddrinfo: " << gai_strerror(e) << endl;
    return -1;
  }

   /* Create socket */
  if ((s = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
    cerr << "error on socket: " << strerror(errno) << endl;
    freeaddrinfo(result);
    return -1;
  }

  /* Bind a name to socket*/
  if (bind(s, result->ai_addr, result->ai_addrlen) == -1) {
    cerr << "error on bind: " << strerror(errno) << endl;
    freeaddrinfo(result);
    if(close(s) == -1){
      cerr << "error on close: " << strerror(errno) << endl;
    }
    return -1;
  }
  freeaddrinfo(result);           /* No longer needed */

  /* Mark socket as passive */
  if (listen(s, MAX_CONN) == -1) {
    cerr << "error on listen: " << strerror(errno) << endl;
    if(close(s) == -1){
      cerr << "error on close: " << strerror(errno) << endl;
    }
    return -1;
  }

  mainSocket = s;
  return 0;
}

/**
 * Handle received signal
 * @param signum signal number
 */
void handler(int signum){
  cont = false;
}

/**
 * Compile regular expressions
 * @param regex target array
 * @return error code
 */
int compileRegEx()
{
  for (int i = 0; i < REGEX_COUNT; i++) {
    if (regcomp(&regex[i], REGEX[i], REG_EXTENDED | REG_NEWLINE) != 0) {
      for (int j = i - 1; j >= 0; j--)
        regfree(&regex[j]);
      cerr << "error: Could not compile regexp" << endl;
      return -1;
    }
  }
  if(atexit(freeRexEx)){
    freeRexEx();
    cerr << "error: Could not set function at exit" << endl;
    return -1;
  }
  return 0;
}

/**
 * Free array of regular expressions
 * @param regex array of regex
 */
void freeRexEx()
{
  for (int i = 0; i < REGEX_COUNT; i++)
    regfree(&regex[i]);
}

