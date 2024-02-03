#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <raylib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TITLE "Game"

static void init_server(void)
{
    // Create socket
    const int fd = socket(PF_INET, SOCK_STREAM, 0);

    // Bind to open port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr))) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Read port
    socklen_t addr_len = sizeof(addr);
    getsockname(fd, (struct sockaddr*)&addr, &addr_len);
    printf("Server is on port %d\n", (int)ntohs(addr.sin_port));

    if (listen(fd, 1)) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connection
    struct sockaddr_storage caddr;
    socklen_t caddr_len = sizeof(caddr);
    const int cfd = accept(fd, (struct sockaddr*)&caddr, &caddr_len);

    // Read from client
    char buf[1024];
    recv(cfd, buf, sizeof(buf), 0);

    printf("Client says:\n %s\n", buf);

    close(fd);
    close(cfd);
}

static void init_client(const char *server, int port)
{
    const int fd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)port);

    char addr_str[NI_MAXHOST + NI_MAXSERV + 1];
    snprintf(addr_str, sizeof(addr_str), "%s:%d", server, port);

    // Parse into address
    inet_pton(AF_INET, addr_str, &addr.sin_addr);

    // Connect to server
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr))) {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }

    const char *msg = "Hello from the client";
    send(fd, msg, strlen(msg) + 1, 0);

    close(fd);
}

static void init_game(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);

    SetTargetFPS(60);
}

static void run_game(void)
{
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }
}

static void quit_game(void)
{
    CloseWindow();
}

int main(int argc, char** argv)
{
    if (argc > 1 && !strcmp(argv[1], "client")) {
        if (argc != 3) {
            perror("Not enough args");
            return -1;
        }

        int port;
        sscanf(argv[2], "%d", &port);

        init_client("127.0.0.1", port);
        init_game();
        run_game();
        quit_game();
    } else {
        init_server();
    }

    return 0;
}
