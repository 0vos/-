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