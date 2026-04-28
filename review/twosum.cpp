#include<bits/stdc++.h>
#include<vector>
using namespace std;
class twosum
{
private:
    /* data */
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        vector<int> ans;
        for(int i=0;i<nums.size();i++){
            // n = target-nums[i];
            for(int j=i+1;j<nums.size();j++){
                if(nums[j]== target-nums[i]){
                    ans.push_back(i);
                    ans.push_back(j);
                    return ans;
                }
            }
        }
        return ans;
    }
};
int main(){
    vector<int> nums;
    int n, val, target;
    cin >> n;
    for(int i = 0; i < n; i++){
        cin >> val;
        nums.push_back(val);
    }
    cin >> target;
    twosum solution;
    vector<int> result = solution.twoSum(nums, target);
    for(int i : result){
        cout << i << " ";
    }
    return 0;
}

