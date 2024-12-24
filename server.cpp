#include "io.cpp"
#include "encoder_decoder.cpp"
#include "human_desire.cpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <regex>
#include <cstring>

#define PORT 8080
const int BUFFER_SIZE = 8192;
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

void handle_decode_string(int client_socket, const string& input_string, const string& file_name) {
    cout << "decode before\n" << input_string << endl;
    string origin = handle_special_char(input_string);
    origin = decode(origin);
    origin = refined_string2json(origin);
    cout << "origin\n" << origin << endl;
    // 获取真正的文件名
    string ffile_name;
    size_t dot_pos = file_name.find(".");
    if(dot_pos!=string::npos){
        ffile_name = file_name.substr(0, dot_pos);
    }
    else{
        ffile_name = file_name;
    }
    // 保存源码到txt文件
    save_txt(origin, ffile_name + "_decoded");
      // 找到 "encoded_content" 后的结束位置
    size_t pos = input_string.find("\"encoded_content\":");
    string new_json = input_string;
    if (pos != std::string::npos) {
        pos = input_string.find("}", pos);
        if (pos != std::string::npos) {
            // 在 "encoded_content" 后面添加 "origin"
            new_json.insert(pos, ",\n    \"origin_data\": \"" + origin + "\"");
        }
    }
    cout << new_json << endl;
    // 构建响应体
    string response_body = new_json;

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
    close(client_socket);
}

void find_string(int client_socket, const string& input_string, const string& find_string) {
    string af_string = regex_replace(input_string, regex("\\\\n"), "\n");
    string ff_string = regex_replace(find_string, regex("\\\\n"), "\n");
    af_string = handle_special_char(af_string);
    ff_string = handle_special_char(ff_string);
    string find_out = get_total_part(af_string, ff_string);
    find_out = refined_string2json(find_out);
    string response_body = "{\n  \"find_content\": \"" + find_out + "\"\n}";

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

void handle_file_upload(int client_socket, const string& input_string, const string& file_name) {
    // 响应内容
    cout << "origin content:\n" << input_string.substr(0, 100) << endl;
    string af_string = regex_replace(input_string, regex("\\\\n"), "\n");
    af_string = handle_special_char(af_string);
    string compressed[2];
    encode(af_string, compressed);
    string response_body = convertToJSON(af_string, compressed[0], compressed[1]);

    // 获取文件名
    string ffile_name;
    size_t dot_pos = file_name.find(".");
    if(dot_pos!=string::npos){
        ffile_name = file_name.substr(0, dot_pos);
    }
    // 保存编码到json文件中
    string compressed_body = convertToJSON(af_string, compressed[0], compressed[1], true);
    save_json(compressed_body, ffile_name);

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

void handle_string_input(int client_socket, const string& input_string, const string& file_name) {
    // 获取文件名
    // string af_string = regex_replace(input_string, regex("\\\\n"), "\n");
    string af_string = handle_special_char(input_string);
    cout << "af_string: "<<af_string << endl;
    string ffile_name;
    size_t dot_pos = file_name.find(".");
    if(dot_pos!=string::npos){
        ffile_name = file_name.substr(0, dot_pos);
    }
    else{
        ffile_name = file_name;
    }
    save_txt(af_string, ffile_name);
    // 编码字符串
    string compressed[2];
    encode(af_string, compressed);
    // printf("\nencode succeed!\n");
    string response_body = convertToJSON(af_string, compressed[0], compressed[1]);
    // cout << "\nresponse\n" << response_body << endl;
    // 保存编码到json文件中
    string compressed_body = convertToJSON(af_string, compressed[0], compressed[1], true);
    save_json(compressed_body, ffile_name);
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
    // 保存源码到txt
    random_str = handle_special_char(random_str);
    save_txt(random_str, "random_input");
    string compressed[2];
    string af_string = regex_replace(random_str, regex("\\\\n"), "\n");
    encode(af_string, compressed);
    string response_body = convertToJSON(af_string, compressed[0], compressed[1]);
    // 保存编码到json文件中
    string compressed_body = convertToJSON(af_string, compressed[0], compressed[1], true);
    save_json(compressed_body, "random_input");
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
        cout << "request:\n" << request.substr(0, 100) << endl;
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
        printf("\nget body\n");
        cout << "body:\n" << body << endl;
        string input_string;
        string file_name;
        size_t content_pos = body.find("\"fileContent\":\"");
        size_t file_name_pos = body.find("\"fileName\":\"");
        if(file_name_pos!=string::npos){
            file_name_pos += 12;
            size_t end_pos = body.find(",", file_name_pos) - 1;
            file_name = body.substr(file_name_pos, end_pos - file_name_pos);
        }
        if(content_pos!=string::npos){
            content_pos += 15;
            size_t end_pos = body.find("}", content_pos) - 1;
            input_string = body.substr(content_pos, end_pos - content_pos);
        }
        handle_file_upload(client_socket, input_string, file_name);
    }
    else if(request.find("POST /string_input") != string::npos){
        cout << "request:\n" << request << endl;
        // 获取请求体
        size_t pos = request.find("\r\n\r\n");
        string body = request.substr(pos + 4);
        // 解析 JSON 数据
        string input_string;
        size_t string_pos = body.find("\"string\":\"");
        if (string_pos != string::npos) {
            string_pos += 10; // 跳过 "\"string\":\""
            size_t end_pos = body.find("\"fileName\":\"", string_pos) - 2;
            input_string = body.substr(string_pos, end_pos - string_pos);
        } // 处理完输入的字符串
        string file_name;
        size_t file_name_pos = body.find("\"fileName\":\"");
        if(file_name_pos!=string::npos){
            file_name_pos += 12;
            size_t end_pos = body.find("\"", file_name_pos);
            file_name = body.substr(file_name_pos, end_pos - file_name_pos);
        } // 处理完文件名
        // 处理字符串并生成响应
        handle_string_input(client_socket, input_string, file_name);
    }
    else if (request.find("POST /search") != string::npos){
        size_t pos = request.find("\r\n\r\n");
        string body = request.substr(pos + 4);

        // 解析 JSON 数据
        string find_s;
        size_t string_pos = body.find("\"Search\":\"");
        if (string_pos != string::npos) {
            string_pos += 10; // 跳过 "\"Search\":\""
            size_t end_pos = body.find("}", string_pos) - 1;
            find_s = body.substr(string_pos, end_pos - string_pos);
        } // 处理完了查找的字符串
        cout << find_s << endl;
        string origin_string;
        size_t origin_pos = body.find("\"OriginString\":\"");
        if (origin_pos != string::npos) {
            origin_pos += 16; // 跳过 "\"OriginString\":\""
            size_t end_pos = body.find("\"Search\":\"", origin_pos) - 2;
            origin_string = body.substr(origin_pos, end_pos - origin_pos);
        } // 处理完了原始的字符串
        cout << find_s << endl;
        // 处理字符串并生成响应
        find_string(client_socket, origin_string, find_s);
    }
    else if (request.find("POST /decode") != string::npos) {
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
        string file_name;
        size_t file_name_pos = body.find("\"fileName\":\"");
        if(file_name_pos!=string::npos){
            file_name_pos += 12;
            size_t end_pos = body.find("\"", file_name_pos);
            file_name = body.substr(file_name_pos, end_pos - file_name_pos);
        }
        size_t content_pos = body.find("\"fileContent\":{");  // 查找 "fileContent":{ 的位置
        if (content_pos != string::npos) {
            content_pos += 15;  // 跳过 "fileContent":{ 的 15 个字符
            
            size_t open_brace_count = 1;  // 初始时遇到一个 '{'
            size_t end_pos = content_pos;
            
            // 遍历整个字符串，找到匹配的 '}'
            while (open_brace_count > 0 && end_pos < body.length()) {
                if (body[end_pos] == '{') {
                    open_brace_count++;  // 如果是左括号，增加计数
                } else if (body[end_pos] == '}') {
                    open_brace_count--;  // 如果是右括号，减少计数
                }
                end_pos++;  // 移动到下一个字符
            }
        
            if (open_brace_count == 0) {
                // 提取整个 JSON 字符串，包含两端的大括号
                input_string = body.substr(content_pos - 1, end_pos - content_pos + 1);
                // 这里用 content_pos - 1 是为了保留开头的 "{"，end_pos - content_pos + 1 保留结尾的 "}"
            }
        }
        handle_decode_string(client_socket, input_string, file_name);
    }
    else if(request.find("OPTIONS /string_input") != string::npos || request.find("OPTIONS /upload") != string::npos || request.find("OPTIONS /random_string") != string::npos || request.find("OPTIONS /search") != string::npos || request.find("OPTIONS /decode") != string::npos){
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

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket created successfully\n";

    // 设置服务器地址相关信息
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 绑定套接字到指定地址和端口
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Bind successful\n";

    // 开始监听端口，允许的最大连接队列长度为3
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening on port " << PORT << "\n";

    // 循环接受请求
    while ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) >= 0) {
        char buffer[BUFFER_SIZE];
        std::string request;
        request.reserve(1024 * 1000); // 预留空间，可按实际预估调整
        int valread;

        // 读取 HTTP 请求头和数据
        while ((valread = read(new_socket, buffer, BUFFER_SIZE - 1)) > 0) {
            buffer[valread] = '\0'; // 确保缓冲区以 null 结尾
            request.append(buffer, valread);

            // 检查是否包含完整的 HTTP 报文
            auto header_end = request.find("\r\n\r\n");
            if (header_end != std::string::npos) {
                // HTTP 报文包含头部，检查 Content-Length
                size_t content_length_pos = request.find("Content-Length: ");
                if (content_length_pos != std::string::npos) {
                    size_t start = content_length_pos + 16; // "Content-Length: " 长度
                    size_t end = request.find("\r\n", start);
                    int content_length = std::stoi(request.substr(start, end - start));
                    size_t total_length = header_end + 4 + content_length; // 头部 + \r\n\r\n + 内容

                    if (request.length() >= total_length) {
                        break; // 接收完整
                    }
                } else {
                    break; // 没有 Content-Length，默认认为是完整的
                }
            }
        }

        if (valread < 0) {
            perror("Read error");
            close(new_socket);
            continue;
        }

        std::cout << "Request received. Length: " << request.length() << " bytes\n";

        try {
            serve_request(new_socket, request); // 调用处理函数
        } catch (...) {
            std::cerr << "An error occurred while serving the request\n";
        }

        request.clear();
        memset(buffer, 0, BUFFER_SIZE);
        close(new_socket);
    }

    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    close(server_fd);
    return 0;
}