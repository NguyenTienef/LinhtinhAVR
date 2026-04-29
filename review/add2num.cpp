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
    private:
        /* data */

    public:
        ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
            ListNode* dummyHead = new ListNode(0);
            ListNode* p = l1, *q = l2, *curr = dummyHead;
            int carry = 0;

            while (p != nullptr || q != nullptr) {
                int x = (p != nullptr) ? p->val : 0;
                int y = (q != nullptr) ? q->val : 0;
                int sum = carry + x + y;
                carry = sum / 10;
                curr->next = new ListNode(sum % 10);
                curr = curr->next;

                if (p != nullptr) p = p->next;
                if (q != nullptr) q = q->next;
            }

            if (carry > 0) {
                curr->next = new ListNode(carry);
            }

            return dummyHead->next;
        }
};

int main(){
    ListNode* l1 = new ListNode(1);
    l1->next = new ListNode(1);
    l1->next->next = new ListNode(9);

    ListNode* l2 = new ListNode(3);
    l2->next = new ListNode(5);
    l2->next->next = new ListNode(7);

    add2num solution;
    ListNode* result = solution.addTwoNumbers(l1, l2);
    while(result != nullptr){
        cout << result->val << " ";
        result = result->next;
    }
    return 0;
}