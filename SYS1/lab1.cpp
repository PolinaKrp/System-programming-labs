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
    int flagA = mkdir("A", S_IRWXU);
            std::cout << "/A was created" << std:: endl;
        int fd1 = open("A/1.txt", O_CREAT | O_WRONLY, S_IRWXU);
        char array1[256];
        for(int i = 0; i < sizeof(array1); i++){
            array1[i] = '0' + rand()%8;
        }
        int elem = write(fd1, array1, sizeof(array1));
        if(elem != -1){
            std::cout << "data was written in A/1.txt" << std::endl;
        }
        close(fd1);
        int fd2 = open("A/2.txt", O_CREAT | O_WRONLY, S_IRWXU);
        char array2[256];
        for(int i = 0; i < sizeof(array2); i++){
            array2[i] = '1';
        }
        int elem2 = write(fd2, array2, sizeof(array2));
        if(elem2 != -1){
            std::cout << "data was written in A/2.txt" << std::endl;
        }
        close(fd2);
    int flagB = -1, flagC = -1, flagD = -1;


    if(flagA == 0){
        chdir("A");
        flagB = mkdir("B", S_IRWXU);
        std::cout << "/B was created" << std::endl;
        int fd3 = open("B/3.txt", O_CREAT | O_WRONLY, S_IRWXU);
        char array1[256];
        for(int i = 0; i < sizeof(array1); i++){
            array1[i] = '0';
        }
        int elem = write(fd3, array1, sizeof(array1));
        if(elem != -1){
            std::cout << "data was written in B/3.txt" << std::endl;
        }
        close(fd3);


        if(flagB == 0){
            chdir("B");
            flagC = mkdir("C", S_IRWXU);
        }
    }

    if(flagC == 0) {
        std::cout << "/C was created" << std::endl;
         chdir("C");

         int flagSymlink = symlink("../A/1.txt", "4.txt");
         if(flagSymlink == 0){
             std::cout << "Symlink was created" << std::endl;
         }

        //  int flagLink = link("../A/1.txt", "5.txt");
        //  if(flagLink == 0){
        //      std::cout << "hardlink was created" << std::endl;
        //  }

        int flagLink1 = link("A/1.txt", "5.txt");
    
        if(flagLink1 == 0) {
            std::cout << "Жесткая ссылка была создана" << std::endl;
        } else {
            std::cerr << "Ошибка при создании жесткой ссылки" << std::endl;
        }
    }
}