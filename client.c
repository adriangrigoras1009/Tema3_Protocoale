#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson-master/parson.h"

#define PORT 8080
#define HOST "34.118.48.238"

int main() {
    char buff[2000];
    int sockfd;
    char* cookie;
    char* token;
    int ruleaza_serveru = 1;
    while(ruleaza_serveru == 1) {
        strcpy(buff, "\0");
        fgets(buff, sizeof(buff), stdin);
        if(strncmp(buff, "register", 8) == 0) {
            if(cookie == NULL) {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char username[50],password[50];
                printf("username=");
                fgets(username, sizeof(username), stdin);
                printf("password=");
                fgets(password, sizeof(password), stdin);
                username[strlen(username)-1] = '\0';
                password[strlen(password)-1] = '\0';
                JSON_Value *root = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root);
                json_object_set_string(root_object, "username", username);
                json_object_set_string(root_object, "password", password);

                char serialized_string[2000];
                json_serialize_to_buffer_pretty(root, serialized_string, 2000);
                char *message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json", NULL, NULL, 0, serialized_string, NULL, 0);
                send_to_server(sockfd, message);
                char *response = receive_from_server(sockfd);
                char *aux = strtok(response, " ");
                aux = strtok(NULL, " ");
                int nr = atoi(aux);
                if(nr == 201)
                    printf("Registered successfully\n");
                else if(nr == 400)
                    printf("This username already exists\n");
                close(sockfd);
            }
            else {
                printf("You are already logged in\n");
            }
            
        }
        else if(strncmp(buff, "login", 5) == 0) {
            if(cookie == NULL) {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char username[50], password[50];
                printf("username=");
                fgets(username, sizeof(username), stdin);
                printf("password=");
                fgets(password, sizeof(password), stdin);
                username[strlen(username)-1] = '\0';
                password[strlen(password)-1] = '\0';
                JSON_Value *root = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root);
                json_object_set_string(root_object, "username", username);
                json_object_set_string(root_object, "password", password);

                char serialized_string[2000];
                json_serialize_to_buffer_pretty(root, serialized_string, 2000);
                char *message = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json", NULL, NULL, 0, serialized_string, NULL, 0);
                send_to_server(sockfd, message);
                char *response = receive_from_server(sockfd);
                char *aux = strtok(response, " ");
                aux = strtok(NULL, " ");
                int nr = atoi(aux);
                if(nr == 200) {
                    printf("Successfully logged in\n");
                    aux = strtok(NULL, "\n");
                    aux = strtok(NULL, "\n");
                    aux = strtok(NULL, "\n");
                    aux = strtok(NULL, "\n");
                    aux = strtok(NULL, "\n");
                    aux = strtok(NULL, "\n");
                    aux = strtok(NULL, "\n");
                    aux = strtok(NULL, "\n");
                    aux = strtok(NULL, ":");
                    aux = strtok(NULL, ": ");
                    aux[strlen(aux)-1] = '\0';
                    cookie = malloc((strlen(aux)+1) * sizeof(char));
                    strcpy(cookie, aux);
                }
                else if(nr == 400)
                    printf("Wrong username or password\n");
                
                close(sockfd);
            }
            else {
                printf("You are already logged in\n");
            }
            
        }
        else if(strncmp(buff, "enter_library", 13) == 0) {
            if(cookie != NULL) {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char **cookies;
                cookies = malloc(sizeof(char*));
                cookies[0] = malloc((strlen(cookie)+1) * sizeof(char));
                strcpy(cookies[0], cookie);
                char *message = compute_get_request(HOST, "/api/v1/tema/library/access", NULL, cookies, 1, NULL, NULL, 0);
                send_to_server(sockfd, message);
                char *response = receive_from_server(sockfd);
                char *json = basic_extract_json_response(response);
                JSON_Value *val = json_parse_string(json);
                JSON_Object *final = json_value_get_object(val);
                char *final_string = json_object_get_string(final, "token");
                token = malloc((strlen(final_string)+1) * sizeof(char));
                strcpy(token, "Bearer ");
                strcat(token, final_string);
                char *aux = strtok(response, " ");
                aux = strtok(NULL," ");
                if(atoi(aux) == 200)
                    printf("You have library permission\n");
                else
                    printf("Library permission failed\n");
                close(sockfd);
            }
            else {
                printf("You are not logged in\n");
            }
        }
        else if(strncmp(buff, "get_books", 9) == 0) {
            if(token != NULL) {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char **headers;
                char **headersName;
                headers = malloc(sizeof(char*));
                headersName = malloc(sizeof(char*));
                headersName[0] = malloc(strlen("Authorization") * sizeof(char));
                headers[0] = malloc((strlen(token)+1) * sizeof(char));
                strcpy(headersName[0], "Authorization");
                strcpy(headers[0], token);
                char *message = compute_get_request(HOST, "/api/v1/tema/library/books", NULL, NULL, 0, headersName, headers, 1);
                send_to_server(sockfd, message);
                char *response = receive_from_server(sockfd);
                char *ceva = basic_extract_json_response(response);
                char *aux = strtok(response, " ");
                aux = strtok(NULL, " ");
                if(atoi(aux) == 200) {
                    printf("[");
                    char *pch = strtok(ceva, ",");
                    while(pch != NULL) {
                        printf("%s\n", pch);
                        pch = strtok(NULL, ",");
                    }
                }   
            }
            else {
                printf("You don't have permission\n");
            }
        }
        else if(strncmp(buff, "get_book", 8) == 0) {
            if(token != NULL) {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char id[10];
                printf("id=");
                fgets(id, sizeof(id), stdin);
                id[strlen(id)-1] = '\0';
                int id1 = atoi(id);

                char **headers;
                char **headersName;
                headers = malloc(sizeof(char*));
                headersName = malloc(sizeof(char*));
                headersName[0] = malloc(strlen("Authorization") * sizeof(char));
                headers[0] = malloc((strlen(token)+1) * sizeof(char));
                strcpy(headersName[0], "Authorization");
                strcpy(headers[0], token);
                char url[100];
                strcpy(url, "/api/v1/tema/library/books/");
                strcat(url, id);
                char *message = compute_get_request(HOST, url, NULL, NULL, 0, headersName, headers, 1);
                send_to_server(sockfd, message);
                char *response = receive_from_server(sockfd);
                char *ceva = basic_extract_json_response(response);
                char *aux = strtok(response, " ");
                aux = strtok(NULL, " ");
                if(atoi(aux) == 200) {
                    char *pch = strtok(ceva, "]");
                    printf("%s\n", pch);
                }
                else
                    printf("Book doesn't exist\n");
                
                

            }
            else {
                printf("You don't have permission\n");
            }
        }
        else if(strncmp(buff, "add_book", 8) == 0) {
            if(token != NULL) {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char title[100], author[100], genre[100], page_count[100], publisher[100];
                printf("title=");
                fgets(title, sizeof(title), stdin);
                printf("author=");
                fgets(author, sizeof(author), stdin);
                printf("genre=");
                fgets(genre, sizeof(genre), stdin);
                printf("page_count=");
                fgets(page_count, sizeof(page_count), stdin);
                printf("publisher=");
                fgets(publisher, sizeof(publisher), stdin);
                title[strlen(title)-1] = '\0';
                author[strlen(author)-1] = '\0';
                genre[strlen(genre)-1] = '\0';
                page_count[strlen(page_count)-1] = '\0';
                publisher[strlen(publisher)-1] = '\0';
                int page_c = atoi(page_count);

                JSON_Value *root = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root);
                json_object_set_string(root_object, "title", title);
                json_object_set_string(root_object, "author", author);
                json_object_set_string(root_object, "genre", genre);
                json_object_set_number(root_object, "page_count", page_c);
                json_object_set_string(root_object, "publisher", publisher);

                char serialized_string[2000];
                json_serialize_to_buffer_pretty(root, serialized_string, 2000);

                char **headers;
                char **headersName;
                headers = malloc(sizeof(char*));
                headersName = malloc(sizeof(char*));
                headersName[0] = malloc(strlen("Authorization") * sizeof(char));
                headers[0] = malloc((strlen(token)+1) * sizeof(char));
                strcpy(headersName[0], "Authorization");
                strcpy(headers[0], token);

                char *message = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json", headers, headersName, 1, serialized_string, NULL, 0);
                send_to_server(sockfd, message);
                char *response = receive_from_server(sockfd);
                char *aux = strtok(response, " ");
                aux = strtok(NULL, " ");
                if(atoi(aux) == 200)
                    printf("Book was added\n");
                else
                    printf("Book was not added\n");
            }
            else {
                printf("You don't have permission\n");
            }
        }
        else if(strncmp(buff, "delete_book", 11) == 0) {
            if(token != NULL) {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char id[10];
                printf("id=");
                fgets(id, sizeof(id), stdin);
                id[strlen(id)-1] = '\0';
                int id1 = atoi(id);

                char **headers;
                char **headersName;
                headers = malloc(sizeof(char*));
                headersName = malloc(sizeof(char*));
                headersName[0] = malloc(strlen("Authorization") * sizeof(char));
                headers[0] = malloc((strlen(token)+1) * sizeof(char));
                strcpy(headersName[0], "Authorization");
                strcpy(headers[0], token);
                char url[100];
                strcpy(url, "/api/v1/tema/library/books/");
                strcat(url, id);
                char *message = compute_delete_request(HOST, url, NULL, NULL, 0, headersName, headers, 1);
                send_to_server(sockfd, message);
                char *response = receive_from_server(sockfd);
                char *ceva = basic_extract_json_response(response);
                char *aux = strtok(response, " ");
                aux = strtok(NULL, " ");
                if(atoi(aux) == 200)
                    printf("Book was removed\n");
                else
                    printf("%s\n", ceva);

            }
            else {
                printf("You don't have permission\n");
            }
        }
        else if(strncmp(buff, "logout", 6) == 0) {
            if(cookie != NULL) {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char **cookies;
                cookies = malloc(sizeof(char*));
                cookies[0] = malloc((strlen(cookie)+1) * sizeof(char));
                strcpy(cookies[0], cookie);
                char *message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, cookies, 1, NULL, NULL, 0);
                send_to_server(sockfd, message);
                char *response = receive_from_server(sockfd);
                char *aux = strtok(response, " ");
                aux = strtok(NULL, " ");
                int nr = atoi(aux);
                if(nr != 200) {
                    printf("Logout successfully!\n");
                }
                else {
                    printf("Logout failed!\n");
                }
            }
            else {
                printf("You are not logged in\n");
            }
        }
        else if(strncmp(buff, "exit", 4) == 0) {
            break;
        }
    }
    
}