%{
#include <netinet/in.h>
#include "pdscli.tab.h"
#define YY_DECL extern "C" int yylex()
int line_num = 1;
%}

DIGIT    [0-9]
DEC_OCTET 25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?

%%

{DIGIT}+ {
  yylval.ival = atoi(yytext);
  return T_INT;
}

{DEC_OCTET}"."{DEC_OCTET}"."{DEC_OCTET}"."{DEC_OCTET} {
  unsigned char byte = 0;
  unsigned int ip = 0;
  for(int i=0;i<yyleng;++i){
    if(yytext[i]=='.'){
      ip |= byte;
      ip = ip << 8;
      byte = 0;
    }else{
      byte = byte*10 + yytext[i]-'0';
    }
  }
  ip |= byte;
  yylval.ival = ip; 
  return T_IPV4;
}

allow {
  return T_ALLOW;
}

deny {
  return T_DENY;
}

tcp {
  yylval.ival = IPPROTO_TCP;
  return T_TCP;
}

udp {
  yylval.ival = IPPROTO_UDP;
  return T_UDP;
}

icmp {
  yylval.ival = IPPROTO_ICMP;
  return T_ICMP;
}

ip {
  yylval.ival = 0;
  return T_IP;
}

any {
  yylval.ival = 0;
  return T_ANY;
}

from {
  return T_FROM;
}

to {
  return T_TO;
}

src-port {
  return T_SRC_PORT;
}

dst-port {
  return T_DST_PORT;
}

[ \t\r]+ ;

\n {
  line_num++;
}

. {
  return T_BAD;
}

%%

