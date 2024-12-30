#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <sstream>
using namespace std;

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


// 保存内容到文件
void save_content(const string& file_path, string content){
    ofstream outfile(file_path);
    if(outfile.is_open()){
        outfile << content;
    }
    else{
        printf("保存文件路径不存在!");
        exit(0);
    }
}


// 字符串到json
string convertToJSON(const string& origin_content, const string& encodingTableString, const string& encodedContent, bool compress_mode=false) {
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
            if(character=="\""){
                character = "\\\"";
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
    if(compress_mode){
        jsonString += "  \"encoded_content\": \"" + encodedContent + "\"\n";
        jsonString += "}";
        return jsonString;
    }
    jsonString += "  \"encoded_content\": \"" + encodedContent + "\",\n";
    string refined_content = "";
    for(size_t i=0;i<origin_content.length();++i){
        string character = string(1, origin_content[i]);
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
        if(character=="\""){
            character = "\\\"";
        }
        refined_content += character;
    }
    jsonString += "  \"origin_content\": \"" + refined_content + "\"\n";
    jsonString += "}";
    cout << "inner\n" <<jsonString << endl;
    return jsonString;
}

bool fileExists(const string& file_name) {
    ifstream file(file_name);
    return file.good(); // 检查文件是否打开成功
}
// 保存json字符串到json文件
void save_json(const string& json_string, const string& file_name){
    string txt_file_path = "data/json/" + file_name + ".json";
    if(fileExists(txt_file_path)){
        save_content(txt_file_path, json_string);
    }
    else{
        fstream newfile(txt_file_path, ios::out);
        if(newfile){
            save_content(txt_file_path, json_string);
        }
        else{
            printf("创建文件失败！\n");
            exit(0);
        }
    }
}
//保存字符到txt源码文件
void save_txt(const string& origin_string, const string& file_name){
    string txt_file_path = "data/" + file_name + ".txt";
    if(fileExists(txt_file_path)){
        save_content(txt_file_path, origin_string);
    }
    else{
        fstream newfile(txt_file_path, ios::out);
        if(newfile){
            save_content(txt_file_path, origin_string);
        }
        else{
            printf("创建文件失败！\n");
            exit(0);
        }
    }
}

string refined_string2json(const string& origin){
    string refined_content = "";
    for(size_t i=0;i<origin.length();++i){
        string character = string(1, origin[i]);
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
        if(character=="\""){
            character = "\\\"";
        }
        refined_content += character;
    }
    return refined_content;
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