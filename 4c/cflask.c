#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "civetweb.h"
#include "functionslist.h"  
#include "functions.h"  
#include "functions.c"


typedef struct url_tree_node {
    char *url;
    int index;
    struct url_tree_node *left;
    struct url_tree_node *right;
} url_tree_node_t;

url_tree_node_t *insert_url(url_tree_node_t *root, const char *url, int index);
int find_url(url_tree_node_t *root, const char *url);
void free_url_tree(url_tree_node_t *root);

url_tree_node_t *url_tree_root = NULL;

url_tree_node_t *create_node(const char *url, int index) {
    url_tree_node_t *node = (url_tree_node_t *)malloc(sizeof(url_tree_node_t));
    node->url = strdup(url);
    node->index = index;
    node->left = node->right = NULL;
    return node;
}

url_tree_node_t *insert_url(url_tree_node_t *root, const char *url, int index) {
    if (root == NULL) {
        return create_node(url, index);
    }
    int cmp = strcmp(url, root->url);
    if (cmp < 0) {
        root->left = insert_url(root->left, url, index);
    } else if (cmp > 0) {
        root->right = insert_url(root->right, url, index);
    }
    return root;
}

int find_url(url_tree_node_t *root, const char *url) {
    if (root == NULL) {
        return 0;
    }
    int cmp = strcmp(url, root->url);
    if (cmp == 0) {
        return root->index;
    } else if (cmp < 0) {
        return find_url(root->left, url);
    } else {
        return find_url(root->right, url);
    }
}

void free_url_tree(url_tree_node_t *root) {
    if (root) {
        free_url_tree(root->left);
        free_url_tree(root->right);
        free(root->url);
        free(root);
    }
}


void initialize_url_tree() {
    url_tree_root = insert_url(url_tree_root, "/", ROOT);
    url_tree_root = insert_url(url_tree_root, "/square", SQUARE);
    url_tree_root = insert_url(url_tree_root, "/cube", CUBE);
    url_tree_root = insert_url(url_tree_root, "/hello", HELLO);
    url_tree_root = insert_url(url_tree_root, "/arithmetic/prime", PRIME);
    url_tree_root = insert_url(url_tree_root, "/pingpong", PINGPONG);
    url_tree_root = insert_url(url_tree_root, "/arithmetic/fibonacci", FIBO);
}

int get_query_param(const char *query_string, const char *param, char *value, size_t value_size) {
    return mg_get_var(query_string,100,param, value, value_size);
}

static int begin_request_handler(struct mg_connection *conn) {
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    char content[256]; 
    char name[100]; 
    char buff[40];
    int content_length;
    int index = find_url(url_tree_root, request_info->uri);
    if (index == SQUARE || index == CUBE || index == PRIME || index == FIBO) {
        get_query_param(request_info->query_string, "num", buff, sizeof(buff));
        int number = atoi(buff);
        char *c = functionList[index]((void *)&number);
        content_length = snprintf(content, sizeof(content), "%s", c);
    } else {
        get_query_param(request_info->query_string, "str", name, sizeof(name));
        char *c = functionList[index]((void *)&name);
        content_length = snprintf(content, sizeof(content), "%s", c);
    }



    // For part 1,2,3

    // if (strcmp(request_info->uri, "/hello") == 0) {
    //         get_query_param(request_info->query_string, "str", name, sizeof(name));
    //         char *c = functionList[HELLO](name);
    //         content_length = snprintf(content, sizeof(content), "%s", c);
    // } else if (strcmp(request_info->uri, "/") == 0) {
    //     char *c = functionList[ROOT]((void*)NULL);
    //     content_length = snprintf(content, sizeof(content),"%s",c);
    // } else if (strcmp(request_info->uri, "/square") == 0) {
    //             get_query_param(request_info->query_string, "num", buff, sizeof(buff));
    //             int number = atoi(buff);
    //             char* k = functionList[SQUARE]((void*)&number);
    //             content_length = snprintf(content, sizeof(content), "%s",k);
    // } else if (strcmp(request_info->uri, "/cube") == 0) {
    //             get_query_param(request_info->query_string, "num", buff, sizeof(buff));
    //             int number = atoi(buff);
    //             char* k = functionList[CUBE]((void*)&number);
    //             content_length = snprintf(content, sizeof(content), "%s",k);
    // } else if (strcmp(request_info->uri, "/pingpong") == 0) {
    //         get_query_param(request_info->query_string, "str", name, sizeof(name));
    //         char *c = functionList[PINGPONG](name);
    //         content_length = snprintf(content, sizeof(content), "%s", name);
    // } else if (strcmp(request_info->uri, "/arithmetic/prime") == 0) {
    //             get_query_param(request_info->query_string, "num", buff, sizeof(buff));
    //             int number = atoi(buff);
    //             char* k = functionList[PRIME]((void*)&number);
    //             content_length = snprintf(content, sizeof(content), "%s",k);
    // }
    // else if (strcmp(request_info->uri, "/arithmetic/fibonacci") == 0) {
    //             get_query_param(request_info->query_string, "num", buff, sizeof(buff));
    //             int number = atoi(buff);
    //             char* k = functionList[FIBO]((void*)&number);
    //             content_length = snprintf(content, sizeof(content), "%s",k);
    // }
    // else{
    //         content_length = snprintf(content, sizeof(content), "Invallid request");
    // }



    // Send HTTP reply to the client
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n"
              "Content-Length: %d\r\n"        // Always set Content-Length
              "\r\n"
              "%s",
              content_length, content);

    // Returning non-zero tells CivetWeb that our function has replied to
    // the client, and CivetWeb should not send client any more data.
    return 1;
}

int main(int argc, char *argv[]) {
    struct mg_context *ctx;
    struct mg_callbacks callbacks;
    if (argc != 3) {
        printf("Usage: %s <port> <num_threads>\n", argv[0]);
        return 1;
    }

    const char *port = argv[1];
    const char *num_threads = argv[2];
    initialize_url_tree();


    const char *options[] = {
        "listening_ports", port,
        "num_threads", num_threads,
        NULL
    };

    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = begin_request_handler;

    // Start the web server.
    ctx = mg_start(&callbacks, NULL, options);

    // Wait until user hits "enter". Server is running in a separate thread.
    getchar();

    // Stop the server.
    mg_stop(ctx);
    free_url_tree(url_tree_root);

    return 0;
}
