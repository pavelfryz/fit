/*
 * Soubor: sip_monitor.cpp  
 * Autor: xfryzp00, xfryzp00@stud.fit.vutbr.cz
 * Projekt: Programovani sitove sluzby 
 */

#include "sip_monitor.h"

using namespace std;

/* packet capture handle */
pcap_t *handle = NULL;
/* regular expressions */
regex_t regex[REGEX_COUNT];
/* map with sessions */
map < string, Session > sessions;
/* array with matched expressions */
regmatch_t rm[MAX_MATCHES];
/* program arguments */
Arguments args;

/**
 * Main program.
 */
int main(int argc, char **argv)
{
  int err;
  char errbuf[PCAP_ERRBUF_SIZE];

  if (getArgs(argc, argv))
    return 1;
  if (compileRegEx())
    return 2;

  handle = pcap_create(args.device.c_str(), errbuf);
  if (handle == NULL) {
    cerr << "error" << errbuf;
    return 3;
  }

  pcap_set_promisc(handle, true);
  pcap_set_timeout(handle, 1000);

  if ((err = pcap_activate(handle)) != 0) {
    if (err == PCAP_WARNING_PROMISC_NOTSUP || err == PCAP_ERROR_NO_SUCH_DEVICE
        || err == PCAP_ERROR_PERM_DENIED)
      cerr << "error: " << pcap_geterr(handle) << endl;
    else
      cerr << "error: Cant activate handle" << endl;
    pcap_close(handle);
    return 4;
  }

  if (initSignalHandler()) {
    pcap_close(handle);
    return 5;
  }

  if (pcap_loop(handle, -1, parsePacket, NULL) == -1) {
    cerr << "error: " << pcap_geterr(handle) << endl;
    pcap_close(handle);
    return 6;
  }

  pcap_close(handle);
  return 0;
}

/**
 * Process packet.
 * @param user pointer to user data
 * @param h structure with packet  time  stamp  and lengths
 * @param bytes packet data
 */
void parsePacket(u_char * user, const pcap_pkthdr * h, const u_char * bytes)
{
  ether_header *ehdr = (ether_header *) bytes;
  int hdrLen = sizeof(ether_header);

  if (ntohs(ehdr->ether_type) == ETHERTYPE_IP) {
    // protocol ipv4
    ip *ihdr = (ip *) (bytes + hdrLen);
    hdrLen += ihdr->ip_hl * 4;
    if (ihdr->ip_p == IPPROTO_UDP) {
      // transport over udp
      hdrLen += sizeof(udphdr);
    }
    else if (ihdr->ip_p == IPPROTO_TCP) {
      // transport over tcp
      tcphdr *tcp = (tcphdr *) (bytes + hdrLen);
      hdrLen += tcp->th_off * 4;
    }
    else {
      // unknown transport protocol
      return;
    }
  }
  else if (ntohs(ehdr->ether_type) == ETHERTYPE_IPV6) {
    // protocol ipv6
    ip6_hdr *i6hdr = (ip6_hdr *) (bytes + hdrLen);
    hdrLen += sizeof(ip6_hdr);
    if (i6hdr->ip6_ctlun.ip6_un1.ip6_un1_nxt == IPPROTO_UDP) {
      // transport over udp
      hdrLen += sizeof(udphdr);
    }
    else if (i6hdr->ip6_ctlun.ip6_un1.ip6_un1_nxt == IPPROTO_TCP) {
      // transport over tcp
      tcphdr *tcp = (tcphdr *) (bytes + hdrLen);
      hdrLen += tcp->th_off * 4;
    }
    else {
      // unknown transport protocol
      return;
    }
  }
  else {
    // unknown internet protocol
    return;
  }

  parseData(bytes + hdrLen, h->caplen - hdrLen, h->ts);
}

/**
 * Process packet data.
 * @param data packet data
 * @param length data length
 * @param time data received time
 */
void parseData(const u_char * data, unsigned int length, const timeval & time)
{
  string msg((char *)data, length);

  if (!regexec(&regex[INVITE], msg.c_str(), 0, NULL, 0)) {
    parseInvite(msg, time);
  }
  else if (!regexec(&regex[RESPONSE], msg.c_str(), MAX_MATCHES, rm, 0)) {
    parseResponse(msg, time);
  }
  else if (!regexec(&regex[BYE], msg.c_str(), 0, NULL, 0)) {
    parseBye(msg, time);
  }
  return;
}

/**
 * Process SIP INVITE method
 * @param msg SIP message
 * @param time message received time
 */
void parseInvite(string & msg, const timeval & time)
{
  if (regexec(&regex[CALLID], msg.c_str(), MAX_MATCHES, rm, 0))
    return;
  string callid = msg.reg_substr(rm[2]);
  map < string, Session >::const_iterator it = sessions.find(callid);
  if (it != sessions.end())
    return;

  if (regexec(&regex[FROM], msg.c_str(), MAX_MATCHES, rm, 0))
    return;
  string from = msg.reg_substr(rm[2]);
  string fusr;
  if (!regexec(&regex[USR], from.c_str(), MAX_MATCHES, rm, 0))
    fusr = from.reg_substr(rm[3]);
  else if (!regexec(&regex[USR2], from.c_str(), MAX_MATCHES, rm, 0))
    fusr = from.reg_substr(rm[1]);
  else
    return;

  if (regexec(&regex[TO], msg.c_str(), MAX_MATCHES, rm, 0))
    return;
  string to = msg.reg_substr(rm[2]);
  string tusr;
  if (!regexec(&regex[USR], to.c_str(), MAX_MATCHES, rm, 0))
    tusr = to.reg_substr(rm[3]);
  else if (!regexec(&regex[USR2], to.c_str(), MAX_MATCHES, rm, 0))
    tusr = to.reg_substr(rm[1]);
  else
    return;

  bool insert = args.fromFilter.empty() && args.toFilter.empty();
  if (!args.fromFilter.empty()) {
    if (args.fromFilter.count(fusr))
      insert = true;
  }

  if (!args.toFilter.empty()) {
    if (args.toFilter.count(tusr))
      insert = true;
  }

  if (insert) {
    Session s;
    s.state = INVITED;
    s.from = from;
    s.to = to;
    sessions.insert(pair < string, Session > (callid, s));
  }
}

/**
 * Process SIP response
 * @param msg SIP message
 * @param time message received time
 */
void parseResponse(string & msg, const timeval & time)
{
  string resmsg = msg.reg_substr(rm[1]);
  int code = atoi(msg.reg_substr(rm[2]).c_str());
  if (regexec(&regex[CSEQ], msg.c_str(), 0, NULL, 0))
    return;
  if (regexec(&regex[CALLID], msg.c_str(), MAX_MATCHES, rm, 0))
    return;
  string callid = msg.reg_substr(rm[2]);
  map < string, Session >::iterator it = sessions.find(callid);
  if (it == sessions.end())
    return;
  if (code == 200) {
    if (it->second.state == INVITED) {
      it->second.state = INPROGRESS;
      it->second.start = time;
      if (args.receivedCalls)
        printSession(it->second);
    }
  }
  else if (code >= 300) {
    it->second.state = CANCELED;
    it->second.start = time;
    it->second.msg = resmsg;
    if (args.refusedCalls)
      printSession(it->second);
    sessions.erase(it);
  }
}

/**
 * Process SIP BYE method
 * @param msg SIP message
 * @param time message received time
 */
void parseBye(string & msg, const timeval & time)
{
  if (regexec(&regex[CALLID], msg.c_str(), MAX_MATCHES, rm, 0))
    return;
  string callid = msg.reg_substr(rm[2]);
  map < string, Session >::iterator it = sessions.find(callid);
  if (it == sessions.end())
    return;
  it->second.state = TERMINATED;
  it->second.end = time;
  printSession(it->second);
  sessions.erase(it);
}

/**
 * Process the program arguments.
 * @param argc count of arguments
 * @param argv array of arguments
 * @return error code
 */
int getArgs(int argc, char **argv)
{
  args.receivedCalls = true;
  args.refusedCalls = false;
  int ch;
  while ((ch = getopt(argc, argv, "i:acf:t:u:")) != -1) {
    switch (ch) {
    case 'i':
      args.device = string(optarg);
      break;
    case 'a':
      if (!args.receivedCalls) {
        cerr << "Invalid combination of options -a and -c" << endl;
        cerr << usage;
        return -1;
      }
      args.refusedCalls = true;
      break;
    case 'c':
      if (args.refusedCalls) {
        cerr << "Invalid combination of options -a and -c" << endl;
        cerr << usage;
        return -1;
      }
      args.receivedCalls = false;
      break;
    case 'f':
      args.fromFilter.insert(string(optarg));
      break;
    case 't':
      args.toFilter.insert(string(optarg));
      break;
    case 'u':
      args.toFilter.insert(string(optarg));
      args.fromFilter.insert(string(optarg));
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
  if (args.device.empty()) {
    cerr << "Missing interface to listen on" << endl;
    cerr << usage;
    return -1;
  }
  return 0;
}

/**
 * Print session information
 * @param s Session
 */
void printSession(const Session & s)
{
  cout << "Status: ";
  if (s.state == INPROGRESS) {
    cout << "Received call" << endl;
  }
  else if (s.state == TERMINATED) {
    cout << "Finished call" << endl;
  }
  else if (s.state == CANCELED) {
    cout << "Refused call" << endl;
    cout << "        " << s.msg << endl;
  }
  cout << "From: " << s.from << endl;
  cout << "To: " << s.to << endl;
  cout << "Time: " << time2Str(s.start) << endl;
  if (s.state == TERMINATED)
    cout << "Duration: " << timeDiff2Str(s.start, s.end) << endl;
  cout << endl;
}

/**
 * Return string representing date and time
 * @param t time
 * @return string representing date, time(YYYY-MM-DD hh:mm:ss)
 */
string time2Str(const timeval & t)
{
  char buf[BUF_SIZE];
  tm *nowtm = localtime(&(t.tv_sec));
  strftime(buf, BUF_SIZE, "%F %T", nowtm);
  return string(buf);
}

/**
 * Return string representing elapsed time
 * @param start start time
 * @param end end time
 * @return string representing elapsed time(hh:mm:ss)
 */
string timeDiff2Str(const timeval & start, const timeval & end)
{
  unsigned long diff = end.tv_sec - start.tv_sec;

  unsigned int h = diff / 3600;
  unsigned int m = (diff % 3600) / 60;
  unsigned int s = diff % 60;
  char buf[BUF_SIZE];
  snprintf(buf, BUF_SIZE, "%02d:%02d:%02d", h, m, s);
  return string(buf);
}

/**
 * Compile regular expressions
 * @param regex target array
 * @return error code
 */
int compileRegEx()
{
  for (int i = 0; i < REGEX_COUNT; i++) {
    if (regcomp(&regex[i], REGEX[i], REG_EXTENDED | REG_NEWLINE | REG_ICASE) !=
        0) {
      for (int j = i - 1; j >= 0; j--)
        regfree(&regex[j]);
      cerr << "error: Could not compile regexp" << endl;
      return -1;
    }
  }
  if (atexit(freeRegEx)) {
    freeRegEx();
    cerr << "error: Could not set function at exit" << endl;
    return -1;
  }
  return 0;
}

/**
 * Free array of regular expressions
 * @param regex array of regex
 */
void freeRegEx()
{
  for (int i = 0; i < REGEX_COUNT; i++)
    regfree(&regex[i]);
}

/**
 * Handle signal
 * @param signum received signal
 */
void handleSignal(int signum)
{
  pcap_breakloop(handle);
}

/**
 * Set signal handler
 * @return error code
 */
int initSignalHandler()
{
  struct sigaction sa = { };
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = handleSignal;

  if (sigaction(SIGHUP, &sa, NULL) || sigaction(SIGINT, &sa, NULL) ||
      sigaction(SIGQUIT, &sa, NULL) || sigaction(SIGABRT, &sa, NULL) ||
      sigaction(SIGTERM, &sa, NULL)) {
    cerr << "error: " << strerror(errno) << endl;
    return -1;
  }
  return 0;
}
