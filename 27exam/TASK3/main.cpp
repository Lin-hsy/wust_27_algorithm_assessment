#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <tuple>
#include <cmath>
#include <climits>
using namespace std;

int R,C;
int dr[4]={-1,1,0,0};
int dc[4]={0,0,-1,1};

int DFS(vector<vector<bool>>& visited,vector<string> &grid,int sr,int sc)
{ 
    
    if(sr<0||sc<0||sr>=R||sc>=C||grid[sr][sc]=='#') return 0;
    if(visited[sr][sc]) return 0;
    visited[sr][sc]=true;
    int area=1;
    for(int i=0;i<4;i++)
    {
        int nr=sr+dr[i];
        int nc=sc+dc[i];
        area+=DFS(visited,grid,nr,nc);
    }
    return area;
}

int A(vector<string>&grid,int sr,int sc,int er,int ec)
{
    vector<vector<int>> g(R,vector<int>(C,INT_MAX));
    priority_queue<tuple<int,int,int>,
                    vector<tuple<int,int,int>>,
                    greater<tuple<int,int,int>>> q;
    int start_h=abs(sr-er)+abs(sc-ec);
    g[sr][sc]=0;
    q.push({start_h+0,sr,sc});
    while(!q.empty())
    {
        auto[f,r,c]=q.top();
        q.pop();
        //是否到终点
        if(r==er&&c==ec) return g[r][c];
        //筛选过期数据
        int h=abs(r-er)+abs(c-ec);
        if(f>g[r][c]+h) continue;
        //四个方向移动
        for(int i=0;i<4;i++)
        {
            int nr=r+dr[i];
            int nc=c+dc[i];
            if(nr<0||nr>=R||nc<0||nc>=C||grid[nr][nc]=='#') continue;
            int cost;
            if(grid[nr][nc]=='S'||grid[nr][nc]=='G'||grid[nr][nc]=='.') cost=1;
            else cost=grid[nr][nc]-'0';
            int new_g=g[r][c]+cost;
            if(new_g<g[nr][nc])
            {
                g[nr][nc]=new_g;
                int h=abs(nr-er)+abs(nc-ec);
                q.push({g[nr][nc]+h,nr,nc});
            }
        }
    }
    return -1;
}

int main()
{
    int area=0,count=0;
    int sr=-1,sc=-1,er=-1,ec=-1;
    //输入数据
    cin>>R>>C;
    vector<string> grid(R);
    vector<vector<bool>> visited(R,vector<bool>(C,false));
    vector<vector<bool>> visited2(R,vector<bool>(C,false));
    for(int i=0;i<R;i++)
    {
        cin>>grid[i];
    }
    //找起点终点
    for(int i=0;i<R;i++)
    {
        for(int j=0;j<C;j++)
        {
            if(grid[i][j]=='S')
            {
                sr=i;
                sc=j;
            }
            if(grid[i][j]=='G')
            {
                er=i;
                ec=j;
            }
        }
    }
    //起点出发可通行格子数
    area=DFS(visited,grid,sr,sc);
    //可通行块数
    for(int i=0;i<R;i++)
    {
        for(int j=0;j<C;j++)
        {
            if(grid[i][j]!='#'&&!visited2[i][j]) 
            {

                DFS(visited2,grid,i,j);
                count++;
            }

        }
    }
    
    int cost=A(grid,sr,sc,er,ec);
    cout<<"reachable_cells:"<<area<<endl
        <<"regions:"<<count<<endl
        <<"cost:"<<cost<<endl;
             return 0;
}
