%{
/*  
 *  pdscli.y - PDS firewall client application.
 */
  
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <map>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/netfilter.h>
using namespace std;

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;
extern int line_num;
 
void yyerror(const char *s);
void add_a_rule();

struct fw_rule{
  unsigned int id;
  unsigned int src_ip;
  unsigned int dst_ip;
  unsigned short src_port;
  unsigned short dst_port;
  unsigned char proto;
  unsigned char action;
} rule;

std::map<int,fw_rule> rules;

const char *usage = 
  "./pdscli [option]\n"
  "option:\n"
  "  -a rule\n"
  "  -f file\n"
  "  -d id\n"
  "  -p\n";

%}

%union {
  unsigned int ival;
}

%token T_ALLOW T_DENY T_FROM T_TO T_SRC_PORT T_DST_PORT T_BAD
%token <ival> T_INT T_IPV4 T_ANY T_IP T_TCP T_UDP T_ICMP 

%%
rules:
  rules rule
  | rule
  ;
rule:
  T_INT action protocol T_FROM srcip T_TO dstip srcport dstport { rule.id = $1; add_a_rule();}
action:
  T_ALLOW {rule.action = NF_ACCEPT;}
  | T_DENY {rule.action = NF_DROP;}
  ;
protocol:
  T_IP {rule.proto = $1;}
  |T_TCP {rule.proto = $1;}
  |T_UDP {rule.proto = $1;}
  |T_ICMP {rule.proto = $1;}
  ;
srcip:
  T_IPV4 {rule.src_ip = $1;}
  |T_ANY {rule.src_ip = $1;}
  ;
dstip:
  T_IPV4 {rule.dst_ip = $1;}
  |T_ANY {rule.dst_ip = $1;}
  ;
srcport: {rule.src_port = 0;}
  |T_SRC_PORT T_INT {
    if(rule.proto == IPPROTO_TCP || rule.proto == IPPROTO_UDP){
      rule.src_port = $2;
    }else{
      rule.src_port = 0;
      std::cerr << "Ignoring src-port on line "<< line_num <<": protocol not tcp or udp" << std::endl;
    } 
  }
  ;
dstport: {rule.dst_port = 0;}
  |T_DST_PORT T_INT {
    if(rule.proto == IPPROTO_TCP || rule.proto == IPPROTO_UDP){
      rule.src_port = $2;
    }else{
      rule.dst_port = 0;
      std::cerr << "Ignoring dst-port on line "<< line_num <<": protocol not tcp or udp" << std::endl;
    }
  }
  ;

%%

int conversion(const char *s, unsigned int *l)
{
  unsigned i=0;
  char *ptr=NULL;
  while(isspace(s[i]))i++;
  if(!isdigit(s[i])) return 1;
  errno=0;
  *l=strtoul(s+i, &ptr, 10);
  if(errno||*ptr!='\0') return 1;
  return 0;
}

void add_a_rule(){
  rules[rule.id] = rule;
}

int print_rules(){
  std::ifstream pf;
  
  pf.open("/proc/pdsfw");
  if (pf.fail()) {        
    std::cerr << "Cannot open /proc/pdsfw for reading" << std::endl;        
    return 1;  
  }
  std::cout<<"id\taction\tsrcip\tsrcport\tdstip\tdstport\tprotocol" << std::endl;
  std::string line;
  while (std::getline(pf, line))
    std::cout << line << std::endl;
  pf.close();
  return 0;
}

int del_a_rule(unsigned int id){
  FILE *pf;
  pf = fopen("/proc/pdsfw", "w");     
  if (!pf) {
    std::cerr << "Cannot open /proc/pdsfw for writting" << std::endl;        
    return 1;     
  } 
  fprintf(pf, "d%u", id);     
  fclose(pf);     
  return 0; 
}

int parse_file(FILE *f){
  yyin = f;
  do {
    yyparse();
  } while (!feof(yyin));
  
  for (std::map<int,fw_rule>::iterator it=rules.begin(); it!=rules.end(); ++it){
    FILE *pf;
    pf = fopen("/proc/pdsfw", "w");     
    if (!pf) {
      std::cerr << "Cannot open /proc/pdsfw for writting" << std::endl;        
      return 1;     
    }
    fprintf(pf, "a%u %u %u %u %u %u %u ", 
      it->second.id,
      it->second.proto,
      it->second.src_ip,
      it->second.dst_ip,
      it->second.src_port,
      it->second.dst_port,
      it->second.action
    );
    fclose(pf);     
  }
}

int main(int argc, char* argv[]) {
  if(argc==2){ 
    if(argv[1]==std::string("-p")){
      if(print_rules()) return 1;
    }else{
      std::cerr << usage << std::endl; 
      return 1;
    }
  }else if(argc==3){
    if(argv[1]==std::string("-d")){
      unsigned int id;
      if(conversion(argv[2],&id)){
	std::cerr << "Wrong format of id" << std::endl;
	return 1;
      }
      if(del_a_rule(id)) return 1;
    }else if(argv[1]==std::string("-f")){
      FILE *infile = fopen(argv[2], "r");
      if (!infile) {
  std::cerr << "Can't open input file" << std::endl;
  return 1;
      }
      parse_file(infile);
      fclose(infile);
    }else if(argv[1]==std::string("-a")){
      FILE * infile = fmemopen(argv[2], strlen(argv[2]),"r");
      if (!infile) {
  cerr << "Can't open memory file" << endl;
  return 1;
      }
      parse_file(infile);
      fclose(infile);
    }
  }else if(argc>3){
    if(argv[1]==std::string("-a")){
      int i = 2;
      std::string s = "";
      while(i<argc) s = s + argv[i++] + " ";
      char * cstr = new char [s.length()+1];
      std::strcpy (cstr, s.c_str());
      FILE * infile = fmemopen(cstr, s.length(),"r");
      if (!infile) {
  cerr << "Can't open memory file" << endl;
  delete[] cstr;
  return 1;
      }
      parse_file(infile);
      fclose(infile);
      delete[] cstr;
    }
  }else{
    std::cerr << usage << std::endl; 
    return 1;
  }
  
  return 0;
}

void yyerror(const char *s) {
  std::cerr << "Error when parsing file on line " << line_num << std::endl;
  fclose(yyin);
  exit(-1);
} 
