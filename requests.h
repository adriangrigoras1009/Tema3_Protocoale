#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,
							char **cookies, int cookies_count, char **headersName, char **headers, int headers_count);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, char **headers, char **headersN,
							int headers_count, char *mess, char** cookies, int cookies_count);

char *compute_delete_request(char *host, char *url, char *query_params,
							char **cookies, int cookies_count, char **headersName, char **headers, int headers_count);
#endif
