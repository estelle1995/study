#include<cstdio>
#include<set>
#include<map>
#include<cstring>
using namespace std;
typedef long long   ll;
int x1[55],x2[55],y1[55],y2[55];
int mp[101][101];
int main()
{
    //freopen("input.txt","r",stdin);
    int n;
    set<int>sx,sy;
    map<int,int>mpx,mpy;
    scanf("%d",&n);
    for(int i=0;i<n;i++){scanf("%d",x1+i);sx.insert(x1[i]);}
    for(int i=0;i<n;i++){scanf("%d",y1+i);sy.insert(y1[i]);}
    for(int i=0;i<n;i++){scanf("%d",x2+i);sx.insert(x2[i]);}
    for(int i=0;i<n;i++){scanf("%d",y2+i);sy.insert(y2[i]);}
    int sp=0;
    for(set<int>::iterator it=sx.begin();it!=sx.end();it++)
        mpx[*it]=sp++;
    sp=0;
    for(set<int>::iterator it=sy.begin();it!=sy.end();it++)
        mpy[*it]=sp++;
    for(int i=0;i<n;i++)x1[i]=mpx[x1[i]];
    for(int i=0;i<n;i++)x2[i]=mpx[x2[i]];
    for(int i=0;i<n;i++)y1[i]=mpy[y1[i]];
    for(int i=0;i<n;i++)y2[i]=mpy[y2[i]];
    memset(mp,0,sizeof(mp));
    for(int i=0;i<n;i++)
        for(int x=x1[i];x<x2[i];x++)
            for(int y=y1[i];y<y2[i];y++)
                mp[x][y]++;
    int Max=0;
    for(int i=0;i<101;i++)
        for(int j=0;j<101;j++)
            if(mp[i][j]>Max)Max=mp[i][j];
    printf("%d\n",Max);
    return 0;
}