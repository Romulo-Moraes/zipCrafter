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

    crafter.writeFile("hello.txt",":)",2);
    crafter.writeFile("hello2.txt",":)",2);

    cout << crafter.getFileIndexByName("hello2.txt");

    crafter.closeZip();
    

};