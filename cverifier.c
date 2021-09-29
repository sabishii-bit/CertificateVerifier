#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>

int main(int argc, char **argv) {

  // Quick test for two command-line arguments
  // argv[0] is program name, argv[1] is address,
  // argv[2] is resource. Each is a C string.
  if (argc != 3) {
    printf("usage: %s address resource\n", argv[0]);
    return 0;
  }

 BIO *sbio, *out;
 int len;
 char tmpbuf[1024];
 SSL_CTX *ctx;
 SSL *ssl;

 /* XXX Seed the PRNG if needed. */

 ctx = SSL_CTX_new(TLS_client_method());

 /* XXX Set verify paths and mode here. */

 sbio = BIO_new_ssl_connect(ctx);
 BIO_get_ssl(sbio, &ssl);
 if (ssl == NULL) {
     fprintf(stderr, "Can't locate SSL pointer\n");
     ERR_print_errors_fp(stderr);
     exit(1);
 }

 /* Don't want any retries */
 SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

 /* XXX We might want to do other things with ssl here */

 /* An empty host part means the loopback address */
 BIO_set_conn_hostname(sbio, "krovetz.net:https");

 out = BIO_new_fp(stdout, BIO_NOCLOSE);
 if (BIO_do_connect(sbio) <= 0) {
     fprintf(stderr, "Error connecting to server\n");
     ERR_print_errors_fp(stderr);
     exit(1);
 }

 /* XXX Could examine ssl here to get connection info */

 BIO_puts(sbio, "GET / HTTP/1.0\n\n");
 for (;;) {
     len = BIO_read(sbio, tmpbuf, 1024);
     if (len <= 0)
         break;
     BIO_write(out, tmpbuf, len);
 }
 BIO_free_all(sbio);
 BIO_free(out);
    
  return 0;
}