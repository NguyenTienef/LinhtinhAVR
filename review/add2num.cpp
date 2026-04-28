#include<bits/stdc++.h>
#include<vector>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class add2num
{


};

int main(){
    ListNode* l1 = new ListNode(2);
    l1->next = new ListNode(4);
    l1->next->next = new ListNode(3);

    ListNode* l2 = new ListNode(5);
    l2->next = new ListNode(6);
    l2->next->next = new ListNode(4);

    add2num solution;
    // ListNode* result = solution.addTwoNumbers(l1, l2);
    // while(result != nullptr){
    //     cout << result->val << " ";
    //     result = result->next;
    // }
    return 0;
}