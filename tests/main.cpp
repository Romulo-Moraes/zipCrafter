#include <iostream>
#include <algorithm>
#include <vector>
#include "./../zipCraft/zipCraftHeader.hpp"

using namespace std;

int main(){
    
    zipCrafter crafter("packagee.zip");

    cout << crafter.checkIfIsFolderByIndex(0);
    
};