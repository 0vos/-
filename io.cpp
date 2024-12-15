#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <sstream>
#include <vector>
#include <zlib.h>
#include <unordered_map>
#include <queue>
#include <functional>
#include "blocks.cpp"
using namespace std;

// 获取图片文件信息
string get_image(const string& file_path) {
    ifstream image_file(file_path, ios::binary);
    if (!image_file.is_open()) {
        cerr << "无法打开图像文件" << endl;
        return "";
    }

    // 跳过文件头（简单处理，直接读取一定字节数作为文件头跳过，实际可能需按格式规范精确处理）
    image_file.seekg(16);  // 这里假设跳过前16字节的文件头，不同格式需按实际调整

    // 将剩余的图像数据读取到字符串中
    string image_data((istreambuf_iterator<char>(image_file)), istreambuf_iterator<char>());
    image_file.close();

    // 字符串进行压缩
    uLongf buffer_size = compressBound(image_data.size());
    vector<Bytef> compressed_data(buffer_size);
    int result = compress(&compressed_data[0], &buffer_size, reinterpret_cast<const Bytef*>(image_data.c_str()), image_data.size());
    if (result == Z_OK) {
        string compressed_string(reinterpret_cast<char*>(compressed_data.data()), buffer_size);
        return compressed_string;
    } else {
        cerr << "图像数据压缩失败" << endl;
        return image_data;  // 如果压缩失败，返回原始未压缩的图像数据字符串
    }
}

// 获取文本文件信息
string get_text(const string& file_path){
    ifstream text(file_path);
    if(!text.is_open()){
        printf("无法打开文件\n");
        exit(1);
    }
    // 获取整个文本
    string content, line;
    while(getline(text, line)){
        content += line;
        content += "\n";
    }
    cout << content << endl;
    text.close();
    return content;
}

// 获取人工输入信息
string get_text(){
    string content;
    getline(cin, content);
    cout << content << endl;
    return content;
}

// 获取随机生成的信息
string get_text(int max_length, int min_ascii, int max_ascii){
    // 初始化随机生成器
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> distribution(min_ascii, max_ascii);
    // 构造随机字符串
    string content;
    for(size_t i=0;i<max_length;++i){
        content += char(distribution(generator));
    }
    cout << "content:" << content << "\nlength:" << content.length()<< endl;
    return content;
}

// 获取编码文件信息
void get_huff(const string& file_path, string* output_list) {
    // 统计字符频率
    ifstream input_file(file_path);
    if (!input_file.is_open()) {
        cerr << "无法打开文件: " << file_path << endl;
        return;
    }
    unordered_map<char, int> freq_map;
    char ch;
    while (input_file.get(ch)) {
        freq_map[ch]++;
    }
    input_file.close();

    // 根据字符频率构建哈夫曼树节点数组
    vector<Node> nodes;
    for (const auto& it : freq_map) {
        Node node;
        node.weight = it.second;
        nodes.push_back(node);
    }

    // 节点个数
    int node_count = nodes.size();
    // 总节点数（包含额外的非叶子节点）
    int total_node_count = 2 * node_count - 1;
    // 扩充节点数组到总节点数大小，用于构建完整哈夫曼树结构
    nodes.resize(total_node_count);

    // 构建哈夫曼树，使用优先队列来辅助，按照节点权重（字符频率）从小到大排序
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    for (int i = 0; i < node_count; ++i) {
        pq.push(make_pair(nodes[i].weight, i));
    }
    int index = node_count;
    while (pq.size() > 1) {
        // 取出权重最小的两个节点
        auto left_pair = pq.top();
        pq.pop();
        auto right_pair = pq.top();
        pq.pop();

        // 创建新的父节点
        Node parent_node;
        parent_node.weight = left_pair.first + right_pair.first;
        parent_node.left = left_pair.second;
        parent_node.right = right_pair.second;

        // 设置子节点的父节点索引
        nodes[left_pair.second].parent = index;
        nodes[right_pair.second].parent = index;

        // 将新父节点加入节点数组和优先队列
        nodes[index] = parent_node;
        pq.push(make_pair(parent_node.weight, index));

        index++;
    }

    // 生成哈夫曼编码表
    vector<TableBlock> huffman_table;
    for (int i = 0; i < node_count; ++i) {
        TableBlock block;
        block.key = (char)i;  // 这里简单用索引对应的ASCII码值作为字符，实际可能需调整
        int current = i;
        string code = "";
        while (nodes[current].parent!= -1) {
            int parent = nodes[current].parent;
            if (nodes[parent].left == current) {
                code = "0" + code;
            } else {
                code = "1" + code;
            }
            current = parent;
        }
        block.value = code;
        huffman_table.push_back(block);
    }

    // 将哈夫曼表转换为字符串格式（按照特定格式拼接，方便后续处理）
    string huffman_table_str;
    for (const auto& block : huffman_table) {
        huffman_table_str += block.key;
        huffman_table_str += " ";
        huffman_table_str += block.value;
        huffman_table_str += "\n";
    }

    // 对原文件内容进行编码（重新读取文件，根据编码表转换为01串）
    ifstream input_again(file_path);
    if (!input_again.is_open()) {
        cerr << "再次打开文件失败" << endl;
        return;
    }
    string encoded_data;
    string line;
    while (getline(input_again, line)) {
        for (char c : line) {
            for (const auto& block : huffman_table) {
                if (block.key == c) {
                    encoded_data += block.value;
                    break;
                }
            }
        }
    }
    input_again.close();

    output_list[0] = huffman_table_str;
    output_list[1] = encoded_data;
}

/*
将编码信息写到编码文件.txt（放到data目录下）
编码信息比如：
哈夫曼表
key value
A   0
B   10
C   111
字符串“ABC”对应的编码内容：010111
写入压缩文件中的格式：
A_0 B_10 C_111###010111
*/
void save_huff(const string& file_path, string* pair) {
    ofstream out_file(file_path);
    if (out_file.is_open()) {
        // 先写入哈夫曼表信息
        out_file << pair[0];

        // 写入分隔符
        out_file << "###";

        // 再写入编码内容
        out_file << pair[1];

        out_file.close();
    } else {
        cerr << "无法打开文件用于保存哈夫曼编码信息" << endl;
    }
}

// 保存源码
void save_origin(const string& file_path, string content){
    ofstream outfile(file_path);
    if(outfile.is_open()){
        outfile << content;
    }
    else{
        printf("保存文件路径不存在");
        exit(1);
    }
}

// 恢复图像（这里假设传入的content是经过解压后的图像二进制数据，且之前压缩前是按照合理的图像格式存储的）
void recover_image(const string& file_path, string content) {
    // 简单判断是否有数据可用于恢复图像
    if (content.empty()) {
        std::cerr << "传入的图像数据为空，无法恢复图像" << std::endl;
        return;
    }

    // 以二进制模式打开要保存的文件
    ofstream out_image(file_path, ios::binary);
    if (!out_image.is_open()) {
        cerr << "无法打开文件用于保存恢复后的图像" << endl;
        return;
    }

    // 简单构造一个BMP文件头（这里只是示意，实际需严格按BMP格式规范来，长度等可能不准确）
    char header[54] = {
        0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00,
        0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // 将文件头写入文件
    out_image.write(header, 54);

    // 直接将解压后的图像数据写入文件（假设是简单的类似24位真彩色位图格式，每3个字节为一个像素的RGB值顺序排列）
    out_image.write(content.c_str(), content.size());

    // 关闭文件
    out_image.close();
}

// test
int main(){
    string file_path = "data/test_origin.txt";
    save_origin(file_path, "Me");
    return 0;
}