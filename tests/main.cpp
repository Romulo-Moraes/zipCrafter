#include <iostream>
#include <algorithm>
#include <vector>
#include <readline/readline.h>
#include "./../zipCraft/zipCraftHeader.hpp"
#include <string.h>
#include <zip.h>

using namespace std;

int main(){
    zipCrafter crafter("package.zip");

    crafter.createZip();

    crafter.writeFile("exmaple.txt","hello!",6);

    crafter.setPasswordToFileByIndex(0,AES_256_ENCRYPTION,":)");
    

};