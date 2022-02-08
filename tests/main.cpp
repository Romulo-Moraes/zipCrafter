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

    crafter.openZip();

    auto opa = crafter.listDirectory("example.txt");  
};