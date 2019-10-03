```c++
//leetcode第146题
class LRUCache {
private:
    struct DListNode {
        int key;
        int value;
        DListNode *next, *pre;
        DListNode(int x, int y): key(x), value(y), next(nullptr), pre(nullptr) {}
    };
    int nums = 0;
    int capacity = 0;
    unordered_map<int, DListNode*>m;
    DListNode *head = nullptr, *rear = nullptr;
    
public:
    LRUCache(int capacity) {
        this->capacity = capacity;
    }
    
    int get(int key) {
        unordered_map<int, DListNode*>::iterator iter = m.find(key);
        if (iter != m.end()) {
            DListNode *p = iter->second;
            if (p != head) {
                p->pre->next = p->next;
                if (p != rear)
                    p->next->pre = p->pre;
                else
                    rear = p->pre;
                p->next = head;
                head->pre = p;
                head = p;
            }
            return head->value;
        }
        return -1;
    }
    
    void put(int key, int value) {
        unordered_map<int, DListNode*>::iterator iter = m.find(key);
        if (iter != m.end()) {
            DListNode *p = iter->second;
            p->value = value;
            this->get(key);
            return;
        }
        while (m.size() >= capacity) {
            DListNode *p = rear;
            m.erase(p->key);
            rear = rear->pre;
            delete p;
            if (rear == nullptr) head = nullptr;
        }
        DListNode *p = new DListNode(key, value);
        if (head == nullptr) head = rear = p;
        else {
            p->next = head;
            head->pre = p;
            head = p;
        }
        m[key] = p;
    }
};
```

java中可以直接用LinkedHashMap实现