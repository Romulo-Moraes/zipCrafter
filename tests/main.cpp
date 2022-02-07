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

    zip *z = crafter.getLibzipFilePointer();

    cout << zip_get_name(z,0,0);
};