#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <zlib.h>
#include "encoder_decoder.cpp"
#include <sstream>
#include <cctype>

using namespace std;

// 判断文件是否是PNG或JPG格式，通过扩展名简单判断（可按需完善更严谨判断）
bool is_png_or_jpg(const string& file_path) {
    string::size_type dot_index = file_path.find_last_of('.');
    if (dot_index == string::npos) {
        return false;
    }
    string extension = file_path.substr(dot_index + 1);
    // 转换为小写字母方便比较（不区分大小写判断扩展名）
    for (char& c : extension) {
        c = tolower(c);
    }
    return (extension == "png" || extension == "jpg" || extension == "jpeg");
}

// 获取图片文件信息
string get_image(const string& file_path, string pair[]){
    // TODO: 将png或jpg图片的二进制内容转到字符串中（图像转成字符串，字符串进行压缩）
    if (!is_png_or_jpg(file_path)) {
        return "文件格式不正确，不是PNG或JPG格式";
    }
    ifstream file(file_path, ios::binary);
    if (!file) {
        return "无法打开文件";
    }
    string binary_content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    encode(binary_content, pair);
    return "图片处理成功，已将二进制内容转为字符串并进行哈夫曼编码压缩";
    //return string();
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
    //cout << content << endl;
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
    //cout << "content:" << content << "\nlength:" << content.length()<< endl;
    return content;
}

// 获取编码文件信息
void get_huff(const string& file_path, string* output_list){
    // TODO: output_list[0]为哈夫曼表字符串，output_list[1]为剩余的“0”“1”构成的字符串
    ifstream huffFile(file_path);
    if (!huffFile.is_open()) {
        printf("无法打开哈夫曼编码文件\n");
        exit(1);
    }

    string line;
    string tablePart = "";
    string codePart = "";
    bool isTable = true;
    while (getline(huffFile, line)) {
        if (line == "###") {
            isTable = false;
            continue;
        }
        if (isTable) {
            tablePart += line + "\n";
        }
        else {
            codePart += line;
        }
    }
    huffFile.close();

    output_list[0] = tablePart;
    output_list[1] = codePart;
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
void save_huff(const string& file_path, string* pair){
    // TODO: 把哈夫曼表和编码内容写道压缩文件中, pair是用encode生成出表字符串、编码字符串对
    ofstream outFile(file_path);
    if (outFile.is_open()) {
        outFile << pair[0] << "###" << pair[1] << endl;
        outFile.close();
    }
    else {
        printf("保存文件路径不存在");
        exit(1);
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

// 恢复图像
void recover_image(const string& file_path, string content){
    // TODO: 将解码后的图像的二进制码恢复到图像
    // 先获取文件扩展名，用于确定图像格式（这里简单根据扩展名判断，实际可更严谨判断）
    string::size_type dot_index = file_path.find_last_of('.');
    if (dot_index == string::npos) {
        cerr << "无效的文件路径，无法确定图像格式" << endl;
        return;
    }
    string extension = file_path.substr(dot_index + 1);
    for (char& c : extension) {
        c = tolower(c);
    }

    if (extension == "png") {
        ofstream out_file(file_path, ios::binary);
        if (!out_file) {
            cerr << "无法创建输出文件用于恢复图像: " << file_path << endl;
            return;
        }

        string pair[2];
        size_t table_end_index = content.find("###");
        if (table_end_index == string::npos) {
            cerr << "编码内容中无法找到哈夫曼表与编码数据的分隔标识，无法正确解码" << endl;
            out_file.close();
            return;
        }
        pair[0] = content.substr(0, table_end_index);
        pair[1] = content.substr(table_end_index + 1);

        string decoded_content = decode(pair);
        out_file.write(decoded_content.data(), decoded_content.size());
        out_file.close();
    }
    else if (extension == "jpg" || extension == "jpeg") {
        ofstream out_file(file_path, ios::binary);
        if (!out_file) {
            cerr << "无法创建输出文件用于恢复图像: " << file_path << endl;
            return;
        }

        string pair[2];
        size_t table_end_index = content.find("###");
        if (table_end_index == string::npos) {
            cerr << "编码内容中无法找到哈夫曼表与编码数据的分隔标识，无法正确解码" << endl;
            out_file.close();
            return;
        }
        pair[0] = content.substr(0, table_end_index);
        pair[1] = content.substr(table_end_index + 1);

        string decoded_content = decode(pair);
        out_file.write(decoded_content.data(), decoded_content.size());
        out_file.close();
    }
    else {
        cerr << "不支持的图像格式: " << extension << endl;
    }
}

// test
/*int main(){
    string file_path = "data/test_origin.txt";
    string pair[2];
    save_origin(file_path, get_image("data/okok.png", pair));
    //string magic= get_text("data/test_origin.txt");
    recover_image("data/okokok.png", get_text("data/test_origin.txt"));
    return 0;
}*/
int main() {
    // 1. 测试获取图片文件信息并编码
    string file_path_image = "data/okok.png";  // 替换为实际的PNG或JPG图片文件路径
    string encoded_pair_image[2];
    string process_result_image = get_image(file_path_image, encoded_pair_image);
    cout << process_result_image << endl;

    // 2. 测试保存哈夫曼编码信息到文件
    string file_path_huff = "data/encoded_image.txt";
    save_huff(file_path_huff, encoded_pair_image);

    // 3. 测试从文件获取编码信息
    string huff_info[2];
    get_huff(file_path_huff, huff_info);

    // 4. 测试恢复图像
    string recovered_file_path = "recovered_image.jpg";  // 定义恢复后的图像文件名，可按需修改
    recover_image(recovered_file_path, huff_info[1]);
    cout << "图像恢复完成，已保存为 " << recovered_file_path << endl;
    
    return 0;
}