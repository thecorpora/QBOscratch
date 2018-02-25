#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <libwebsockets.h>

#include<pthread.h>

pthread_t tid, tid_2;

#define KGRN "\033[0;32;32m"
#define KCYN "\033[0;36m"
#define KRED "\033[0;32;31m"
#define KYEL "\033[1;33m"
#define KMAG "\033[0;35m"
#define KBLU "\033[0;32;34m"
#define KCYN_L "\033[1;36m"
#define RESET "\033[0m"

static int destroy_flag = 0;
static char c;
static struct lws *wsi_p;
static void INT_HANDLER(int signo) {
  destroy_flag = 1;
}

char * fifo_say = "/home/pi/Documents/pipes/pipe_say";
char * fifo_cmd = "/home/pi/Documents/pipes/pipe_cmd";
char * fifo_listen = "/home/pi/Documents/pipes/pipe_listen";
char * fifo_feel = "/home/pi/Documents/pipes/pipe_feel";
char * fifo_findFace = "/home/pi/Documents/pipes/pipe_findFace";


/* *
 * websocket_write_back: write the string data to the destination wsi.
 */
static int websocket_write_back(struct lws *wsi_in, char *str, int str_size_in) 
{
  if (str == NULL || wsi_in == NULL)
    return -1;
  
  int n;
  int len;
  char *out = NULL;
  
  if (str_size_in < 1) 
    len = strlen(str);
  else
    len = str_size_in;
  
  out = (char *)malloc(sizeof(char)*(LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING));
  //* setup the buffer*/
  memcpy (out + LWS_SEND_BUFFER_PRE_PADDING, str, len );
  //* write out*/
  n = lws_write(wsi_in, out + LWS_SEND_BUFFER_PRE_PADDING, len, LWS_WRITE_TEXT);
  
  printf(KBLU"[websocket_write_back] %s\n"RESET, str);
  //* free the buffer*/
  free(out);
  
  return n;
}

/* inspect PIPE LISTEN thread*/
void* inspect_PIPE_LISTEN(void *arg)
{
  char str_aux[80];
  char str[256];
  char strToSend[1024];
  char strTotal[1024];
  char strTextToSend[1024];
  char listen_buff[1024];
  int nread, fd;
  int file_closed = 0, len_aux;
  FILE *file, *fileTouch;
  
  while (1) {
    sleep(1);
    memset(listen_buff, 0, 1024);
    /* read text from the FIFO_LISTEN */
    fd = open(fifo_listen, O_RDONLY);
    nread = read(fd, listen_buff, 1024);
    printf ("From FIFO_LISTEN: %s\n", listen_buff);
    close(fd);

    if (nread) {
	sprintf(strTextToSend, "Text: %s", listen_buff);
	websocket_write_back(wsi_p ,(char *)strTextToSend, -1);
    }
  }
}

/* inspect PIPE FEEL thread*/
void* inspect_PIPE_FEEL(void *arg)
{
  char strTextToSend[1024];
  char listen_buff[1024];
  int nread, fd;
  
  while (1) {
    sleep(1);
    memset(listen_buff, 0, 1024);
    /* read text from the FIFO_LISTEN */
    printf("opening %s\n", fifo_feel);
    fd = open(fifo_feel, O_RDONLY);
    nread = read(fd, listen_buff, 1024);
    printf ("From FIFO_FEEL: %s\n", listen_buff);
    close(fd);

    if (nread) {
	sprintf(strTextToSend, "%s", listen_buff);
	websocket_write_back(wsi_p ,(char *)strTextToSend, -1);
    }
  }
}


/* inspect PIPE FIND FACE thread*/
void* inspect_PIPE_FIND_FACE(void *arg)
{
  char strTextToSend[1024];
  char listen_buff[1024];
  int nread, fd;


  while (1) {
    sleep(1);
    memset(listen_buff, 0, 1024);
    fd = open(fifo_findFace, O_RDONLY);
    //printf("opening %s\n", fifo_findFace);
    /* read text from the FIFO_FIND_FACE */
    nread = read(fd, listen_buff, 1024);
    if (nread > 0) {
	printf ("From FIFO_FIND_FACE: %s\n", listen_buff);
	sprintf(strTextToSend, "Face: %s", listen_buff);
	websocket_write_back(wsi_p ,(char *)strTextToSend, -1);
    }
    close(fd);
    // read all the pipe.
    if (nread > 0) {
	fd = open(fifo_findFace, O_RDONLY | O_NONBLOCK);
	nread = read(fd, listen_buff, 1024);
    }
    close(fd);
  }
}




static int ws_service_callback(struct lws *wsi,
			       enum lws_callback_reasons reason, void *user,
			       void *in, size_t len)
{
  char strSystem[256];
  FILE * file;
  int cmd_say;

  switch (reason) {
 
  case LWS_CALLBACK_ESTABLISHED:
    wsi_p = wsi;

    // Log
    printf(KYEL"[Main Service] Connection established\n"RESET);
    break;
    
    //* If receive a data from client*/
  case LWS_CALLBACK_RECEIVE:
    cmd_say = 0;

    printf(KCYN_L"[Main Service] Server recvived:%s\n"RESET,(char *)in);


    // detect 'say' command
    char in_copy[1024];
    strcpy(in_copy, in);
    char* token = strtok(in_copy, " ");
    while(token) {
	// printf("token: %s\n", token);
	token = strtok(NULL, " ");
	if ( (token) && (strcmp(token, "say") == 0) ) { // say command detected.
		token = strtok(NULL, " ");
		if (strcmp(token, "-t") == 0) {
			cmd_say = 1;
			token = strtok(NULL, "\"");
			int fd;
    			/* write text to the FIFO */
    			fd = open(fifo_say, O_WRONLY);
			sprintf(in_copy,"%s", token);
			sprintf (strSystem, "To FIFO_SAY: %s\n", in_copy);
    			write(fd, in_copy, strlen(in_copy));
    			close(fd);
			/* remove the FIFO */
//    			unlink(fifo_say);
		}
	}
    }
    if (!cmd_say) {
	int fd;
	/* write text to the FIFO */
	fd = open(fifo_cmd, O_WRONLY);
	sprintf (strSystem, "To FIFO_CMD: %s\n", in);
    	write(fd, in, strlen(in));
    	close(fd);
    }
//system ("../Python_projects/PiCmd.py -c nose -co red");

    //* echo back to client*/
    printf("%s\n", strSystem);
    websocket_write_back(wsi ,(char *)strSystem, -1);
    
    break;
  case LWS_CALLBACK_CLOSED:
    printf(KYEL"[Main Service] Client close.\n"RESET);
    break;
    
  default:
    break;
  }
  
  return 0;
}

struct per_session_data {
  int fd;
};

int main(void) {
  // server url will usd port 5000
  int port = 51717;
  const char *interface = NULL;
  struct lws_context_creation_info info;
  struct lws_protocols protocol;
  struct lws_context *context;
  // Not using ssl
  const char *cert_path = NULL;
  const char *key_path = NULL;
  // no special options
  int opts = 0;
  int err;

  //* register the signal SIGINT handler */
  struct sigaction act;
  act.sa_handler = INT_HANDLER;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);
  sigaction( SIGINT, &act, 0);

  //* setup websocket protocol */
  protocol.name = "my-echo-protocol";
  protocol.callback = ws_service_callback;
  protocol.per_session_data_size=sizeof(struct per_session_data);
  protocol.rx_buffer_size = 0;

  //* setup websocket context info*/
  memset(&info, 0, sizeof info);
  info.port = port;
  info.iface = interface;
  info.protocols = &protocol;
  // TODO: comprobar, se ha quitado para que no salga el warning de deprecated
  info.extensions = NULL; //lws_get_internal_extensions();
  info.ssl_cert_filepath = cert_path;
  info.ssl_private_key_filepath = key_path;
  info.gid = -1;
  info.uid = -1;
  info.options = opts;

  //* create libwebsocket context. */
  context = lws_create_context(&info);
  if (context == NULL) {
    printf(KRED"[Main] Websocket context create error.\n"RESET);
    return -1;
  }

  printf(KGRN"[Main] Websocket context create success.\n"RESET);

  /* create the FIFO_SAY (named pipe) */
  mkfifo(fifo_say, 0666);
  printf("Creating fifo_say...\n");
  /* create the FIFO_CMD (named pipe) */
  mkfifo(fifo_cmd, 0666);
  printf("Creating fifo_cmd...\n");

  // create inspect PIPE LISTEN thread
  err = pthread_create(&tid, NULL, &inspect_PIPE_LISTEN, NULL);
  if (err != 0)
    printf("\ncan't create thread :[%s]", strerror(err));
  else
  printf("\n Thread PIPE_LISTEN created successfully\n");

  // create inspect PIPE FEEL thread
  err = pthread_create(&tid_2, NULL, &inspect_PIPE_FEEL, NULL);
  if (err != 0)
    printf("\ncan't create thread :[%s]", strerror(err));
  else
  printf("\n Thread PIPE_FEEL created successfully\n");

  // create inspect PIPE FIND_FACE thread
  err = pthread_create(&tid_2, NULL, &inspect_PIPE_FIND_FACE, NULL);
  if (err != 0)
    printf("\ncan't create thread :[%s]", strerror(err));
  else
    printf("\n Thread PIPE_FIND_FACE created successfully\n");


  //* websocket service */
  while ( !destroy_flag) {
    lws_service(context, 50);
  }
  usleep(10);
  lws_context_destroy(context);

  return 0;
}
