#ifndef _SBUNDLE_TOOL_H
#define _SBUNDLE_TOOL_H

#include <stdio.h>
#include <utility>
#include <fstream>
#include <bits/stdc++.h>

// 全局变量声明
extern int twoPow[1 << 16];
extern int msk;

// 结构声明
struct BIT_SET;

// 全局变量声明 (指针和对象)
extern int *head, *nxt, *to, etot;
extern int *degree, *que, *nV, n;
extern bool *del, *ins;
extern int *notadj;
extern int *up_bound, *ordID;
extern int *dis;
// 声明结构体 BIT_SET
struct BIT_SET /*位图运算，可以快速的判断出一个顶点的邻居和非邻居。*/
{
    int n, m;      /*n表示顶点数量，m表示边的数量。*/
    unsigned *buf; /*这个数组在该位图的init的时候被设置了。*/
    BIT_SET()      /*默认的构造函数。*/
    {
        n = m = 0;
        buf = nullptr;
    }
    ~BIT_SET()
    {
        if (buf != nullptr)
            delete[] buf;
    }
    void init(int _n) /*初始化一个位图数据结构，根据输入的位数 _n，确定位图需要多少个 unsigned 整数来存储，然后分配相应的内存空间，并将位图的数据初始化为全 0.这样子的分配效率的确高多了！*/
    {                 /*这种位图通常用于位运算和集合操作，可以高效地表示和处理大量的布尔值信息。*/
        m = _n & 31;  /*这行代码计算 _n 的低 5 位，它使用了位运算 &（按位与），31 的二进制表示为 11111，所以 & 操作可以提取 _n 的低 5 位，结果赋值给变量 m。这个操作的目的是确定每个 unsigned 整数中的位数，通常是 32 位。*/
        n = _n >> 5;  /*这行代码计算 _n 的高位，也就是除以 32 后的商，用右移运算符 >> 实现，将结果赋值给变量 n。这个操作的目的是确定需要多少个 unsigned 整数来表示 _n 位的位图。*/
        buf = new unsigned[n + 1];
        for (int i = 0; i <= n; ++i)
            buf[i] = 0; /*在循环中，将 buf 数组中的每个元素初始化为 0，以确保位图的初始状态是所有位都为 0。*/
    }
    void flip() /*对位图中的每个 unsigned 整数执行按位取反操作，*/
    {
        for (int i = 0; i < n; ++i)
            buf[i] = ~buf[i]; /*对位图中的每个 unsigned 整数执行按位取反操作，即将所有的 0 变为 1，将所有的 1 变为 0。这一步实现了位图中每个位的反转。*/
        buf[n] ^= ((unsigned)1 << m) - 1;
        /*这行代码执行了一个异或操作，用于反转最后一个 unsigned 整数中的高位。((unsigned)1 << m) - 1：这部分代码计算了一个值，它是将 1 左移 m 位，然后减去 1。
        这个操作实际上生成了一个二进制数，其中低 m 位为 1，高位为 0。例如，如果 m 等于 5，那么 (1 << m) - 1 的二进制表示为 0000011111。^：这是异或运算符. */

        /*反转最后一个 unsigned 整数中的高位是为了确保位图的长度（或位数）是精确的，这在位图操作中非常重要。位图通常用于表示一组元素的存在与否，其中每个位对应一个元素。
        在某些情况下，位图的位数可能不是 32 的整数倍，因此最后一个 unsigned 整数可能只部分用于存储位信息。这就需要考虑如何正确处理这些额外的位。*/
    }
    void set(int x) /*在特定位置设置一个位。*/
    {
        buf[x >> 5] ^= (unsigned)1 << (x & 31);
        /*x >> 5 是一个位运算操作，将整数 x 右移5位，相当于将 x 除以32，这是因为每个无符号整数通常有32位（也就是4个字节）。这一步用于定位到存储位 x 的无符号整数。
        (x & 31)：这是位与运算操作，用于获取 x 的最低5位的值，因为 31 的二进制表示是 11111，它只有最低5位是1，其余位是0。
        (unsigned)1 << (x & 31)：将整数1左移5位，得到一个只有第5位是1的无符号整数。这是一种位掩码，用于在 buf[x >> 5] 中设置指定位置的位。5=4+1，则4则是无符合整数的字节大小。
        */
    }
    bool test(int x) /*检查位集合中在位置x的位是否设置，这个程序中用来表示一个顶点是否存在于某个图的集合中。*/
    {                /*这个 test 函数通过位操作检查位集合中特定位置 x 的位是否设置。如果该位被设置（即为 1），则函数返回 true；如果未被设置（即为 0），则返回 false。*/
        return buf[x >> 5] >> (x & 31) & 1;
        /*& 1：最后，我们使用位与操作 &，将上一步的结果与 1 进行与运算。这样可以屏蔽掉除了最低位以外的其他位。如果最低位是 1，
        这个表达式的结果就是 1（即 true），如果最低位是 0，结果就是 0（即 false）。*/
    }
    int lb(unsigned x) /*根据 x 的值在 twoPow 中查找相应的结果。*/ /*lb 函数根据 x 的值在 twoPow 中查找相应的结果，并根据 x 的哪一部分（高16位或低16位）包含有效位来调整这个结果。*/
    {
        if (x & msk)                /*它检查 x 的低16位是否有任何位被设置。*/
            return twoPow[x & msk]; /*如果 x 的低16位中有设置的位，函数返回 twoPow 数组中相应索引处的值。*/
        return twoPow[x >> 16 & msk] + 16;
    }
    int lowbit() /*此函数的目的是查找并返回集合中从右往左数第一个非零位的位置。它使用了位运算来高效地完成这项任务。*/
    {
        for (int i = 0; i <= n; ++i)
        {
            unsigned x = buf[i] & ((~buf[i]) + 1); /*这行代码使用位运算找到 buf[i] 中最低位的非零位。
            ~buf[i]：首先对 buf[i] 进行按位取反操作。
            (~buf[i]) + 1：然后对结果加 1。这实际上是计算 buf[i] 的二进制补码。
            buf[i] & ((~buf[i]) + 1)：最后，原始值和它的补码进行按位与运算。这个运算的结果是保留了 buf[i] 中最右边的那个 1，而将其他所有位都置为 0*/
            if (x)                                 /*如果 x 不为零，表示在当前的 buf[i] 中找到了非零位。*/
                return lb(x) + (i << 5);
        }
        return -1;
    }
    bool empty() /*用来高效的判断一个位图集合是否为空。*/
    {
        for (int i = 0; i <= n; ++i)
            if (buf[i])
                return false;
        return true;
    }
    void operator&=(const BIT_SET &rhs) /*这是 BIT_SET 类型的按位与赋值操作符的重载。该操作符接受一个常量引用参数 rhs，表示右侧的 BIT_SET 对象。*/
    {
        for (int i = 0; i <= n; ++i)
            this->buf[i] &= rhs.buf[i];
        /*这个函数通常用于将两个位集合进行逻辑与操作，可能用在诸如集合交集之类的场合，只有当两个 BIT_SET 对象中相同位置的位都被设置时，结果 BIT_SET 对象在这个位置的位才会被设置。*/
    }
    void cp(BIT_SET &rhs) const
    { /*这个 cp 函数实际上实现了一个深拷贝操作，因为它不仅仅复制了成员变量的值，还复制了指向动态分配数组的指针所指向的数据。这是在处理涉及动态内存分配的类时常见的做法，
       以确保副本拥有自己的独立数据副本，防止潜在的悬挂指针或双重释放等问题。*/
        rhs.n = n;
        rhs.m = m;
        if (rhs.buf == NULL)
            rhs.buf = new unsigned[n + 1];
        for (int i = 0; i <= n; ++i)
            rhs.buf[i] = buf[i];
    }
}; /*adjN表示基于位图表示的邻接矩阵，invN表示非邻居矩阵，inD表示在候选集中的顶点集合，inG表示在当前图中的顶点集合。*/

// 声明外部变量 adjN, invN, inD, inG
extern BIT_SET *adjN, *invN, inD, inG;

const int INF = 0x3f3f3f3f;

class MaximumFlow
{
private:
    struct Node
    {
        int from, to, next;
        int cap;
    };
    Node *edge;
    int *cap_backup;
    int tol;
    int *head;
    int *dep;
    int *gap, *que;
    int *cur;
    int *S;
    int n;
    void BFS(int start, int end);

public:
    void reserve(int n, int m);
    void init(int _n);
    void addedge(int u, int v, int w);
    int SAP(int start, int end);
};
extern MaximumFlow mf;
extern std::vector<int> svex;

class VertexConnectivity
{
private:
    int *nV, *oID;

public:
    void reserve(int n);
    bool solve(bool cals, int bound);
};

#endif // _SBUNDLE_TOOL_H