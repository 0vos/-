#include "io.cpp"
#include "encoder_decoder.cpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <regex>

#define PORT 8080
using namespace std;
bool ends_with(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix;
}
// 根据文件扩展名获取 MIME 类型
string get_mime_type(const string& file_path) {
    if (ends_with(file_path, ".html")) return "text/html";
    if (ends_with(file_path, ".css")) return "text/css";
    if (ends_with(file_path, ".js")) return "application/javascript";
    if (ends_with(file_path, ".png")) return "image/png";
    if (ends_with(file_path, ".jpg") || ends_with(file_path, ".jpeg")) return "image/jpeg";
    return "application/octet-stream"; // 默认 MIME 类型
}

void handle_file_upload(int client_socket, const string& input_string) {
    // 响应内容
    cout << "origin content:\n" << input_string << endl;
    string af_string = regex_replace(input_string, regex("\\\\n"), "\n");
    string compressed[2];
    encode(af_string, compressed);
    string response_body = convertToJSON(compressed[0], compressed[1]);

    // HTTP 响应头，添加 CORS 支持
    string http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Content-Length: " + to_string(response_body.length()) + "\r\n\r\n" +
        response_body;

    send(client_socket, http_response.c_str(), http_response.length(), 0);
    close(client_socket);
}

void handle_string_input(int client_socket, const string& input_string) {
    // 编码字符串
    string compressed[2];
    string af_string = regex_replace(input_string, regex("\\\\n"), "\n");
    encode(af_string, compressed);
    string response_body = convertToJSON(compressed[0], compressed[1]);
    // 构造 HTTP 响应
    string http_response =
        "HTTP/1.1 200 OK\r\n"                              // 返回成功状态码
        "Content-Type: application/json\r\n"              // 明确指定 JSON 类型
        "Access-Control-Allow-Origin: *\r\n"              // 允许所有源
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"  // 允许 Content-Type 请求头
        "Content-Length: " + to_string(response_body.length()) + "\r\n\r\n" +
        response_body;

    // 发送响应
    send(client_socket, http_response.c_str(), http_response.length(), 0);

    // 关闭连接
    close(client_socket);
}

// 用于生成随机字符串
void handle_random_string(int client_socket) {
    string random_str = get_text(10, 48, 78);
    string compressed[2];
    string af_string = regex_replace(random_str, regex("\\\\n"), "\n");
    encode(af_string, compressed);
    string response_body = convertToJSON(compressed[0], compressed[1]);

    // 构造 HTTP 响应
    string http_response =
        "HTTP/1.1 200 OK\r\n"                              // 返回成功状态码
        "Content-Type: application/json\r\n"              // 明确指定 JSON 类型
        "Access-Control-Allow-Origin: *\r\n"              // 允许所有源
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"  // 允许 Content-Type 请求头
        "Content-Length: " + std::to_string(response_body.length()) + "\r\n\r\n" +
        response_body;

    // 发送响应
    send(client_socket, http_response.c_str(), http_response.length(), 0);
    close(client_socket);
}

// 处理静态文件请求
void serve_static_file(int client_socket, const string& file_path) {
    ifstream file(file_path, ios::binary);
    if (!file.is_open()) {
        // 如果文件不存在，返回 404
        string error_response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 50\r\n\r\n"
            "<html><body><h1>404 Not Found</h1></body></html>";
        send(client_socket, error_response.c_str(), error_response.length(), 0);
        close(client_socket);
        return;
    }

    // 读取文件内容
    stringstream buffer;
    buffer << file.rdbuf();
    string file_content = buffer.str();
    file.close();

    // 获取文件的 MIME 类型
    string mime_type = get_mime_type(file_path);

    // 构造 HTTP 响应
    string http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + mime_type + "\r\n"
        "Content-Length: " + to_string(file_content.length()) + "\r\n\r\n" +
        file_content;

    send(client_socket, http_response.c_str(), http_response.length(), 0);
    close(client_socket);
}

// 处理客户端请求
void serve_request(int client_socket, const string& request) {
    if(request.find("GET /random_string") != string::npos){
        handle_random_string(client_socket);
    } else if (request.find("GET ") != string::npos) {
        // 处理静态文件请求
        size_t pos = request.find("GET ");
        size_t path_end = request.find(" ", pos + 4);
        string file_path = request.substr(pos + 4, path_end - pos - 4);

        if (file_path == "/") {
            file_path = "index.html"; // 默认首页
        } else {
            file_path = file_path.substr(1); // 去掉开头的 "/"
        }

        serve_static_file(client_socket, file_path);
    }
    else if (request.find("POST /upload") != string::npos) {
        // 处理文件上传请求
        cout << "request:\n" << request << endl;
        size_t body_pos = request.find("\r\n\r\n");
        printf("body pos%lu", body_pos);
        if (body_pos == string::npos) {
            // 如果没有请求体，返回 400 错误
            string error_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
            send(client_socket, error_response.c_str(), error_response.length(), 0);
            close(client_socket);
            return;
        }
        string body = request.substr(body_pos + 4);
        printf("get body\n");
        cout << "body:\n" << body << endl;
        string input_string;
        size_t content_pos = body.find("\"fileContent\":\"");
        if(content_pos!=string::npos){
            content_pos += 15;
            size_t end_pos = body.find("\"", content_pos);
            input_string = body.substr(content_pos, end_pos - content_pos);
        }
        handle_file_upload(client_socket, input_string);
    }
    else if(request.find("POST /string_input") != string::npos){
        // cout << "request:\n" << request << endl;
        // 获取请求体
        size_t pos = request.find("\r\n\r\n");
        string body = request.substr(pos + 4);

        // 解析 JSON 数据
        string input_string;
        size_t string_pos = body.find("\"string\":\"");
        if (string_pos != string::npos) {
            string_pos += 10; // 跳过 "\"string\":\""
            size_t end_pos = body.find("\"", string_pos);
            input_string = body.substr(string_pos, end_pos - string_pos);
        }
        // 处理字符串并生成响应
        handle_string_input(client_socket, input_string);
    }
    else if(request.find("OPTIONS /string_input") != string::npos || request.find("OPTIONS /upload") != string::npos || request.find("OPTIONS /random_string") != string::npos){
        string options_response =
            "HTTP/1.1 204 No Content\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Content-Length: 0\r\n\r\n";
        send(client_socket, options_response.c_str(), options_response.length(), 0);
        close(client_socket);
        return;
    }
    else {
        // 如果请求方法不为 GET 或 POST，返回 400 错误
        string error_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(client_socket, error_response.c_str(), error_response.length(), 0);
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
        string request(buffer, valread);
        serve_request(new_socket, request);
    }

    return 0;
}