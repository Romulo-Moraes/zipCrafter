#include <iostream>
#include <algorithm>
#include <vector>
#include <readline/readline.h>
#include "./../zipCraft/zipCraftHeader.hpp"
#include <string.h>

using namespace std;

int main(){
    char *userInput;
    cout << "We need find the zip file password, you can do it?" << endl;

    zipCrafter crafter("package.zip");

    crafter.openZip();

    cout << crafter.checkIfEncrypted() << endl;

    while(true){
        userInput = readline("Type some password: ");
        char buffer[3];
        if(crafter.readEncryptedFileByIndex(0,userInput,2,buffer)){
            cout << "Well done, you found the password!" << endl;
            cout << buffer;
            exit(0);
        }
        else{
            cout << "Wrong password, try again" << endl << endl;
        }
    };
};