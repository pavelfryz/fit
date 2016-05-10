/*
 * Soubor:   webinfo.c
 * Autor:    xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt:  Webovy klient
 * Popis:    Program ziska informace o objektu na zadane url.
 */

#include "webinfo.h"

using namespace std;

/**
 * Main program.
 */
int main(int argc, char **argv)
{
  args_t args;
  url_t url;
  regex_t regex[REGEX_COUNT];
  string response;
  int redirectCount = 0;
  bool redirect;

  /* Process arguments */
  if (getArgs(args, argc, argv))
    return 1;

  /* Prepare regular expressions */
  if (compileRegEx(regex))
    return 2;

  do {
    redirect = false;
    /* Process url */
    if (parseUrl(url, args.url, regex)) {
      freeRexEx(regex);
      return 3;
    }
    /* Get HTTP header */
    if (getHead(response, url)) {
      freeRexEx(regex);
      return 4;
    }
    /* Process HTTP header */
    if (parseResponse(redirect, args, response, regex)) {
      freeRexEx(regex);
      return 5;
    }
    if (redirect)
      redirectCount++;
  } while (redirect && redirectCount < MAX_REDIRECT);

  /* Free memory used by regular expressions */
  freeRexEx(regex);

  if (redirect == MAX_REDIRECT) {
    cerr << "error: Maximum redirect reached" << endl;
    return 6;
  }
  return 0;
}

/**
 * Get HTTP header of given url.
 * @param response returned HTTP header
 * @param t target url
 * @return error code
 */
int getHead(string & response, url_t t)
{
  int s, e, rv, flags;
  socklen_t len;
  char buf[BUF_SIZE];
  struct addrinfo hints = { };
  struct addrinfo *result, *rp;
  struct pollfd ufds;
  string msg("HEAD " + t.path + " HTTP/1.1\r\n"
             "Host: " + t.host + "\r\n"
             "Connection: close\r\n\r\n");

  /* Obtain address matching host/port */
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  if ((e = getaddrinfo(t.host.c_str(), t.port.c_str(), &hints, &result)) != 0) {
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

  /* Send request */
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
    size_t pos = response.find("\r\n\r\n");
    if (pos!=string::npos){
      response.erase(pos+2);
      break;
    }
  }
  if (rv == 0 && response.empty()) {
    cerr << "error: Timeout occurred!  No data after " << WAIT_TIME/1000 << " seconds"<<endl;
    cout << response;
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

  /* Close connection, clean up socket */
  if (close(s) == -1) {
    cerr << "error: " << strerror(errno) << endl;
    return -1;
  }
  return 0;
}

/**
 * Process the program arguments.
 * @param args structure for arguments
 * @param argc count of arguments
 * @param argv array of arguments
 * @return error code
 */
int getArgs(args_t & args, int argc, char **argv)
{
  bool l = true;
  bool s = true;
  bool m = true;
  bool t = true;
  int ch;
  if (argc > 1) {
    args.url = argv[--argc];
    while ((ch = getopt(argc, argv, "lsmt")) != -1) {
      switch (ch) {
      case 'l':
        if (l) {
          l = false;
          args.opts.push_back('l');
        }
        break;
      case 's':
        if (s) {
          s = false;
          args.opts.push_back('s');
        }
        break;
      case 'm':
        if (m) {
          m = false;
          args.opts.push_back('m');
        }
        break;
      case 't':
        if (t) {
          t = false;
          args.opts.push_back('t');
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
 * Process the HTML header.
 * @param redirect true, if html code = 301|302
 * @param args program arguments
 * @param response HTML header
 * @param regex array of regular expression
 * @return error code
 */
int parseResponse(bool & redirect, args_t & args, string response,
                  regex_t regex[REGEX_COUNT])
{
  int code;
  regmatch_t match_url[MAX_MATCHES];

  /* Remove <CR> characters */
  string::iterator end = remove(response.begin(), response.end(), '\r');
  response.erase(end, response.end());

  /* Get HTML status code */
  if (regexec(&regex[HTML_CODE], response.c_str(), MAX_MATCHES, match_url, 0) != 0) {
    cerr << "error: Could not parse response" << endl;
    return -1;
  }
  code = atoi(response.substr(match_url[CODE].rm_so,
              match_url[CODE].rm_eo - match_url[CODE].rm_so).c_str());

  /* Status code 4xx|5xx - error */
  if (code >= 400 && code < 600) {
    cerr << "Chyba: " << code << response.substr(match_url[CODE_MSG].rm_so,
                                                 match_url[CODE_MSG].rm_eo -
                                                 match_url[CODE_MSG].
                                                 rm_so) << endl;
    return -1;
  }
  /* Status code 301|302 - redirect */
  else if (code == 301 || code == 302) {
    redirect = true;
    if (regexec(&regex[LOCATION], response.c_str(), MAX_MATCHES, match_url, 0) != 0) {
      cerr << "error: Could not redirect, missing location" << endl;
      return -1;
    }
    args.url = response.substr(match_url[LOCATION].rm_so, match_url[LOCATION].rm_eo -
                               match_url[LOCATION].rm_so);
  }
  /* Status code 200 - ok */
  else if (code == 200) {
    /* No options - print header */
    if (args.opts.empty())
      cout << response;
    /* Process all options */
    else {
      for (unsigned int i = 0; i < args.opts.length(); i++) {
        int status;
        switch (args.opts[i]) {
        case 'l':
          cout << "Content-Length:";
          status =
              regexec(&regex[LENGTH], response.c_str(), MAX_MATCHES, match_url, 0);
          break;
        case 's':
          cout << "Server:";
          status =
              regexec(&regex[SERVER], response.c_str(), MAX_MATCHES, match_url, 0);
          break;
        case 'm':
          cout << "Last-Modified:";
          status =
              regexec(&regex[MODIFIED], response.c_str(), MAX_MATCHES, match_url, 0);
          break;
        case 't':
          cout << "Content-Type:";
          status =
              regexec(&regex[TYPE], response.c_str(), MAX_MATCHES, match_url, 0);
          break;
        }
        if (status == 0)        /* Value found */
          cout << response.substr(match_url[1].rm_so,
                                  match_url[1].rm_eo -
                                  match_url[1].rm_so) << endl;
        else                    /* Value missing */
          cout << " N/A" << endl;
      }
    }
  }
  /* Unknown status code */
  else {
    cerr << "error: code " << code << " not implemented" << endl;
    return -1;
  }
  return 0;
}

/**
 * Process given url and store it into structure.
 * @param parsedUrl structure for url
 * @param url given url
 * @param regex array of regular expression
 * @return error code
 */
int parseUrl(url_t & parsedUrl, string url, regex_t regex[REGEX_COUNT])
{
  regmatch_t match_url[MAX_MATCHES];
  if (regexec(&regex[URL], url.c_str(), MAX_MATCHES, match_url, 0) != 0) {
    cerr << "error: Unknown/missing protocol\nCould not parse url: " << url << endl;
    return -1;
  }

  /* Store host part */
  parsedUrl.host = url.substr(match_url[HOST].rm_so, match_url[HOST].rm_eo -
                              match_url[HOST].rm_so);

  /* Store path part */
  parsedUrl.path = url.substr(match_url[PATH].rm_so, match_url[PATH].rm_eo -
                              match_url[PATH].rm_so);

  /* Store port */
  if (regexec(&regex[PORT], parsedUrl.host.c_str(), MAX_MATCHES, match_url, 0) == 0) {
    parsedUrl.port = parsedUrl.host.substr(match_url[PORT].rm_so);
    parsedUrl.host.erase(match_url[PORT].rm_so - 1);
  }

  /* Path wasn given, set default */
  if (parsedUrl.path.empty())
    parsedUrl.path = "/";

  /* Port wasn given, set default */
  if (parsedUrl.port.empty())
    parsedUrl.port = "80";

  /* Host wasn given, error */
  if (parsedUrl.host.empty()) {
    cerr << "error: Missing host\nCould not parse url: " << url << endl;
    return -1;
  }
  return 0;
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
