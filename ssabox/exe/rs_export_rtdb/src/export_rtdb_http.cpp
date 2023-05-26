#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "pwr.h"
#include "co_dcli.h"
#include "export_rtdb_http.h"

extern int exp_debug;
static void sigpipe_handle(int x) {}

long http_request(const char* hostname, const char* path, std::string body) {
  signal(SIGPIPE, sigpipe_handle);

  SSLeay_add_ssl_algorithms();
  const SSL_METHOD* client_method = SSLv23_client_method();
  SSL_load_error_strings();
  SSL_CTX* ctx = SSL_CTX_new(client_method);

  if (exp_debug)
    printf("(1) SSL context initialized\n");

  struct hostent* host_entry = gethostbyname(hostname);
  struct in_addr ip;
  bcopy(host_entry->h_addr, &(ip.s_addr), host_entry->h_length);
  
  if (exp_debug)
    printf("(s) '%s' has IP address '%s'\n", hostname, inet_ntoa(ip));

  int sd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in sock_addr;
  memset(&sock_addr, '\0', sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(443);
  memcpy(&(sock_addr.sin_addr.s_addr), host_entry->h_addr, host_entry->h_length);
  int err = connect(sd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
  if (err < 0) { perror("can't connect to server port"); exit(1); }
  if (exp_debug)
    printf("(3) TCP connection open to host '%s', port %d\n", hostname, sock_addr.sin_port);

  SSL* ssl = SSL_new(ctx);
  SSL_set_fd(ssl, sd);
  err = SSL_connect(ssl);
  if (exp_debug) {
    printf("(4) SSL endpoint created & handshake completed\n");
    printf("(5) SSL connected with cipher: %s\n", SSL_get_cipher(ssl));
  }

  X509* server_cert = SSL_get_peer_certificate(ssl);
  if (exp_debug) {
    printf("(6) server's certificate was received:\n");
    char* str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
    printf("  subject: %s\n", str);
    str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
    printf("  subject: %s\n", str);
  }
  X509_free(server_cert);

  char host_header[512];
  sprintf(host_header, "Host: %s\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\nContent-Type: application/vnd.schemaregistry.v1+json\r\nContent-Length: %d\r\nExpect: 100-continue\r\n\r\n", hostname, (int)body.size());
  std::string outbuf = std::string("POST ") + path + " HTTP/1.1\r\n" + host_header + body + "\r\n";

#if 0
  // Test
  pwr_tFileName fname;
  dcli_translate_filename(fname, "$pwrp_log/http_schema.txt");
  FILE* fp = fopen(fname, "w");
  fprintf(fp, "%s", outbuf.c_str());
  fclose(fp);
  exit(0);
  // End test
#endif

  err = SSL_write(ssl, outbuf.c_str(), outbuf.size());

  // shutdown(sd, 1);
  if (exp_debug)
    printf("(7) sent HTTP request with blength %d over encrypted channel:\n\n", (int)body.size());

  char inbuf[4096];
  uint32_t schema_id = 0;
  int http_code;

#if 0
  int cnt = 0;
  while (SSL_peek(ssl, inbuf, sizeof(inbuf) - 1) == 0) {
    cnt++;
    if (exp_debug)
      printf("Wait for schema id reply %d...\n", cnt);
    sleep(1);
    if (cnt > 15) {
      if (exp_debug)
	printf("No reply received\n");
      return 0;
    }
  }
#endif
  err = SSL_read(ssl, inbuf, sizeof(inbuf) - 1);
  if (err < 0)
    err = 0;
  inbuf[err] = '\0';
  http_code = atoi(inbuf + 9);
  char* res_body = strstr(inbuf, "{\"id\":");
  schema_id = 0;
  if (res_body != NULL) {
    schema_id = atoi(res_body+6);
  }
  if (exp_debug)
    printf("(8) got back HTTP response %d %d: \"%s\"\n", err, http_code, inbuf);

  if (exp_debug)
    printf("SSL state: %s\n", SSL_state_string_long(ssl));

  if (res_body == NULL) {
    if (exp_debug)
      printf("No id, read again\n");
    err = SSL_read(ssl, inbuf, sizeof(inbuf) - 1);
    if (err < 0)
      err = 0;
    inbuf[err] = '\0';
    
    schema_id = 0;
    res_body = strstr(inbuf, "{\"id\":");
    if (res_body != NULL) {
      schema_id = atoi(res_body+6);
    }
    if (exp_debug)
      printf("(8) got back HTTP response %d %d: \"%s\"\n", err, http_code, inbuf);
  }

  //SSL_shutdown(ssl);
  close(sd);
  SSL_free(ssl);
  SSL_CTX_free(ctx);
  if (exp_debug)
    printf("(9) all done, cleaned up and closed connection\n");

  if (schema_id > 0) {
    return schema_id;
  }
  return -http_code;
}
