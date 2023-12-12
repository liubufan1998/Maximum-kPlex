#ifndef _SBUNDLE_TOOL_H
#define _SBUNDLE_TOOL_H

#include <stdio.h>
#include <utility>
#include <fstream>
#include <bits/stdc++.h>
#include "sbundle_tool.h"

int twoPow[1 << 16], msk = (1 << 16) - 1;

struct BIT_SET 
{
    int n, m; 
    unsigned *buf; 
    BIT_SET() 
    {
        n = m = 0;
        buf = nullptr;
    }
    ~BIT_SET()
    {
        if (buf != nullptr)
            delete[] buf;
    }
    void init(int _n) 
    {    
        m = _n & 31;  
        n = _n >> 5; 
        buf = new unsigned[n + 1];
        for (int i = 0; i <= n; ++i)
            buf[i] = 0; 
    }
    void flip() 
    {
        for (int i = 0; i < n; ++i)
            buf[i] = ~buf[i]; 
        buf[n] ^= ((unsigned)1 << m) - 1;
    }
    void set(int x) 
    {
        buf[x >> 5] ^= (unsigned)1 << (x & 31);
    }
    bool test(int x) 
    {   
        return buf[x >> 5] >> (x & 31) & 1;
    }
    int lb(unsigned x) 
    {
        if (x & msk)   
            return twoPow[x & msk]; 
        return twoPow[x >> 16 & msk] + 16;
    }
    int lowbit() 
    {
        for (int i = 0; i <= n; ++i)
        {
            unsigned x = buf[i] & ((~buf[i]) + 1); 
            if (x) 
                return lb(x) + (i << 5);
        }
        return -1;
    }
    bool empty() 
    {
        for (int i = 0; i <= n; ++i)
            if (buf[i])
                return false;
        return true;
    }
    void operator&=(const BIT_SET &rhs) 
    {
        for (int i = 0; i <= n; ++i)
            this->buf[i] &= rhs.buf[i];
        
    }
    void cp(BIT_SET &rhs) const
    {  
        rhs.n = n;
        rhs.m = m;
        if (rhs.buf == NULL)
            rhs.buf = new unsigned[n + 1];
        for (int i = 0; i <= n; ++i)
            rhs.buf[i] = buf[i];
    }
} *adjN, *invN, inD, inG; 

int *head, *nxt, *to, etot; 
int *degree, *que, *nV, n;
bool *del, *ins; 
int *notadj;
int *up_bound, *ordID; 
int *dis; 

class MaximumFlow 
{
private:
    struct Node 
    {
        int from, to, next;
        int cap;
    } *edge; 
    int *cap_backup;
    int tol;
    int *head;
    int *dep;
    int *gap, *que;
    int *cur;
    int *S;
    int n;
    void BFS(int start, int end) 
    {  
        memset(dep, -1, n * sizeof(int)); 
        memset(gap, 0, n * sizeof(int)); 
        gap[0] = 1; 
        int front, rear; 
        front = rear = 0;
        dep[end] = 0; 
        que[rear++] = end; 
        while (front != rear) 
        {
            int u = que[front++]; 
            for (int i = head[u]; i != -1; i = edge[i].next) 
            {
                int v = edge[i].to; 
                if (dep[v] != -1) 
                    continue; 
                que[rear++] = v; 
                dep[v] = dep[u] + 1; 
                ++gap[dep[v]]; 
            }
        }
        
    }

public:
    
    void reserve(int n, int m) 
    {
        
        n += 3;
        edge = new Node[m * 4 + n * 2];      
        cap_backup = new int[m * 4 + n * 2]; 
        head = new int[n + n];               
        dep = new int[n + n];
        gap = new int[n + n];
        que = new int[n + n];
        cur = new int[n + n];
        S = new int[n + n];
    }

    void init(int _n) 
    {
        tol = 0;   
        n = _n + 2;  
        memset(head, -1, n * sizeof(int)); 
    }

    
    void addedge(int u, int v, int w) 
    {
       
        edge[tol].from = u;
        edge[tol].to = v;
        edge[tol].cap = w;
        cap_backup[tol] = w;
        edge[tol].next = head[u];
        head[u] = tol++;

        
        edge[tol].from = v;
        edge[tol].to = u;
        edge[tol].cap = 0;
        cap_backup[tol] = 0;
        edge[tol].next = head[v];
        head[v] = tol++;
    }

    int SAP(int start, int end) 
    {
        
        for (int i = 0; i < tol; ++i)
            edge[i].cap = cap_backup[i];
        int res = 0; 
        BFS(start, end); 
        
        int top = 0; 
        memcpy(cur, head, n * sizeof(int)); 
        int u = start; 
        int i;
        while (dep[start] < n) 
        {                      
            if (u == end)
            {
                
                int temp = INF + 1; 
                int inser = 0; 
                for (i = 0; i < top; ++i) 
                    if (temp > edge[S[i]].cap) 
                    {
                        temp = edge[S[i]].cap; 
                        inser = i;
                    }
                
                for (i = 0; i < top; ++i)
                {
                    edge[S[i]].cap -= temp; 
                    edge[S[i] ^ 1].cap += temp; 
                }
                res += temp; 
                top = inser;
                u = edge[S[top]].from; 
            }
            if (u != end && gap[dep[u] - 1] == 0) 
                break;
            for (i = cur[u]; i != -1; i = edge[i].next)  
                if (edge[i].cap != 0 && dep[u] == dep[edge[i].to] + 1) 
                    break;

            
            if (i != -1)
            {
                cur[u] = i; 
                S[top++] = i; 
                u = edge[i].to; 
            }
            else 
            {
                int min = n; 
                for (i = head[u]; i != -1; i = edge[i].next) 
                {
                    if (edge[i].cap == 0) 
                        continue;
                    if (min > dep[edge[i].to]) 
                    { 
                        min = dep[edge[i].to]; 
                        cur[u] = i; 
                    }
                }
                --gap[dep[u]]; 
                dep[u] = min + 1; 
                ++gap[dep[u]]; 
                
                if (u != start) 
                    u = edge[S[--top]].from; 
            }
        }
        return res; 
    }
} mf;

std::vector<int> svex; 
class VertexConnectivity
{
private:
    
    int *nV, *oID; 

public:
    
    void reserve(int n)
    {
        n += 3;
        nV = new int[n];
        oID = new int[n];
    }

    
    
    bool solve(bool cals, int bound) 
    {
        int nn = 0; 
        if (cals)   
        {
            for (auto u : svex)
                nV[u] = nn, oID[nn++] = u;        
            mf.init(nn + nn);                     
            for (auto u : svex)                   
                mf.addedge(nV[u], nV[u] + nn, 1); 
            for (auto u : svex)
            {
                for (auto v : svex)
                {
                    if (!adjN[u].test(v)) 
                        continue;
                    mf.addedge(nV[u] + nn, nV[v], INF); 
                }
            }
            int u = nV[svex.back()];     
            for (int i = 0; i < nn; ++i) 
            {
                if (i == u || adjN[oID[u]].test(oID[i])) 
                    continue; 
                if (mf.SAP(u + nn, i) < bound) 
                    return false;
            }
            return true; 
        }
        else 
        {
            for (int i = 0; i < n; ++i)
                if (!del[i])                          
                    nV[i] = nn, oID[nn++] = i;        
            mf.init(nn + nn);   
           
            for (int i = 0; i < n; ++i)               
                if (!del[i])                          
                    mf.addedge(nV[i], nV[i] + nn, 1); 
            for (int i = 0; i < n; ++i)               
            {
                if (del[i]) 
                    continue;
                for (int e = head[i]; ~e; e = nxt[e]) 
                {
                    int v = to[e]; 
                    if (del[v])    
                        continue;
                    mf.addedge(nV[i] + nn, nV[v], INF); 
                }
            }
            
            
            for (int i = 0; i < nn; ++i)         
                for (int j = i + 1; j < nn; ++j) 
                     
                    if (!adjN[oID[i]].test(oID[j]) && mf.SAP(nn + i, j) < bound)
                     
                        return false;
            return true;
        }
    }
} vc;

#endif