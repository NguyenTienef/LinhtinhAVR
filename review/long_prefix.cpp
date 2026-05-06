#include<bits/stdc++.h>

using namespace std;

class long_prefix{
public:
    string longestCommonPrefix(vector<string>& strs) {
        if (strs.empty()) return "";
        string prefix = strs[0];
        for (int i = 1; i < strs.size(); i++) {
            while (strs[i].find(prefix) != 0) {
                prefix = prefix.substr(0, prefix.size() - 1);
                if (prefix.empty()) return "";
            }
        }
        return prefix;
    }
};

int main(){
    long_prefix lp;
    vector<string> strs = {"flower", "flow", "flight"};
    cout << lp.longestCommonPrefix(strs);
    return 0;
}