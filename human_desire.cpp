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
    int next[in_test.size()];
    for(size_t i=0;i<in_test.size();++i){
        next[i] = 0;
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
    while(j<= test.size()){
        if(i== in_test.size()){
            judge++;
            posion+= to_string(j-in_test.size());
            posion+="_";
            i= 0;
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
    posion+= to_string(judge);
    return posion;
}
int count_num_of_same(string test, string in_test){
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
        string num_of_posion= posion.substr(k+1);
        return (stoi(num_of_posion));
    }
}
void posion_of_number(int *p, string test, string in_test){
    string posion= search(test, in_test);
    string temp_posion= "";
    int j=0,k= 0;
    while(posion[j]!= '\0'){
        if(posion[j]!= '_'){
            temp_posion+= posion[j];
        }else{
            p[k]= stoi(temp_posion);
            k++;
            temp_posion= "";
        }
        j++;
    }
}
string get_total_part(string test,  string in_test){
    int count= count_num_of_same(test, in_test);
    int *p= new int(count);
    posion_of_number(p, test, in_test);
    if(count== 0){
        return "";
    }else{
        cout<<"存在"<<endl;
    }
    int j= 0;
    string part1, part2;
    string final_all= "";
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
        final_all+= (to_string(j+1)+ "."+ part1 +"("+in_test+")"+part2+ "\n");
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