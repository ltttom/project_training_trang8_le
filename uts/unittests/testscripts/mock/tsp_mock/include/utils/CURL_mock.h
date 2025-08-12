#include "gtest/gtest.h"
#include "gmock/gmock.h"
#ifndef CURLINC_CURL_H
#define CURLINC_CURL_H
typedef enum {
  CURLE_OK = 0,
  CURLE_UNSUPPORTED_PROTOCOL,    /* 1 */
  CURLE_FAILED_INIT,             /* 2 */
  CURLE_URL_MALFORMAT,           /* 3 */
  CURLE_NOT_BUILT_IN,            /* 4 - [was obsoleted in August 2007 for
                                    7.17.0, reused in April 2011 for 7.21.5] */
  CURLE_COULDNT_RESOLVE_PROXY,   /* 5 */
  CURLE_COULDNT_RESOLVE_HOST,    /* 6 */
  CURLE_COULDNT_CONNECT,         /* 7 */
  CURLE_WEIRD_SERVER_REPLY,      /* 8 */
  CURLE_REMOTE_ACCESS_DENIED,    /* 9 a service was denied by the server
                                    due to lack of access - when login fails
                                    this is not returned. */
  CURLE_FTP_ACCEPT_FAILED,       /* 10 - [was obsoleted in April 2006 for
                                    7.15.4, reused in Dec 2011 for 7.24.0]*/
  CURLE_FTP_WEIRD_PASS_REPLY,    /* 11 */
  CURLE_FTP_ACCEPT_TIMEOUT,      /* 12 - timeout occurred accepting server
                                    [was obsoleted in August 2007 for 7.17.0,
                                    reused in Dec 2011 for 7.24.0]*/
  CURLE_FTP_WEIRD_PASV_REPLY,    /* 13 */
  CURLE_FTP_WEIRD_227_FORMAT,    /* 14 */
  CURLE_FTP_CANT_GET_HOST,       /* 15 */
  CURLE_HTTP2,                   /* 16 - A problem in the http2 framing layer.
                                    [was obsoleted in August 2007 for 7.17.0,
                                    reused in July 2014 for 7.38.0] */
  CURLE_FTP_COULDNT_SET_TYPE,    /* 17 */
  CURLE_PARTIAL_FILE,            /* 18 */
  CURLE_FTP_COULDNT_RETR_FILE,   /* 19 */
  CURLE_OBSOLETE20,              /* 20 - NOT USED */
  CURLE_QUOTE_ERROR,             /* 21 - quote command failure */
  CURLE_HTTP_RETURNED_ERROR,     /* 22 */
  CURLE_WRITE_ERROR,             /* 23 */
  CURLE_OBSOLETE24,              /* 24 - NOT USED */
  CURLE_UPLOAD_FAILED,           /* 25 - failed upload "command" */
  CURLE_READ_ERROR,              /* 26 - couldn't open/read from file */
  CURLE_OUT_OF_MEMORY,           /* 27 */
  CURLE_OPERATION_TIMEDOUT,      /* 28 - the timeout time was reached */
  CURLE_OBSOLETE29,              /* 29 - NOT USED */
  CURLE_FTP_PORT_FAILED,         /* 30 - FTP PORT operation failed */
  CURLE_FTP_COULDNT_USE_REST,    /* 31 - the REST command failed */
  CURLE_OBSOLETE32,              /* 32 - NOT USED */
  CURLE_RANGE_ERROR,             /* 33 - RANGE "command" didn't work */
  CURLE_HTTP_POST_ERROR,         /* 34 */
  CURLE_SSL_CONNECT_ERROR,       /* 35 - wrong when connecting with SSL */
  CURLE_BAD_DOWNLOAD_RESUME,     /* 36 - couldn't resume download */
  CURLE_FILE_COULDNT_READ_FILE,  /* 37 */
  CURLE_LDAP_CANNOT_BIND,        /* 38 */
  CURLE_LDAP_SEARCH_FAILED,      /* 39 */
  CURLE_OBSOLETE40,              /* 40 - NOT USED */
  CURLE_FUNCTION_NOT_FOUND,      /* 41 - NOT USED starting with 7.53.0 */
  CURLE_ABORTED_BY_CALLBACK,     /* 42 */
  CURLE_BAD_FUNCTION_ARGUMENT,   /* 43 */
  CURLE_OBSOLETE44,              /* 44 - NOT USED */
  CURLE_INTERFACE_FAILED,        /* 45 - CURLOPT_INTERFACE failed */
  CURLE_OBSOLETE46,              /* 46 - NOT USED */
  CURLE_TOO_MANY_REDIRECTS,      /* 47 - catch endless re-direct loops */
  CURLE_UNKNOWN_OPTION,          /* 48 - User specified an unknown option */
  CURLE_SETOPT_OPTION_SYNTAX,    /* 49 - Malformed setopt option */
  CURLE_OBSOLETE50,              /* 50 - NOT USED */
  CURLE_OBSOLETE51,              /* 51 - NOT USED */
  CURLE_GOT_NOTHING,             /* 52 - when this is a specific error */
  CURLE_SSL_ENGINE_NOTFOUND,     /* 53 - SSL crypto engine not found */
  CURLE_SSL_ENGINE_SETFAILED,    /* 54 - can not set SSL crypto engine as
                                    default */
  CURLE_SEND_ERROR,              /* 55 - failed sending network data */
  CURLE_RECV_ERROR,              /* 56 - failure in receiving network data */
  CURLE_OBSOLETE57,              /* 57 - NOT IN USE */
  CURLE_SSL_CERTPROBLEM,         /* 58 - problem with the local certificate */
  CURLE_SSL_CIPHER,              /* 59 - couldn't use specified cipher */
  CURLE_PEER_FAILED_VERIFICATION, /* 60 - peer's certificate or fingerprint
                                     wasn't verified fine */
  CURLE_BAD_CONTENT_ENCODING,    /* 61 - Unrecognized/bad encoding */
  CURLE_OBSOLETE62,              /* 62 - NOT IN USE since 7.82.0 */
  CURLE_FILESIZE_EXCEEDED,       /* 63 - Maximum file size exceeded */
  CURLE_USE_SSL_FAILED,          /* 64 - Requested FTP SSL level failed */
  CURLE_SEND_FAIL_REWIND,        /* 65 - Sending the data requires a rewind
                                    that failed */
  CURLE_SSL_ENGINE_INITFAILED,   /* 66 - failed to initialise ENGINE */
  CURLE_LOGIN_DENIED,            /* 67 - user, password or similar was not
                                    accepted and we failed to login */
  CURLE_TFTP_NOTFOUND,           /* 68 - file not found on server */
  CURLE_TFTP_PERM,               /* 69 - permission problem on server */
  CURLE_REMOTE_DISK_FULL,        /* 70 - out of disk space on server */
  CURLE_TFTP_ILLEGAL,            /* 71 - Illegal TFTP operation */
  CURLE_TFTP_UNKNOWNID,          /* 72 - Unknown transfer ID */
  CURLE_REMOTE_FILE_EXISTS,      /* 73 - File already exists */
  CURLE_TFTP_NOSUCHUSER,         /* 74 - No such user */
  CURLE_OBSOLETE75,              /* 75 - NOT IN USE since 7.82.0 */
  CURLE_OBSOLETE76,              /* 76 - NOT IN USE since 7.82.0 */
  CURLE_SSL_CACERT_BADFILE,      /* 77 - could not load CACERT file, missing
                                    or wrong format */
  CURLE_REMOTE_FILE_NOT_FOUND,   /* 78 - remote file not found */
  CURLE_SSH,                     /* 79 - error from the SSH layer, somewhat
                                    generic so the error message will be of
                                    interest when this has happened */

  CURLE_SSL_SHUTDOWN_FAILED,     /* 80 - Failed to shut down the SSL
                                    connection */
  CURLE_AGAIN,                   /* 81 - socket is not ready for send/recv,
                                    wait till it's ready and try again (Added
                                    in 7.18.2) */
  CURLE_SSL_CRL_BADFILE,         /* 82 - could not load CRL file, missing or
                                    wrong format (Added in 7.19.0) */
  CURLE_SSL_ISSUER_ERROR,        /* 83 - Issuer check failed.  (Added in
                                    7.19.0) */
  CURLE_FTP_PRET_FAILED,         /* 84 - a PRET command failed */
  CURLE_RTSP_CSEQ_ERROR,         /* 85 - mismatch of RTSP CSeq numbers */
  CURLE_RTSP_SESSION_ERROR,      /* 86 - mismatch of RTSP Session Ids */
  CURLE_FTP_BAD_FILE_LIST,       /* 87 - unable to parse FTP file list */
  CURLE_CHUNK_FAILED,            /* 88 - chunk callback reported error */
  CURLE_NO_CONNECTION_AVAILABLE, /* 89 - No connection available, the
                                    session will be queued */
  CURLE_SSL_PINNEDPUBKEYNOTMATCH, /* 90 - specified pinned public key did not
                                     match */
  CURLE_SSL_INVALIDCERTSTATUS,   /* 91 - invalid certificate status */
  CURLE_HTTP2_STREAM,            /* 92 - stream error in HTTP/2 framing layer
                                    */
  CURLE_RECURSIVE_API_CALL,      /* 93 - an api function was called from
                                    inside a callback */
  CURLE_AUTH_ERROR,              /* 94 - an authentication function returned an
                                    error */
  CURLE_HTTP3,                   /* 95 - An HTTP/3 layer problem */
  CURLE_QUIC_CONNECT_ERROR,      /* 96 - QUIC connection error */
  CURLE_PROXY,                   /* 97 - proxy handshake error */
  CURLE_SSL_CLIENTCERT,          /* 98 - client-side certificate required */
  CURLE_UNRECOVERABLE_POLL,      /* 99 - poll/select returned fatal error */
  CURLE_TOO_LARGE,               /* 100 - a value/data met its maximum */
  CURL_LAST /* never use! */
} CURLcode;

typedef enum {
    CURLINFO_RESPONSE_CODE
} CURLINFO;

typedef enum{
    CURLOPT_URL,
    CURLOPT_CAPATH,
    CURLOPT_SSL_SESSIONID_CACHE,
    CURLOPT_SSL_VERIFYPEER,
    CURLOPT_SSL_VERIFYHOST,
    CURLOPT_TIMEOUT,
    CURLOPT_VERBOSE,
    CURLOPT_HTTPHEADER,
    CURLOPT_POSTFIELDS,
    CURLOPT_CUSTOMREQUEST,
    CURLOPT_HEADERFUNCTION,
    CURLOPT_HEADERDATA,
    CURLOPT_WRITEFUNCTION,
    CURLOPT_WRITEDATA
} CURLoption;

#include "tsp_mock/header_mock/HTTPSManagerAdapter_Base.h"

struct curl_slist {
  char *data;
  struct curl_slist *next;
};

curl_slist *curl_slist_append(struct curl_slist *list, const char *data) {
    curl_slist* new_node = (curl_slist*)malloc(sizeof(curl_slist));
    new_node->data = strdup(data);
    new_node->next = NULL;
    if(list == NULL) return new_node;

    curl_slist* current = list;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
    return list;
}

void curl_slist_free_all(struct curl_slist *list) { }

#define CURL_EXTERN
#define CURL void

#ifndef CURLINC_EASY_H
#define CURLINC_EASY_H

/* Flag bits in the curl_blob struct: */
#define CURL_BLOB_COPY   1 /* tell libcurl to copy the data */
#define CURL_BLOB_NOCOPY 0 /* tell libcurl to NOT copy the data */

class MockCURLcode{
public:
  MOCK_METHOD0(curl_easy_init, CURL*(void));
  MOCK_METHOD1(curl_easy_perform, CURLcode(CURL *curl));
  MOCK_METHOD1(curl_easy_cleanup, void(CURL *curl));
  MOCK_METHOD1(curl_easy_duphandle, CURL*(CURL *curl));
  MOCK_METHOD1(curl_easy_reset, void(CURL *curl));
  MOCK_METHOD4(curl_easy_recv, CURLcode(CURL *curl, void *buffer, size_t buflen, size_t *n));
  MOCK_METHOD4(curl_easy_send, CURLcode(CURL *curl, const void *buffer, size_t buflen, size_t *n));
  MOCK_METHOD1(curl_easy_upkeep, CURLcode(CURL *curl));

  MOCK_METHOD3(curl_easy_setopt, CURLcode(CURL *curl, CURLoption option, curl_slist* data));
  MOCK_METHOD3(curl_easy_setopt, CURLcode(CURL *curl, CURLoption option, const char* data));
  MOCK_METHOD3(curl_easy_setopt, CURLcode(CURL *curl, CURLoption option, ecallapp::HttpData_t* data));
  MOCK_METHOD3(curl_easy_setopt, CURLcode(CURL *curl, CURLoption option, long int data));

  MOCK_METHOD3(curl_easy_getinfo, CURLcode(CURL *curl, CURLINFO info, long* data));

  MockCURLcode() {}
  ~MockCURLcode() {}
};

static MockCURLcode* M_URL;

struct curl_blob {
  void *data;
  size_t len;
  unsigned int flags;
};

CURL_EXTERN CURL* curl_easy_init(void) {
  return M_URL->curl_easy_init();
}

CURL_EXTERN CURLcode curl_easy_setopt(CURL *curl, CURLoption option, curl_slist* data) {
    return M_URL->curl_easy_setopt(curl, option, data);
}

CURL_EXTERN CURLcode curl_easy_setopt(CURL *curl, CURLoption option, const char* data) {
  return M_URL->curl_easy_setopt(curl, option, data);
}

CURL_EXTERN CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ecallapp::HttpData_t* data) {
  return M_URL->curl_easy_setopt(curl, option, data);
}


CURL_EXTERN CURLcode curl_easy_setopt(CURL *curl, CURLoption option, long int data) {
  return M_URL->curl_easy_setopt(curl, option, data);
}

CURL_EXTERN CURLcode curl_easy_perform(CURL *curl) {
    return M_URL->curl_easy_perform(curl);
}

CURL_EXTERN void curl_easy_cleanup(CURL *curl){
  return M_URL->curl_easy_cleanup(curl);
}

CURL_EXTERN CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, long* data) {
  return M_URL->curl_easy_getinfo(curl, info, data);
}

CURL_EXTERN CURL *curl_easy_duphandle(CURL *curl) {
  return M_URL->curl_easy_duphandle(curl);
}

CURL_EXTERN void curl_easy_reset(CURL *curl) {
  return M_URL->curl_easy_reset(curl);
}

CURL_EXTERN CURLcode curl_easy_recv(CURL *curl, void *buffer, size_t buflen, size_t *n){
  return M_URL->curl_easy_recv(curl, buffer, buflen, n);
}

CURL_EXTERN CURLcode curl_easy_send(CURL *curl, const void *buffer, size_t buflen, size_t *n){
  return M_URL->curl_easy_send(curl, buffer, buflen, n);
}

CURL_EXTERN CURLcode curl_easy_upkeep(CURL *curl) {
  return M_URL->curl_easy_upkeep(curl);
}

#endif
#endif