//
//  main.cpp
//  1.8Is_Rotation
//
//  Created by Zhihong Ren on 2/28/16.
//  Copyright © 2016 Zhihong Ren. All rights reserved.
//

#include <iostream>
#include <string>
using namespace std;
bool is_rotation(string s, string t){
    int len1=s.size(),len2=t.size();
    if (len1==len2&&len1>0) {
        string temp=s+s;
        return isSubstring(temp,t);
    }
    return false;
}
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
