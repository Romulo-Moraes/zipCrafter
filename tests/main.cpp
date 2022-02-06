#include <iostream>
#include <algorithm>
#include <vector>
#include "./../zipCraft/zipCraftHeader.hpp"

using namespace std;

int main(){
    
    zipCrafter crafter("package.zip");

    crafter.listDirectory("");

    crafter.closeZip();
    
};