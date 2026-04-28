#include<bits/stdc++.h>

using namespace std;

class palindrome_number
{
private:
    /* data */
public:
    palindrome_number(/* args */);
    ~palindrome_number();
};

palindrome_number::palindrome_number(/* args */)
{



}

palindrome_number::~palindrome_number()
{

}

int main(){
    int x;
    cin >> x;
    if(x < 0) {
        cout << "false";
        return 0;
    }
    string s = to_string(x);
    string rev_s = s;
    reverse(rev_s.begin(), rev_s.end());
    if(s == rev_s) {
        cout << "true";
    } else {
        cout << "false";
    }
    return 0;
}