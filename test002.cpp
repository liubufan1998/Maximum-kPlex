#include <iostream>
#include "sbundle_tool.h"
using namespace std;

int main(){
    int n; /*表示顶点的数量。*/
    int m; /*表示边的数量。*/
    VertexConnectivity *vc = new VertexConnectivity();
    // MaximumFlow *mf = new MaximumFlow();
    std::ifstream infile("edges.txt");  
    if (!infile.is_open()) {  
        std::cout << "Failed to open file edges.txt" << std::endl;  
        return 1;  
    }
    infile >> n >> m;  
    std::pair<int, int> *E = new std::pair<int, int>[m]; /*边的集合*/
    for (int i = 0; i < m; i++) {  
        infile >> E[i].first >> E[i].second;  
    } 
    mf.reserve(n,m); /*初始化最大流里面的变量，便于后面调用。*/
    vc->reserve(n); /*初始化我们的点连通类。*/
    /*创建两个新的位集合数组adjN和invN，分别用于存储邻接关系和逆邻接关系。*/
    
    adjN = new BIT_SET[n]; /*adjN数组表示顶点之间的邻接关系矩阵*/
    invN = new BIT_SET[n]; /*invN表示顶点之间的逆邻接关系矩阵*/
    // std::pair<int, int> *E = new std::pair<int, int>[m];; /*表示边的集合*/

    // /*手动为边集合进行赋值。*/
    // printf("Now please input the individual edges. \n");
    // for(int i = 0; i < m; i++){
    //     int u, v; /*一条边的两个顶点，左边的顶点编号小于等于右边。*/
    //     scanf("%d %d",&u, &v);  
    //     E[i] = std::make_pair(u,v);      
    // }
    /*初始化adjN数组，为每个顶点准备邻接位集*/
    for (int i = 0; i < n; i++)
        adjN[i].init(n);
    for (int i = 0; i < m; i++) /*更新每个顶点的邻接位集。*/
    {
        int u = E[i].first, v = E[i].second; /*从边数组E中获取当前边的两个顶点u和v。*/
        printf("Two vertices of the edge are %d and %d\n", E[i].first, E[i].second);
        /*使用位集合数组adjN记录顶点之间的邻接关系。这两行代码将位集合中的相应位设置为1，表示u和v之间有一条边*/
        adjN[u].set(v);
        adjN[v].set(u);
    }

    for(int i = 0; i < n; i++){ /*接下来设置解集S中的元素。*/
        svex.push_back(i);
    }
    /*接下来是最重要的测试，即判断一个图是不是s-bundle。*/
    printf("Now please enter the value of s-bundle: \n");
    int s_value;
    scanf("%d",&s_value);
    if(vc->solve(true, n-s_value)){/*因为solve函数中的bound就是|V|-s，也是要用当前的顶点总数减去s*/
        printf("Congratulations! This graph is a qualified s-bundle!!! \n");
    }else{
        printf("Sorry, this graph is not a s-bundle!!\n");
    }

    delete[] adjN;
    delete[] invN;
    
    return 0;
}