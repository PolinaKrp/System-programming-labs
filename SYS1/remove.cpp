#include<stdio.h>
#include<string>
#include<stdlib.h>
#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<math.h>
#include<cstring>


int main(){
    chdir("A");
    chdir("B");
    chdir("C");
    int flagLink = unlink("5.txt");
    if(flagLink == 0){
        std::cout << "hard link was deleted"<<std::endl;
    }
    int flagSymLink = unlink("4.txt");
    if(flagSymLink == 0){
        std::cout << "symlink was deleted"<<std::endl;
    }
    chdir("../");
    int flag = rmdir("C");
    if(flag == 0){
        std::cout << "/C was deleted"<<std::endl;
    }
    chdir("../");
    int flagFile = remove("B/3.txt");
    if(flagFile== 0){
        std::cout << "3.txt was deleted"<<std::endl;
    }
    flag = rmdir("B");
    if(flag == 0){
        std::cout << "/B was deleted"<<std::endl;
    }
    chdir("../");
    flagFile = remove("A/1.txt");
    if(flagFile== 0){
        std::cout << "1.txt was deleted"<<std::endl;
    }
    flagFile = remove("A/2.txt");
    if(flagFile== 0){
        std::cout << "2.txt was deleted"<<std::endl;
    }
    flag = rmdir("A");
    if(flag == 0){
        std::cout << "/A was deleted"<<std::endl;
    }
}