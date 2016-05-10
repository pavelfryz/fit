/*
 * Soubor:   client.cpp
 * Autor:    xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt:  Klient/Server - preklad DNS
 * Popis:    Klient posila pozadavek serveru o preklad zadaneho
 *           domenoveho jmena.
 */

#include "client.h"

using namespace std;

/**
 * Main program.
 */
int main(int argc, char **argv)
{
  args_t args;
  string response;
  regex_t regex[REGEX_COUNT];

  /* Prepare regular expressions */
  if (compileRegEx(regex))
    return 1;

  /* Process arguments */
  if (getArgs(args, argc, argv, regex)) {
    freeRexEx(regex);
    return 2;
  }

  /* Get IP from server */
  if (getIP(response, args)) {
    freeRexEx(regex);
    return 3;
  }

  /* Process response from server */
  if (parseResponse(response, regex)) {
    freeRexEx(regex);
    return 4;
  }

  /* Free memory used by regular expressions */
  freeRexEx(regex);

  return 0;
}

/**
 * Connect to server and ask for ip of given domainname.
 * @param response response of server
 * @param args argument with domainname
 * @return error code
 */
int getIP(string & response, args_t args)
{
  int s, e, flags, rv;
  socklen_t len;
  char buf[BUF_SIZE];
  struct addrinfo hints = { };
  struct addrinfo *result;
  struct pollfd ufds;

  string msg("TRANSLATE " + args.domain + "\n");
  for(unsigned int i=0;i<args.type.length();i++) {
    if (args.type[i]=='4') msg.append("TYPE IPV4\n");
    else msg.append("TYPE IPV6\n");
  }
  msg.append("END\n\n");

  /* Obtain address(es) matching host/port */
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  if ((e = getaddrinfo(args.host.c_str(), args.port.c_str(), &hints, &result)) != 0) {
    cerr << "error: " << gai_strerror(e) << endl;
    return -1;
  }

  /* Create socket */
  if ((s = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
    cerr << "error: " << strerror(errno) << endl;
    freeaddrinfo(result);
    return -1;
  }
  ufds.fd = s;

  /* Set socket to be non-blocking */
  flags = fcntl(s, F_GETFL, 0);
  if(flags == -1){
    cerr << "error: " << strerror(errno) << endl;
    if (close(s) == -1)
      cerr << "error: " << strerror(errno) << endl;
    return -1;
  }
  if(fcntl(s, F_SETFL, flags | O_NONBLOCK) == -1){
    cerr << "error: " << strerror(errno) << endl;
    if (close(s) == -1)
      cerr << "error: " << strerror(errno) << endl;
    return -1;
  }

  /* Connect to address*/
  if ((connect(s, result->ai_addr, result->ai_addrlen) == -1) && (errno != EINPROGRESS)) {
    cerr << "error: " << strerror(errno) << endl;
    if (close(s) == -1)
      cerr << "error: " << strerror(errno) << endl;
    freeaddrinfo(result);
    return -1;
  }
  freeaddrinfo(result);         /* No longer needed */

  /* Wait until we connect or timeout expires*/
  ufds.events = POLLOUT;
  rv = poll(&ufds, 1, WAIT_TIME);
  if(rv <= 0){
    if(rv == 0)
      cerr << "error: Timeout occurred! Could not connect after " << WAIT_TIME/1000 << " seconds" << endl;
    else
      cerr << "error: " << strerror(errno) << endl;
    if (close(s) == -1)
      cerr << "error: " << strerror(errno) << endl;
    return -1;
  }

  /* Check if connect completed successfully */
  len = sizeof(e);
  if(getsockopt(s, SOL_SOCKET, SO_ERROR, &e, &len) == -1) {
    cerr << "error: " << strerror(errno) << endl;
    if (close(s) == -1)
      cerr << "error: " << strerror(errno) << endl;
    return -1;
  }
  if(e != 0) {
    cerr << "error: Could not connect" << endl;
    if (close(s) == -1)
      cerr << "error: " << strerror(errno) << endl;
    return -1;
  }

  /* send request */
  if ((e = send(s, msg.c_str(), msg.length(), 0)) != msg.length()) {
    if (e == -1)
      cerr << "error: " << strerror(errno) << endl;
    else
      cerr << "error: Could not send all data" << endl;
    if (close(s) == -1)
      cerr << "error: " << strerror(errno) << endl;
    return -1;
  }

   /* Read response */
  response.clear();
  ufds.events = POLLIN;
  while ((rv = poll(&ufds, 1, WAIT_TIME)) > 0 && (e = recv(s, buf, BUF_SIZE - 1, 0)) > 0) {
    buf[e] = '\0';
    response += buf;
    size_t pos = response.find("\n\n");
    if (pos!=string::npos){
      response.erase(pos+1);
      break;
    }
  }
  if (rv == 0 && response.empty()) {
    cerr << "error: Timeout occurred!  No data after " << WAIT_TIME/1000 << " seconds"<<endl;
    if (close(s) == -1)
      cerr << "error: " << strerror(errno) << endl;
    return -1;
  }
  if (e == -1 || rv == -1) {
    cerr << "error: " << strerror(errno) << endl;
    if (close(s) == -1)
      cerr << "error: " << strerror(errno) << endl;
    return -1;
  }

  /* close connection, clean up socket */
  if (close(s) == -1) {
    cerr << "error: " << strerror(errno) << endl;
    return -1;
  }
  return 0;
}

/**
 * Process the response from server.
 * @param response returned response from server
 * @param regex array of regular expressions
 * @return error code
 */
int parseResponse(string response, regex_t regex[REGEX_COUNT])
{
  regmatch_t match_url[MAX_MATCHES];
  int ret=-1;
  bool cont=true;
  int start=0;
  do{
    if (regexec(&regex[IPE], response.substr(start).c_str(), MAX_MATCHES, match_url, 0) != 0) {
      cerr << "error: Could not parse response" << endl;
      return -1;
    }

    string type=response.substr(match_url[IPERR].rm_so+start,match_url[IPERR].rm_eo-match_url[IPERR].rm_so);
    if(type=="IP "){
      ret = 0;
      cout << response.substr(match_url[IP].rm_so+start, match_url[IP].rm_eo-match_url[IP].rm_so) << endl;
    }else if(type=="ERROR "){
      cerr << response.substr(match_url[ERR].rm_so+start,match_url[ERR].rm_eo-match_url[ERR].rm_so) << endl;
    }else{
      cont = false;
    }
    start += match_url[0].rm_eo+1;
  }while(cont);

  return ret;
}

/**
 * Process the program arguments.
 * @param args structure for arguments
 * @param argc count of arguments
 * @param argv array of arguments
 * @return error code
 */
int getArgs(args_t & args, int argc, char **argv, regex_t regex[REGEX_COUNT])
{
  bool ipv4 = true;
  bool ipv6 = true;
  int ch;
  if (argc > 3) {
    args.host = argv[1];
    regmatch_t match_url[MAX_MATCHES];

    /* Store port */
    if (regexec(&regex[PORT], args.host.c_str(), MAX_MATCHES, match_url, 0) != 0) {
      cerr << "error: Missing port" << endl;
      cerr << usage;
      return -1;
    }

    args.port = args.host.substr(match_url[PORT].rm_so);
    args.host.erase(match_url[PORT].rm_so - 1);

    /* Host wasn given, error */
    if (args.host.empty()) {
      cerr << "error: Missing host" << endl;
      cerr << usage;
      return -1;
    }

    args.domain = argv[--argc];
    // mozna zkontrolovat domain;
    argc -= 1;
    argv += 1;
    while ((ch = getopt(argc, argv, "46")) != -1) {
      switch (ch) {
      case '4':
        if (ipv4) {
          ipv4 = false;
          args.type.push_back('4');
        }
        break;
      case '6':
        if (ipv6) {
          ipv6 = false;
          args.type.push_back('6');
        }
        break;
      default:
        cerr << usage;
        return -1;
      }
    }
    if (argc - optind) {
      cerr << usage;
      return -1;
    }
    return 0;
  }
  cerr << usage;
  return -1;
}


/**
 * Compile regular expressions
 * @param regex target array
 * @return error code
 */
int compileRegEx(regex_t regex[REGEX_COUNT])
{
  for (int i = 0; i < REGEX_COUNT; i++) {
    if (regcomp(&regex[i], REGEX[i], REG_EXTENDED | REG_NEWLINE) != 0) {
      for (int j = i - 1; j >= 0; j--)
        regfree(&regex[j]);
      cerr << "error: Could not compile regexp" << endl;
      return -1;
    }
  }
  return 0;
}

/**
 * Free array of regular expressions
 * @param regex array of regex
 */
void freeRexEx(regex_t regex[REGEX_COUNT])
{
  for (int i = 0; i < REGEX_COUNT; i++)
    regfree(&regex[i]);
}
