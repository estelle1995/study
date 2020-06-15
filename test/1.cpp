#include <iostream>

using namespace std;

typedef struct Node
{
  unsigned int age : 3;
} Age;

int main() {
    int a;
    cout << "hello" << endl;
    Age *age = new Age();
    age->age = 3;
    cout << age << endl;
    cin >> a;
    return 0;
}