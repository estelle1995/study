#include<stdio.h>
#include<stdlib.h>
char sign[26];
char char_sign[256];
int check(const char*a) {
    int len = strlen(a);
    if (len == 0) return -1;
    int index = 0;
    if (*a == '+' || *a == '-') index++;
    if (index >= len) return -1;
    for (int i = 0; i < len; i++) {
        if (char_sign[a[i]] == 256) return -1;
    }
    return 1;
}

void Num52Add2(const char*a, const char*b, char*c, int clen) { 
    int alen = strlen(a), blen = strlen(b);
    int floop = 0;
    while (alen > 1 || blen > 1) {
        int temp = floop;
        if (alen > 1) temp += char_sign[a[--alen]];
        if (blen > 1) temp += char_sign[b[--blen]];
        c[clen--] = sign[temp % 52];
        floop = temp / 52;
    }
}

void Num52Del2(const char*a, const char*b, char*c, int clen) {
    int alen = strlen(a), blen = strlen(b);
    int floop = 0;
    while (alen > 1 || blen > 1) {
        int temp = 0;
        if (alen > 1) temp = char_sign[a[--alen]];
        if (blen > 1) temp -= char_sign[b[--blen]];
        temp -= floop;
        if (temp < 0) { floop = 1; temp += 52;}
        else floop = 0;
        c[clen--] = sign[temp]; 
    }
}

int compare_abs2(const char*a, const char*b) {
    int a_l = 0, a_len = strlen(a), b_l = 0, b_len = strlen(b);
    for (int i = 0; i < a_len; i++) if (a[i] == 'a') a_l++; else break;
    for (int i = 0; i < b_len; i++) if (b[i] == 'a') b_l++; else break;
    a_len -= a_l; b_len -= b_l;
    if (a_len > b_len) return 1;
    if (a_len < b_len) return -1;
    for (int i = a_len; i < a_len; i++) {
        if (a[i] == b[b_l++]) continue;
        return char_sign[a[i]] - char_sign[b[b_l - 1]];
    }
}

int compare_abs(const char*a, const char*b) { 
    if (*a == '-' || *a == '+') a++;
    if (*b == '-' || *b == '+') b++;
    return compare_abs2(a, b);
}

int tyde(char *c, char *c2, int n, int flag) { 
    int clen = n;
    for (int i = 0; i < n; i++) {
        if (c2[i] == 'a') clen--;
        else break;
    }
    if (clen == 0) {
        c = (char*)malloc(sizeof(char) * 2);
        c[0] = 'a'; c[1] = '\0';
        return 0;
    } 
    int k;
    if (flag == -1) {
        c = (char*)malloc(sizeof(char) * (clen + 2)); 
        c[clen + 1] = '\0'; k = clen; 
        c[0] = '-';
    }
    else {
        c = (char*)malloc(sizeof(char) * (clen + 1)); 
        c[clen] = '\0'; 
        k = clen - 1;
    }
    for (int i = 0; i < clen; i++) {
        c[k--] = c2[--n];
    }
    free(c2);
    while (*c != '\0') {
        printf("%c", *c); c++;
    }
    return clen;
}

int Num52Add(const char*a, const char*b, char *c, int cLen) {
    //sign可以直接根据数字获得字符
    for (int i = 0; i < 26; i++) sign[i] = 'a' + i;
    for (int i = 26; i < 52; i++) sign[i] = 'A' + i - 26;
    //char_sign可以直接根据字符获得整数
    for (int i = 0; i < 256; i++) {
        char_sign[i] = 256;
        if (i >= 'a' && i  <= 'z') char_sign[i] = i - 'a';
        if (i >= 'A' && i <= 'Z') char_sign[i] = i - 'A' - 26;
    }
    //首先检查字符串是否为正确数字。
    if (check(a) < 0) return -1;
    if (check(b) < 0) return -1;
    int alen = strlen(a), blen = strlen(b), flag = 1;
    int n = alen > blen ? alen + 2 : blen + 2;
    //开辟一个辅助字符串空间c2，c2的存储从第 n - 1 位开始往前存储。
    char *c2 = (char *)malloc(sizeof(char) * n);
    //将c2所有字符置a
    for (int i = 0; i < n; i++) c2[i] = 'a';
    //判断两个字符串绝对值相加还是相减
    if ((*a == '-' && *b == '-') || (*a != '-' && *b != '-')) {
        //获得最后结果的正负值
        if (*a == '-' && *b == '-') { flag = -1;}
        if (*a == '-' || *a == '+') a++;
        if (*b == '-' || *b == '+') b++;
        //除去符号位两个字符串相加  
        Num52Add2(a, b, c2, n - 1);
    } else {
        //判断绝对值计算结果是 a - b, 还是 b - a;
        if (compare_abs(a, b) > 0) {
            //获得最后结果的正负值
            if (*a == '-') flag = -1; 
            if (*a == '-' || *a == '+') a++;
            if (*b == '-' || *b == '+') b++;  
            Num52Del2(a, b, c2, n - 1);
        } else {
            if (*b == '-') flag = -1;
            if (*a == '-' || *a == '+') a++;
            if (*b == '-' || *b == '+') b++;  
            Num52Del2(b, a, c2, n - 1);
        }
    }
    //整理字符串。将c2中从开始值不为0的字符复制到c中，并释放c2空间。
    int clen2 = n, c2_k;
    for (c2_k = 0; c2_k < n; c2_k++) {
        if (c2[c2_k] == 'a') clen2--;
        else break;
    }
    if (clen2 == 0) {
        if (cLen < 2) return -1;
        c[0] = 'a'; c[1] = '\0';
        return 0;
    }
    if (cLen < clen2 + 1) return -1;
    if (flag == -1 && cLen < clen2 + 2) return -1;
    int k = 0;
    if (flag == -1) {c[k++] = '-';}
    for (int i = 0; i < clen2; i++) c[k++] = c2[c2_k++];
    free(c2);
    return clen2;
}

int main() {
    char *c = (char *)malloc(sizeof(char) * 10) ;
    if (Num52Add("-abcA", "ac", c, 10) >= 0) {
        while (*c) {
            printf("%c", *(c++));
        }
    }
    return 0;
}