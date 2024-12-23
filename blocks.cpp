#include <string>
using namespace std;
// 哈夫曼静态二叉树节点
typedef struct Node {
    int weight;     // 权重（字符出现的次数）
    int parent;     // 父节点索引
    int left;       // 左孩子索引
    int right;      // 右孩子索引
    // 初始化
    Node(): weight(0), parent(-1), left(-1), right(-1){}
}Node;

// 哈夫曼表模块
typedef struct TableBlock{
    string key; // 字符
    string value; //字符对应的哈夫曼编码值
    // 初始化
    TableBlock(): key(""),value(""){}
}TableBlock;