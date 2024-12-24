<<<<<<< Updated upstream
#include<iostream>
#include<string>
using namespace std;
//我直接将它做成一个管理系统，是针对于字符串的管理系统，因为我们无论是存储图片还是文本，最后都将其转化为字符串进行
//哈夫曼压缩了，这个管理系统实现了对于字符串的增，删，改，查功能，将题目中的查询和更新全部实现
int repeat(int i, int j, int next[], string in_test){
    if(i== 0){
        return 0;
    }else{
        i--;
        i= next[i];
        if(in_test[i]== in_test[j]){
            return i;
        }else{
            i= repeat(i, j, next, in_test);
            return i;
        }
    }
}
int search(string test, string in_test){
    int next[in_test.size()]= {0};
    int i=0, j=1;
    while(j<= in_test.size()){
        if(in_test[j]== in_test[i]){
            next[j]= i+1;
            i++;
            j++;
        }else{
            if(i== 0){
                j++;
            }else{
                i= repeat(i, j, next, in_test);
            }
        }
    }
    // for(i=0; i<in_test.size(); i++){
    //     cout<<next[i]<<" ";
    // }
    i=0, j=0;
    int judge= 0;
    while(j<= test.size()){
        if(i== in_test.size()){
            judge= 1;
            break;
        }else{
            if(test[j]== in_test[i]){
                i++;
                j++;
            }else{
                if(i== 0){
                    j++;
                }else{
                    i= next[i-1];
                    if(test[j]== in_test[i]){
                        i++;
                    }
                    j++;
                }
            }
        }
    }
    //cout<<j;
    if(judge== 0){
        cout<<"不存在";
        return 0;
    }else{
        cout<<"存在";
    }
    return (j-in_test.size());
}
string add(string test, string new_test, string in_test){
    //printf("请输入想插入字符串在谁的后面： ");
    int number= search(test, in_test);
    string part1= test.substr(0, number);
    string part2= test.substr(number);
    string end_test= part1+new_test+part2;
    return end_test;
}
string re_move(string test, string in_test){
    int number= search(test, in_test);
    string part1= test.substr(0, number);
    string part2= test.substr(number+in_test.size());
    string end_test= part1+part2;
    return end_test;
}
int main()
{
    string test= "abxabcabcaby";
    string in_test= "abcaby";
    cout<<"请输入想进行的操作1.查找 2.增加 3.删除"<<endl;
    int choice;
    cin>>choice;
    if(choice==1){
        search(test, in_test);
    }else if(choice==2){
        string new_test;
        cin>>new_test;
        test= add(test, new_test, in_test);
        cout<<test<<endl;
    }else if(choice==3){
        test= re_move(test, in_test);
        cout<<test<<endl;
    }
    return 0;
}
=======
#include<iostream>
#include<string>
using namespace std;
//我直接将它做成一个管理系统，是针对于字符串的管理系统，因为我们无论是存储图片还是文本，最后都将其转化为字符串进行
//哈夫曼压缩了，这个管理系统实现了对于字符串的增，删，改，查功能，将题目中的查询和更新全部实现
int repeat(int i, int j, int next[], string in_test){
    //repeat函数主要用来递归计算next数组，就是采用最常规的next数组的计算方法
    if(i== 0){
        return 0;
    }else{
        i--;
        i= next[i];
        if(in_test[i]== in_test[j]){
            return i;
        }else{
            i= repeat(i, j, next, in_test);
            return i;
        }
    }
}
string search(string test, string in_test){
    int next[in_test.size()];//通过要匹配的字符串的长度来设置next数组的大小
    for(size_t i=0;i<in_test.size();++i){
        next[i] = 0;//将其初始化为0
    }
    int i=0, j=1;
    //计算next数组
    while(j<= in_test.size()){
        if(in_test[j]== in_test[i]){
            next[j]= i+1;
            i++;
            j++;
        }else{
            if(i== 0){
                j++;
            }else{
                i= repeat(i, j, next, in_test);
            }
        }
    }
    i=0, j=0;
    int judge= 0;
    string posion= "";
    //以下使用kmp匹配算法
    while(j<= test.size()){
        if(i== in_test.size()){//j去遍历test数组，i去遍历in_test数组，当i==in_test.size()时，说明完全匹配上了
            judge++;//judge用来计数在当前test字符串中有多少个与in_test相同的字符串
            posion+= to_string(j-in_test.size());//position用来计数in_test字符串出现在test中的哪里，将数字以字符串的形式存储起来  
            posion+="_";//每个位置之间用_隔开
            i= 0;//将i清空，开始下一次匹配
        }else{
            if(test[j]== in_test[i]){//当字符相等时，匹配下一个
                i++;
                j++;
            }else{//不相等时
                if(i== 0){//根据kmp算法，如果已经退回至in_test头部，则开始匹配test的下一个字符
                    j++;
                }else{
                    i= next[i-1];//根据next数组，跳转至相应的in_test位置进行比较
                    if(test[j]== in_test[i]){//相等时匹配下一个
                        i++;
                    }
                    j++;
                }
            }
        }
    }
    posion+= to_string(judge);//原position中的信息为“数字_……数字_，最后给它加上这里面一共有多少个位置，即多少个数字
    return posion;//将这个包含了所有位置和位置个数的字符串返回作为搜索的结果
}
int count_num_of_same(string test, string in_test){//该函数用来截取position最后的位置个数，将其作为数字返回(主要是用来构建数组大小)
    int j=0, k=0;
    string posion= search(test, in_test);
    while(posion[j]!= '\0'){
        if(posion[j]== '_'){
            k= j;
        }
        j++;
    }
    if(k== 0){
        return 0;
    }else{
        string num_of_posion= posion.substr(k+1);//截取个数字符串
        return (stoi(num_of_posion));//将其转变为数字返回
    }
}
void posion_of_number(int *p, string test, string in_test){//该函数用于将position中的位置转换为数字存储到数组p中
    string posion= search(test, in_test);
    string temp_posion= "";
    int j=0,k= 0;
    while(posion[j]!= '\0'){
        if(posion[j]!= '_'){//以“_”为界限进行切割
            temp_posion+= posion[j];
        }else{
            p[k]= stoi(temp_posion);//将字符串转化为数字进行保存
            k++;
            temp_posion= "";//还原字符串为空
        }
        j++;
    }
}
string get_total_part(string test, string in_test){//为了以实际情况相结合以及方便用户使用，会将搜索到的in_test字符串在原test字符串中的前后5个字符进行输出，方便用户知道在哪里进行修改
    int count= count_num_of_same(test, in_test);//
    int *p= new int(count);
    posion_of_number(p, test, in_test);//将位置录入到p数组中，以数字形式存储起来
    if(count== 0){
        return "";
    }else{
        //cout<<"存在"<<endl;
    }
    int j= 0;
    string part1, part2;
    string final_all= "";
    //以下是对test字符串中的边缘进行检测，如果in_test字符串靠近开头，可能会出现开头到in_test没有5个字符的可能，那么就是有多少输出多少，末尾也是如此
    for(j=0; j<count; j++){
        if(p[j]-5> 0){
            part1= test.substr(p[j]-5, 5);
            if(p[j]+5< test.size()){
                part2= test.substr(p[j]+in_test.size(), 5);
            }else{
                part2= test.substr(p[j]+in_test.size());
            }
        }else{
            part1= test.substr(0, p[j]);
            if(p[j]+5< test.size()){
                part2= test.substr(p[j]+in_test.size(), 5);
            }else{
                part2= test.substr(p[j]+in_test.size());
            }
        }    
        //cout<<j+1<<"."<<"\033[31m"<<part1<<" "<<"\033[33m"<<part2<<"\033[0m"<<endl;
        final_all+= (to_string(j+1)+ "."+ part1 +"("+in_test+")"+part2+ "\n");//加括号，使其更加醒目
    }
    cout<<final_all;
    return final_all;
}
// int main()
// {
//     string test= "adjasjdhaqabanqwqkejqkeabaadadad";
//     string in_test= "aba";
//     //对位置进行切割，找到该字符串出现的所有位置
    
//     cout<<"请输入想进行的操作1.查找 2.退出"<<endl;
//     string choice;
//     cin>>choice;
//     while(choice[0]!= '\0'){
//         if(choice== "1"){
//             string posion= search(test, in_test);
//             if(posion!= "" &&posion!= "0"){
//                 cout<<"存在"<<endl;
//             }else{
//                 cout<<"不存在"<<endl;
//             }
//             get_total_part(test, in_test);
//         }else if(choice=="2"){
//             break;
//         }
//         else{
//             cout<<"输入格式不对"<<endl;
//         }
//         cout<<"请输入想进行的操作1.查找2.退出"<<endl;
//         cin>>choice;
//     }
    
//     return 0;
// }
>>>>>>> Stashed changes
