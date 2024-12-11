#include <iostream>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

// 用于处理上传文件
void handle_file_upload(int client_socket) {
    // 这里模拟返回上传文件后的响应
    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nAccess-Control-Allow-Origin: *\n\n"
                           "<html><body><h1>File Uploaded Successfully!</h1></body></html>";
    send(client_socket, response.c_str(), response.length(), 0);
    close(client_socket);
}

// 用于处理字符串输入
void handle_string_input(int client_socket, const std::string& input_string) {
    // 模拟返回压缩后的编码结果
    std::string compressed = "compress" + input_string;  // 这里替换为哈夫曼编码结果
    std::string response = "HTTP/1.1 200 OK\nContent-Type: application/json\nAccess-Control-Allow-Origin: *\n\n"
                           "{\"encoded\": \"" + compressed + "\"}";
    send(client_socket, response.c_str(), response.length(), 0);
    close(client_socket);
}

// 用于生成随机字符串
void handle_random_string(int client_socket) {
    std::string random_str = "abcdh";  // 这里可以生成更复杂的随机字符串
    std::string compressed = "codes:" + random_str;  // 这里替换为哈夫曼编码结果
    std::string response = "HTTP/1.1 200 OK\nContent-Type: application/json\nAccess-Control-Allow-Origin: *\n\n"
                           "{\"random_string\": \"" + compressed + "\"}";
    send(client_socket, response.c_str(), response.length(), 0);
    close(client_socket);
}

void serve_request(int client_socket, const std::string& request) {
    if (request.find("POST /upload") != std::string::npos) {
        handle_file_upload(client_socket);
    } else if (request.find("GET /string_input") != std::string::npos) {
        size_t pos = request.find("\r\n\r\n");
        std::string input_string = request.substr(pos + 4);  // 获取字符串输入
        handle_string_input(client_socket, input_string);
    } else if (request.find("GET /random_string") != std::string::npos) {
        handle_random_string(client_socket);
    } else {
        // 默认返回首页
        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nAccess-Control-Allow-Origin: *\n\n"
                               "<html><body><h1>Huffman Compression Software</h1></body></html>";
        send(client_socket, response.c_str(), response.length(), 0);
        close(client_socket);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) >= 0) {
        int valread = read(new_socket, buffer, 1024);
        std::string request(buffer, valread);
        serve_request(new_socket, request);
    }

    return 0;
}