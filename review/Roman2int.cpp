#include<iostream>
#include<unordered_map>

using namespace std;

class roman2int{
    private:
        /* data */
    public:
        int romanToInt(string s) {
            unordered_map<char, int> m = {
                {'I', 1},
                {'V', 5},
                {'X', 10},
                {'L', 50},
                {'C', 100},
                {'D', 500},
                {'M', 1000}
            };
            int sum = 0;
            for (int i = 0; i < s.size(); i++){
                if (i > 0 && m[s[i]] > m[s[i-1]]){
                    sum += m[s[i]] - m[s[i-1]];
                } else {
                    sum += m[s[i]];
                }
            }
            return sum;
        }

};


int main(){
    string s;
    cin >> s;
    roman2int r2i;
    cout << r2i.romanToInt(s);
    return 0;
}
