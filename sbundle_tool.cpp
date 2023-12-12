#include <stdio.h>
#include <utility>
#include <fstream>
#include <bits/stdc++.h>
#include "sbundle_tool.h"

int twoPow[1 << 16], msk = (1 << 16) - 1;

int *head, *nxt, *to, etot; /*to数组是存储一条边的另外一个顶点；etot是一个全局变量，用于追踪邻接表中边的总数。*/
int *degree, *que, *nV, n;
bool *del, *ins; /*ins数组用来表示一个顶点是不是位于我们的解集顶点集合之中。*/
int *notadj;
int *up_bound, *ordID; /*这里的up_bound其实就是退化排序过程中每个顶点的core值。*/
int *dis;              /*距离数组，用来存放某个顶点和其他所有顶点的距离。该数组在bfs（广度优先搜索）函数里面被初始化。*/
BIT_SET *adjN, *invN, inD, inG;
void MaximumFlow::BFS(int start, int end) /*定义了一个用于最大流算法里面的广度优先搜索算法。这个算法的核心思想是找到一条从源点到汇点的最短路径，这条路径被称为“增广路径”，
并且在这条路径上可以增加最大的流量。通过遍历所有的邻接节点并更新它们的路径长度，我们可以找到从源点到汇点的最短路径。*/
{                                         /*其实这个算法的思想非常简单：就是先把汇点的路径长度设置为0（因为目标就是到达汇点，而汇点自己到自己肯定是0.。），然后找汇点的所有一跳邻居，然后路径+1；然后
                                      找1条邻居的邻居，路径继续+1（其实就是广度优先搜索的思想）；直到遍历完所有的顶点。*/
    memset(dep, -1, n * sizeof(int));     /*dep用于存储从源点到每个点的最短路径长度，初始化为-1表示还没有计算路径长度。*/
    memset(gap, 0, n * sizeof(int));      /*gap用于存储到达每个点的增广路径的数量，初始化为0。*/
    gap[0] = 1;                           /*由于从源点出发的第一条边必然存在于增广路径中（除非存在负权环），将0号位置设置为1。*/
    int front, rear;                      /*初始化队列的前端和后端指针*/
    front = rear = 0;
    dep[end] = 0;         /*为汇点设置路径长度为0*/
    que[rear++] = end;    /*将汇点添加到队列中*/
    while (front != rear) /*持续进行，直到队列为空，遍历完所有的顶点。*/
    {
        int u = que[front++];                            /*首先取出队列前端的一个节点u*/
        for (int i = head[u]; i != -1; i = edge[i].next) /*对于顶点u，遍历它的所有邻居顶点v*/
        {
            int v = edge[i].to;  /*取出其中一个邻居顶点，命名为v。*/
            if (dep[v] != -1)    /*如果 v 已经有了路径长度（即 dep[v] != -1）*/
                continue;        /*则跳过此节点*/
            que[rear++] = v;     /*否则，将 v 添加到队列尾部*/
            dep[v] = dep[u] + 1; /*并将v的路径长度设为u的路径长度加1*/
            ++gap[dep[v]];       /*对于每个新的路径长度 dep[v]，我们将相应的 gap[dep[v]] 值加1*/
        }
    }
    /*对于广度优先搜索，我们需要注意的是：对于非连通图，广度优先搜索可以访问所有与起始节点直接或间接相连的节点。但是，如果图是完全非连通的，
    即没有任何路径可以从源节点到达某些节点，那么这些节点将不会被访问。总的来说，广度优先搜索可以用于遍历非连通图，但需要注意它可能无法访问到所有节点。*/
}
void MaximumFlow::reserve(int n, int m) /*n 和 m 分别是顶点和边的数量。*/ /*reserve 方法是用于分配足够空间来存储图的边和顶点信息，这里专门用于最大流算法定制的reserve方法。*/
{
    /*这里进行了额外的空间分配（n += 3 和 m * 4 + n * 2），这可能是为了处理算法中的特定边界情况或是为了避免数组越界。*/
    n += 3;
    edge = new Node[m * 4 + n * 2];      /*edge 用于存储边信息*/
    cap_backup = new int[m * 4 + n * 2]; /*cap_backup 用于备份边的容量*/
    head = new int[n + n];               /*head 用于指示每个顶点的邻接链表的开始，*/
    dep = new int[n + n];
    gap = new int[n + n];
    que = new int[n + n];
    cur = new int[n + n];
    S = new int[n + n];
}

void MaximumFlow::init(int _n) /*init 方法初始化网络流的图结构*/
{
    tol = 0;                           /*tol 用于追踪边的数量的计数器*/
    n = _n + 2;                        /*n 被设置为 _n + 2，这里的+2是为了包括源点和汇点。*/
    memset(head, -1, n * sizeof(int)); /*head 数组被初始化为 -1，这可能表示每个顶点的邻接链表开始之前没有边。*/
}

/*addedge 方法用于向基于最大流的图中添加边。*/
void MaximumFlow::addedge(int u, int v, int w) /*三个参数中，顶点u和v分别是两个不同的顶点，而w则表示边的容量。*/
{
    /*它创建了两条边：从 u 到 v 有容量 w，从 v 到 u 的容量为 0。这通常是网络流中的正向边和反向边。每次添加边时，tol 都会递增，用于记录下一个边的索引。
    这里使用了邻接链表的数据结构，head 数组记录了每个顶点的第一条出边，而 next 属性用于指向下一条出边。这样的结构使得可以快速地迭代顶点的所有出边。
    cap_backup 用于存储边的初始容量，这在算法中可能用于复原图的初始状态。需要注意的是，to表示的是正向边，即从源点到汇点的方向；而from则表示的是反向边，表示
    从汇点到源点的方向。*/

    /*第一条边，正向边。*/
    edge[tol].from = u;
    edge[tol].to = v;
    edge[tol].cap = w;
    cap_backup[tol] = w;
    edge[tol].next = head[u];
    head[u] = tol++;

    /*第二条边，反向边。*/
    edge[tol].from = v;
    edge[tol].to = u;
    edge[tol].cap = 0;
    cap_backup[tol] = 0;
    edge[tol].next = head[v];
    head[v] = tol++;
}

/*这段代码实现了网络流中的SAP（Shortest Augmenting Path）算法，用于计算从起点start到终点end的最大流。代码中使用了很多网络流算法的标准技术，
例如前向边和反向边、层次图（level graph）、以及GAP优化。该算法会返回一个int类型res，表示最大流的结果，也就是最小割的数值。注意了！！在求解具有连通度属性的max s-bundle问题的
时候，最重要的一个定理依据就是“最大流最小割”定理！也就是说s-bundle是想找出来最小割的大小，然而我们要是直接求解最小割的话会非常的不方便，所以说我们就求解了最大流，
因为最大流问题有非常成熟的方法来进行求解。该算法采用的是Ford-Fulkerson算法和SAP（Shortest Augmenting Path，最短增广路径）算法的结合。
关于Ford-Fulkerson算法的详细说明：第一步：构建一个残差图，根据图中边的容量来初始化所有的残差。
第二步：如果该图中存在着增广路径，则：
1）在残差图中寻找一条增广路径；
2）寻找该条增广路径上的瓶颈容量x；
3）更新剩余的残差；即（residual = residual - x);
4）增加一条反向路径，在该路径上，所有的边的权重都是x。
不过需要特别注意的一点是！！！下面的SAP算法的实现不是基于所谓的Ford-Fulkerson算法和Emonds-card算法，而是更加高效的dinic's算法，因为有层级的概念！！
所谓的Dinic's算法的具体步骤是：
1）首先构建残差图residual graph，该残差图是原图的一份拷贝。
2）不断重复下述过程：
a. 构建残差图的层次图level graph。
b. 找到该层次图上的一个阻塞流blocking flow。
c. 更新残差图（更新权重、移除饱和边、并且增加反向边。）*/

MaximumFlow mf;
int MaximumFlow::SAP(int start, int end)
{
    /* 恢复每条边的初始容量，为新的流计算做准备*/
    for (int i = 0; i < tol; ++i)
        edge[i].cap = cap_backup[i];
    int res = 0;     /*初始化结果变量res为0，这个变量用来存储最大流的流量*/
    BFS(start, end); /*使用BFS（广度优先搜索）构建一个层次图，里面包含了每个顶点到达汇点的最短路径长度和增广路径数量。*/
    /*调用了BFS函数后，我们可以得到两个非常关键的数组：（1）dep用于存储从源点到每个点的最短路径长度；（2）gap用于存储到达每个点的增广路径的数量。*/
    int top = 0;                        /*记录当前遍历到的边*/
    memcpy(cur, head, n * sizeof(int)); /*复制head数组到cur数组。*/
    int u = start;                      /*初始化u为源节点*/
    int i;
    while (dep[start] < n) /*当源节点的最短路径小于n时，继续循环*/
    {
        if (u == end) /*如果到达终点，尝试更新流*/
        {
            /*找到增广路径中容量最小的边*/
            int temp = INF + 1;            /*使用了一个辅助数组temp来记录当前最小容量*/
            int inser = 0;                 /*inser记录最小容量的边的索引。*/
            for (i = 0; i < top; ++i)      /*遍历当前增广路径上的所有边。*/
                if (temp > edge[S[i]].cap) /*如果当前边的容量小于temp*/
                {
                    temp = edge[S[i]].cap; /*更新temp和inser*/
                    inser = i;
                }
            /*更新路径上所有边的容量*/
            for (i = 0; i < top; ++i)
            {
                edge[S[i]].cap -= temp;     /*更新当前边的容量*/
                edge[S[i] ^ 1].cap += temp; /*更新反向边的容量。这里使用了异或运算符，是因为在有向图中，一个边的两个方向是相反的。*/
            }
            res += temp;           /*增加结果，即增加最大流的流量。*/
            top = inser;           /*回退到增广路径上容量最小边的起点*/
            u = edge[S[top]].from; /*更新当前节点*/
        }
        if (u != end && gap[dep[u] - 1] == 0) /*如果找不到增广路径，且GAP优化条件成立（即深度小于n的节点中已经没有可以增广的边），退出循环。*/
            break;
        for (i = cur[u]; i != -1; i = edge[i].next)                /*查找当前节点u指向的下一个合适的边*/
            if (edge[i].cap != 0 && dep[u] == dep[edge[i].to] + 1) /*如果当前边有容量并且指向的节点与当前节点在同一层，则退出循环。*/
                break;

        /*如果找到了合适的边（即前面的for循环中没有出现异常情况break掉），前进到该边的终点*/
        if (i != -1)
        {
            cur[u] = i;     /*更新当前节点记录的边*/
            S[top++] = i;   /*将当前边加入到增广路径中。*/
            u = edge[i].to; /*前进到下一个节点。*/
        }
        else /*如果当前节点没有合适的边，则退回到前一个节点，并更新层次*/
        {
            int min = n;                                 /*这个变量用于存储当前节点可以回退到的最早的层次。*/
            for (i = head[u]; i != -1; i = edge[i].next) /*遍历当前节点u的所有后继节点。*/
            {
                if (edge[i].cap == 0) /*如果当前边的容量为0，则跳过后续代码，继续遍历下一个后继节点。*/
                    continue;
                if (min > dep[edge[i].to]) /*如果当前前继节点的目标深度大于min，则更新min*/
                {
                    min = dep[edge[i].to]; /*更新min为当前前继节点的目标深度*/
                    cur[u] = i;            /*更新当前节点u记录的边为当前前继节点。*/
                }
            }
            --gap[dep[u]];    /*减少当前节点所在层次在GAP数组中的计数，因为我们在回溯。*/
            dep[u] = min + 1; /*更新当前节点的深度为min+1，这是回溯的体现。*/
            ++gap[dep[u]];    /*增加新深度在GAP数组中的计数，因为我们又前进了一步。*/
            /*如果不是起点，则回退*/
            if (u != start)              /* 如果当前节点不是源节点。*/
                u = edge[S[--top]].from; /*回退到源节点，这是通过从增广路径中移除最后一个边并回退到源节点实现的。*/
        }
    }
    return res; /*返回最大流的结果*/
}

std::vector<int> svex; /*存储最后获得的s-bundle的顶点集合*/
/*为 nV 和 oID 数组分配内存空间*/
void VertexConnectivity::reserve(int n)
{
    n += 3;
    nV = new int[n];
    oID = new int[n];
}

/*solve函数是整个求解max s-bundle中的核心函数！！！它主要用于计算顶点连通性以及图的最小割，或者说至少移除多少个顶点会使得当前的图不连通，这是非常重要的一个函数功能。*/
/*该函数有两种不同的模式，取决于参数 cals 的值。（1）如果 cals 为真，函数用于检查子图 svex 的连通性。（2）如果 cals 为假，函数用于检查整个图的全局连通性。*/
bool VertexConnectivity::solve(bool cals, int bound) /*传入的两个参数中，第一个参数是是否仅仅计算当前解集svex的连通性；第二个参数是当前的下界bound*/
{
    int nn = 0; /*定义并初始化一个局部变量 nn，用于跟踪数组 nV 和 oID 的填充情况。*/
    if (cals)   /*如果 cals 为真，则计算当前解集svex的连通性*/
    {
        for (auto u : svex)
            nV[u] = nn, oID[nn++] = u;        /*，并在 nV 中设置新的编号，同时在 oID 中保持其原始编号。这里类似于常老师写的代码。*/
        mf.init(nn + nn);                     /*初始化 mf(max flow)对象，初始化网络流的图结构，参数 nn + nn 表示在一个流网络中的双向边。*/
        for (auto u : svex)                   /*遍历 svex 中的每个顶点，并添加正向边。*/
            mf.addedge(nV[u], nV[u] + nn, 1); /*添加从顶点到其对应超级汇点的边，边的容量设置为 1。*/
        for (auto u : svex)
        {
            for (auto v : svex)
            {
                if (!adjN[u].test(v)) /*如果顶点u和顶点v不相邻，则跳过这一次循环。*/
                    continue;
                mf.addedge(nV[u] + nn, nV[v], INF); /*这里添加反向边，为所有相邻顶点对添加无限大容量的边。*/
            }
        }
        int u = nV[svex.back()];     /*选取 svex 向量中最后一个顶点的新编号 u。back方法返回一个指向std::vector中最后一个元素的迭代器*/
        for (int i = 0; i < nn; ++i) /*对于 nn 范围内的每个顶点 i，则运行 SAP 算法计算从超级源点 u + nn 到 i 的最大流。*/
        {
            if (i == u || adjN[oID[u]].test(oID[i])) /*如果该顶点是顶点 u自身，或者u与i邻接*/
                continue;                            /*则跳过这一次循环。*/
            if (mf.SAP(u + nn, i) < bound)           /*如果结果小于 bound，则返回 false，表示无法达到给定的顶点连通性。注意，这里调用了SAP算法。*/
                return false;
        }
        return true; /*如果所有测试都通过，返回 true，表示图至少具有bound的顶点连通性。*/
    }
    else /*整个 else 分支的目的是检查图的全局顶点连通性，而不是 svex 子图的连通性。 通过对所有未删除顶点进行网络流计算，它验证图是否在移除任意两个不相邻顶点后仍然连通。
         如果在任何情况下连通性小于 bound，则图的顶点连通性认为是小于 bound。*/
    {
        for (int i = 0; i < n; ++i)
            if (!del[i])                   /*如果顶点 i 没有被删除（即 del[i] 为假）*/
                nV[i] = nn, oID[nn++] = i; /*则将 i 存储在 nV 中，并且在 oID 中以新的编号 nn 存储原始编号 i，nn 递增。*/
        mf.init(nn + nn);                  /*初始化网络流对象 mf，节点数量为 nn + nn，这包括了为每个顶点添加的超级源点和超级汇点。*/
        /*注意，初始化的时候之所以变成nn+nn，是因为需要添加两个方向的流！然后下面的代码中也刚好体现了这一点！！！
        首先，在正向的流中，边的容量正常设置为1；其次，在反方向的流中，边的容量被设置成无穷大！这是为了运行最大流算法做铺垫。*/
        for (int i = 0; i < n; ++i)               /*再次遍历所有未被删除的顶点。*/
            if (!del[i])                          /*对于每个未删除的顶点，*/
                mf.addedge(nV[i], nV[i] + nn, 1); /*添加一条从顶点到其对应的超级汇点的边，边的容量为 1。*/
        for (int i = 0; i < n; ++i)               /*对于每个未删除的顶点 i，遍历其邻接表中的所有边*/
        {
            if (del[i]) /*这个条件语句检查顶点 i 是否已被删除，如果已删除，则跳过当前顶点的处理。*/
                continue;
            for (int e = head[i]; ~e; e = nxt[e]) /*遍历顶点 i 的邻接表中的所有边。*/
            {
                int v = to[e]; /*获取当前边的另一端点 v。*/
                if (del[v])    /*检查另一端点 v 是否已被删除，如果已删除，则跳过当前边的处理。*/
                    continue;
                mf.addedge(nV[i] + nn, nV[v], INF); /*如果边的另一端点 v 未删除，则在超级汇点和顶点之间添加一条容量为无限大的边。
                这表示从顶点 i 出发，可以无限大地流向顶点 v，反映了原始图中的连通性。*/
            }
        }
        /*现在已经完成了对正反两个方向的边的初始化和添加。*/
        /*接下来使用两层循环遍历所有顶点对 (i, j)*/
        for (int i = 0; i < nn; ++i)         /*外层循环，用于遍历顶点编号 i，其中 nn 是未删除顶点的数量。*/
            for (int j = i + 1; j < nn; ++j) /*内层循环，用于遍历顶点编号 j，确保 i 和 j 不是同一个顶点，避免重复检查。*/
                                             /*这个条件语句检查顶点 oID[i] 和顶点 oID[j] 是否不相邻。它通过检查邻接矩阵 adjN 中的对应位来确定两个顶点之间是否没有边连接。mf.SAP(nn + i, j)：
                                             这是调用一个网络流算法（可能是最大流算法）来计算从超级源点 nn + i 到顶点 j 的最大流。这里的目的是验证在移除顶点 oID[i] 后，是否仍然可以通过网络流
                                             从超级源点到达顶点 j。*/
                if (!adjN[oID[i]].test(oID[j]) && mf.SAP(nn + i, j) < bound)
                    /*将计算得到的最大流值与指定的 bound 进行比较，看是否小于 bound。如果小于 bound，表示从超级源点到达顶点 j 的最大流小于 bound，即移除顶点 oID[i] 后，
                    图的顶点连通性小于 bound。*/
                    return false;
        return true;
    }
}
