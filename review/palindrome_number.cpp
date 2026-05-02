#include<bits/stdc++.h>

using namespace std;

class palindrome_number
{
private:
    /* data */
public:
    /* bool isPalindrome(int x) {
        if(x < 0) {
            return false;
        }
        string s = to_string(x);
        string rev_s = s;
        reverse(rev_s.begin(), rev_s.end());
        return s == rev_s;
    }*/
    bool isPalindrome(int x){
        if (x < 0){
            return false;
        }
        int original = x;
        long rn = 0;
        while (x > 0){
            rn = rn*10 + x%10;
            x = x/10;
        }
        return rn == original;    
    }
};

int main(){
    int x;
    cin >> x;
    palindrome_number pn;
    if(pn.isPalindrome(x)) {
        cout << "true";
    } else {
        cout << "false";
    }
    return 0;
}