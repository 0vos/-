#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <sstream>
using namespace std;

// 获取图片文件信息
string get_image(const string& file_path){
    // TODO: 将png或jpg图片的二进制内容转到字符串中（图像转成字符串，字符串进行压缩）
    return string();
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
void get_huff(const string& file_path, string* output_list){
    // TODO: output_list[0]为哈夫曼表字符串，output_list[1]为剩余的“0”“1”构成的字符串
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
}

// 字符串到json
string convertToJSON(const string& encodingTableString, const string& encodedContent) {
    // 初始化 JSON 字符串
    string jsonString = "{\n";
    
    // 解析编码表字符串
    jsonString += "  \"encoding_table\": {\n";
    istringstream stream(encodingTableString);
    string pair;
    
    // 解析每个“字符_编码”对
    bool first = true;
    while (getline(stream, pair, ' ')) {
        size_t delimiterPos = pair.find('_');
        if (delimiterPos != string::npos) {
            string character = pair.substr(0, delimiterPos);  // 字符
            if(character=="\n"){
                character = "\\n";
            }
            if(character=="\r"){
                character = "\\r";
            }
            if(character=="\\"){
                character = "\\\\";
            }
            if(character=="\t"){
                character = "\\t";
            }
            string code = pair.substr(delimiterPos + 1);      // 编码
            
            if (!first) {
                jsonString += ",\n"; // 不是第一个元素，加上逗号
            }
            jsonString += "    \"" + character + "\": \"" + code + "\"";
            first = false; // 第一个元素后不再加逗号
        }
    }
    
    jsonString += "\n  },\n";
    jsonString += "  \"encoded_content\": \"" + encodedContent + "\"\n";
    jsonString += "}";
    cout << "inner" <<jsonString << endl;
    return jsonString;
}
// test
// int main(){
//     string encodingTableString = "A_0 B_10 C_111";
//     string encodedContent = "010111";

//     // 转换为 JSON 格式
//     string jsonOutput = convertToJSON(encodingTableString, encodedContent);

//     // 输出 JSON 字符串
//     cout << "JSON Output:\n" << jsonOutput << endl;

//     return 0;
// }