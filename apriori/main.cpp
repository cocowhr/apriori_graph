#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <map>
#include <list>
using namespace std;
#define ITEMNUM 10
class Data
{
    friend ostream &operator<<(ostream &output, const Data* data);
public:
    Data() {}
    ~Data() {}
    string bv;
    bitset<ITEMNUM> tidlist;
    double sup;
};
ostream& operator<<( ostream& os,const Data* data)
{
    return os << data->bv << ":" << data->tidlist<< ":" <<data->sup;
}
typedef struct
{
    vector<string> p;     //记录i项集的所有内容（比如1项集的内容，2项集的内容，或者3项集的内容等等）
    vector<double> sup;    //各个项集的支持计数（比如1项集中的每一条记录的支持度计数为多少，或者2项集中的每一条记录的支持度计数为多少等等）
    int n;        //项集的数目（比如得到的1项集中有多少条记录，或者为2项集中有多少条记录，或者3项集中有多少条记录）
} Large;
void printLar(vector<Large> &Lar)
{
    for(int i=1; i<=Lar.size(); i++)
    {
        for(int j=1; j<=Lar[i-1].n; j++)
        {
            cout<<Lar[i-1].p[j-1]<<":"<<Lar[i-1].sup[j-1]<<endl;
        }
    }
}
void initData(Data *d,string* items)
{
    double count=0;
    for(int j=1; j<=ITEMNUM; j++)
    {
        string::size_type idx = items[j-1].find(d->bv);
        if ( idx != string::npos )
        {
            count++;
            d->tidlist[ITEMNUM-j]=1;//最高位为第一个items
        }
    }
    d->sup=count/ITEMNUM;
}
bool Comp(Data* first,Data* second)
{
    return first->sup > second->sup;
}
/*栈*/
template<typename T, class C = list<T> >
class Stack
{
public:
    typedef typename C::iterator iterator;
	Stack()
	{
		stacksize=0;
	}
    iterator begin()
    {
        return cc.begin();
    }
    iterator end()
    {
        return cc.end();
    }
    void push(const T& vaule)
    {
        stacksize++;
        cc.push_front(vaule);
    }
    void pop()
    {
        stacksize--;
        cc.pop_front();
    }
    bool empty()
    {
        return stacksize==0;
    }
    T top()
    {
        return cc.front();
    }
    int size()
    {
        return stacksize;
    }
private:
    int stacksize;
    C cc;
};

/*图*/

template<typename VertexType,typename VertexType2,typename VertexType3, typename InfoType>
class ALGraph
{
public:
    ALGraph(int verNum)
        : vexnum(verNum), arcnum(0)
    {
        for (int i = 0; i < MAX_VERTEX_NUM; i++)
            vertices[i].firstarc = NULL;
    }

    //打印邻接链表
    void displayGraph()
    {
        for (int i = 0; i < vexnum; i++)
        {
            cout << "第" << i+1 << "个顶点是：" << vertices[i].name<<"支持度是"<<vertices[i].data
                 << " 邻接表为: ";
            ArcNode *arcNode = vertices[i].firstarc;
            while (arcNode != NULL)
            {
                cout << " -> " << vertices[arcNode->adjvex].name
                     << "(" << arcNode->info << ")";
                arcNode = arcNode->nextarc;
            }
            cout << endl;
        }
    }
    //构造一个有向图
    void CreateDG( vector<Data*> &dataarray, map<string,double>&minsupmap,Large& L2)
    {
        InitVertics(dataarray);
        int vhead, vtail;
        int n=dataarray.size();
        for(int i=1; i<=n; i++)
        {
            Data* vi=dataarray[i-1];
            for(int j=i+1; j<=n; j++)
            {
                Data* vj=dataarray[j-1];
                double sup1=min(vi->sup,vj->sup);
                double sup2=max(minsupmap[vi->bv],minsupmap[vj->bv]);
                if (sup1>=sup2)
                {
                    double sup3=(double)(vi->tidlist&vj->tidlist).count()/ITEMNUM;
                    if(sup3>=sup2)
                    {
                        L2.p.push_back(vi->bv+vj->bv);
                        L2.sup.push_back(sup3);
                        L2.n++;
                        insertArc(i-1, j-1, 0);
                    }
                }
//
            }
        }
    }
    bool isempty()
    {
        return 0==vexnum;
    }
    void DFSsearch(vector<Data*> &dataarray, map<string,double>&minsupmap,vector<Large> &Lar)
    {
        //深度优先搜索非递归算法
        Stack <int>s;
        ArcNode* p;
        int* visited=new int [vexnum+1]();
        visited[0] = 1;
//        cout<<"节点："<<vertices[0].data<<endl;
        s.push(0);
        while (!s.empty())
        {
            p=vertices[s.top()].firstarc;
            while(p)
            {
                if(!visited[p->adjvex])
                {
//                    cout<<vertices[s.top()].name<<endl;
                    visited[p->adjvex]=1;
                    if(s.size()>=2)
                    {
                        bool allbeside=true;
                        Stack<int>::iterator itor= s.begin();
                        while (itor != s.end())
                        {
                            int adj=p->adjvex;
                            bool beside=false;
                            ArcNode* pf=vertices[*itor++].firstarc;
                            while(pf)
                            {
                                if(pf->adjvex==adj)
                                {
                                    beside=true;
                                    break;
                                }
                                else
                                {
                                    pf=pf->nextarc;
                                }
                            }
                            if(!beside)
                            {
                                allbeside=false;
                                break;
                            }

                        }
                        if(allbeside)//Vk(p->adjvex)是否是其所有的祖先结点的邻接点
                        {
                            int i=1;//频繁集层数
                            double sup1=dataarray[p->adjvex]->sup;
                            double sup2=minsupmap[dataarray[p->adjvex]->bv];
                            Stack<int>::iterator itor= s.begin();
                            while (itor != s.end())
                            {
                                i++;
                                sup1=min(sup1,vertices[*itor].data);
                                sup2=max(sup2,minsupmap[vertices[*itor++].name]);
                            }
                            if(sup1>=sup2)
                            {
                                bitset<ITEMNUM> bit=dataarray[p->adjvex]->tidlist;
                                itor= s.begin();
                                while (itor != s.end())
                                {
                                    bit=bit&vertices[*itor++].tidlist;
                                }
                                double sup3=(double)bit.count()/ITEMNUM;
                                if(sup3>=sup2)
                                {
                                    if(Lar.size()==i-1)
                                    {
                                        Large Li;
                                        Li.n=0;
                                        Lar.push_back(Li);
                                    }
                                    else if(Lar.size()<i-1)
                                    {
                                        cout<<"so wrong!!!"<<endl;
                                    }
                                    string point=(dataarray[p->adjvex]->bv);
                                    Stack<int>::iterator itor= s.begin();
                                    while (itor != s.end())
                                    {
                                        point+=vertices[*itor].name;
                                        itor++;
                                    }
                                    Lar[i-1].n++;
                                    Lar[i-1].p.push_back(point);
                                    Lar[i-1].sup.push_back(sup3);
                                }
                            }
                        }
                    }
                    s.push(p->adjvex);
                    break;
                }
                else
                    p=p->nextarc;
            }
            if (p == NULL)
            {
                s.pop();
            }
        }

    }
    void deleteVex()
    {
        //初始条件：图G存在，v是G中某个顶点。操作结果：删除G中顶点v及其相关弧
        int i,j=0;
        ArcNode *p,*q;
        p=vertices[j].firstarc;//删除以v为初读的弧或边
        while(p)
        {
            q=p;
            p=p->nextarc;
            free (q);
            arcnum--;
        }
        vexnum--;
        for(i=j; i<vexnum; i++) //顶点v后面的顶点前移
            vertices[i]=vertices[i+1];
        for(i=0; i<vexnum; i++) //删除以v为入度的弧或边且必要时修改表结点的顶点位置值
        {
            p=vertices[i].firstarc;
            while(p)
            {
                if(p->adjvex==j)
                {
                    if(p==vertices[i].firstarc)//待删除的结点是第一个结点
                    {
                        vertices[i].firstarc=p->nextarc;
                        free(p);
                        p=vertices[i].firstarc;
                        arcnum--;
                    }
                    else
                    {
                        q->nextarc=p->nextarc;
                        free(p);
                        p=q->nextarc;
                        arcnum--;
                    }
                }
                else
                {
                    if(p->adjvex>j)
                        p->adjvex--;//修改表结点的顶点位置值(序号)
                    q=p;
                    p=p->nextarc;
                }

            }
        }
    }
//private:
    //初始化邻接链表的表头数组
    void InitVertics( vector<Data*> &dataarray)
    {
//        cout << "请输入每个顶点的关键字：" << endl;
        VertexType val;
        for (int i = 0; i < vexnum; i++)
        {
            vertices[i].name=dataarray[i]->bv;
            vertices[i].data =dataarray[i]->sup;
            vertices[i].tidlist =dataarray[i]->tidlist;
        }
    }

    //插入一个表结点
    void insertArc(int vHead, int vTail, InfoType w)
    {
        //构造一个表结点
        ArcNode *newArcNode = new ArcNode;
        newArcNode->adjvex = vTail;
        newArcNode->nextarc = NULL;
        newArcNode->info = w;

        //arcNode 是vertics[vHead]的邻接表
        ArcNode *arcNode = vertices[vHead].firstarc;
        if (arcNode == NULL)
            vertices[vHead].firstarc = newArcNode;
        else
        {
            while (arcNode->nextarc != NULL)
            {
                arcNode = arcNode->nextarc;
            }
            arcNode->nextarc = newArcNode;
        }
        arcnum++;
    }

    //const数据成员必须在构造函数里初始化
    static const int MAX_VERTEX_NUM = 20;  //最大顶点个数


    struct ArcNode          //表结点
    {
        int adjvex;        //该弧所指向的顶点的位置
        ArcNode *nextarc;  //指向下一条弧的指针
        InfoType info;     //该弧相关信息的指针
    };
    struct VNode           //头结点
    {
        VertexType name;    //顶点英文字母
        VertexType2 data;    //顶点支持度
        VertexType3 tidlist; //tidlist
        ArcNode *firstarc;  //指向第一条依附于该顶点的弧的指针
    };

    /*VNode AdjList[MAX_VERTEX_NUM];*/
    /* AdjList[MAX_VERTEX_NUM] vertices;*/
    VNode vertices[MAX_VERTEX_NUM];
    int vexnum;             //图的当前顶点数
    int arcnum;             //图的弧数

};
/*置信度*/

void Rules(vector<Large> &L,string* l)
{
    int lk=L.size();//lk为频繁项目集层数
    int tp1,tp2;

    for(int i=2; i<=lk; i++)              //依次对每个频繁项目集推导关联规则
    {

        cout<<"L"<<i<<':'<<endl;
        for(int j=0; j<L[i-1].n; j++)   //对当前频繁项目集的每一条记录进行推导
        {
            int n=1;
            for(int h=0; h<i; h++)
            {
                n*=2;                 //频繁项集子集总个数（实际总共有(2^i)-2个子集）
            }
            for(int h=1; h<n-1; h++)      //对总数为(2^i)-2个子集进行操作
            {
                string temp1;//申请一维整形数组temp1，存放关联规则前件；整形变量tp1记录关联规则前件包含的项数
                string temp2;//申请一维整形数组temp2，存放关联规则后件；整形变量tp2记录关联规则后件包含的项数
                tp1=0;
                tp2=0;
                int t=1;
                for(int m=0; m<i; m++)
                {
                    string tt;
                    if((h/t)%2==1)
                    {
//						temp1[tp1++]=*(*((*(L+i)).p+j)+m);
                        tt=temp1;
                        temp1=((L[i-1].p)[j])[m]+tt;
                    }
                    else
                    {
//						temp2[tp2++]=*(*((*(L+i)).p+j)+m);
                        tt=temp2;
                        temp2=((L[i-1].p)[j])[m]+tt;
                    }
                    t*=2;
                }
//                cout<<temp1<<endl;
                int len=temp1.length();
                vector<string>::iterator itit1=find(L[len-1].p.begin(),L[len-1].p.end(),temp1);               //统计关联规则前件在数据库事务中出现的次数
//                cout<<itit1-L[len-1].p.begin()<<"下标"<<endl;
                double counts=L[len-1].sup[itit1-L[len-1].p.begin()]*ITEMNUM;
//                cout<<counts<<"   count"<<endl;
                if((double)(L[i-1].sup[j]*ITEMNUM/counts>=0.01))//检查是否大于等于最小置信度阈值 0.2=min_conf
                {
                    cout<<temp1<<' ';
                    cout<<"==>";
                    cout<<temp2<<' ';
                    cout<<"\t"<<"置信度="<<(double)(L[i-1].sup[j]*ITEMNUM/counts) *100<<"%"<<"\t"<<"\t"<<"支持度="<<(double)(L[i-1].sup[j]) *100<<"%"<<endl;
                }
            }
        }
    }
}

int main()
{
    string items[ITEMNUM];
    items[0]="ABDG";
    items[1]="BDE";
    items[2]="ABCEF";
    items[3]="BDEG";
    items[4]="ABCEF";
    items[5]="BEG";
    items[6]="ACDE";
    items[7]="BE";
    items[8]="ABEF";
    items[9]="ACDE";
    map<char,int>itemmap;
    int id=1;
    for(int i=1; i<=ITEMNUM; i++)
    {
        int len=items[i-1].length();
        for(int j=1; j<=len; j++)
        {
            char word=items[i-1][j-1];
            map<char,int>::iterator it;
            it=itemmap.find(word);
            if(it==itemmap.end())
            {
                itemmap[word]=id;
                id++;
            }
        }
    }
    int num=itemmap.size();
    vector<Data*> dataarray;
    Data **d=new Data*[num];
    for(int i=1; i<=num; i++)
    {
        d[i-1]=new Data();
    }
    map<char,int>::iterator iter;
    for( iter=itemmap.begin(); iter!=itemmap.end(); iter++)
    {
        char a= iter->first;
        int p = iter->second;
        d[p-1]->bv=a;
    }
    for(int i=1; i<=num; i++)
    {
        initData(d[i-1],items);
        dataarray.push_back(d[i-1]);
    }
    sort(dataarray.begin(),dataarray.end(),Comp);
    map<string,double>minsupmap;
    minsupmap["A"]=0.3;
    minsupmap["B"]=0.5;
    minsupmap["C"]=0.2;
    minsupmap["D"]=0.3;
    minsupmap["E"]=0.4;
    minsupmap["F"]=0.2;
    minsupmap["G"]=0.4;
    vector<Data*>::iterator dataiter;
    for(dataiter=dataarray.begin(); dataiter!=dataarray.end();)
    {

        if((*dataiter)->sup<minsupmap[(*dataiter)->bv])
        {
            dataiter=dataarray.erase(dataiter);    //删除元素，返回值指向已删除元素的下一个位置
        }
        else
        {
            ++dataiter;    //指向下一个位置
        }
    }
    vector<Large>Lar;
    Large L1;
    L1.n=dataarray.size();
    int i=1;
    for(dataiter=dataarray.begin(); dataiter!=dataarray.end(); dataiter++)
    {
        L1.p.push_back((*dataiter)->bv);
        L1.sup.push_back((*dataiter)->sup);
        i++;
    }
    Lar.push_back(L1);
    Large L2;
    L2.n=0;
    ALGraph<string,double, bitset<ITEMNUM>,int> dgGraph(L1.n);
    dgGraph.CreateDG(dataarray,minsupmap,L2);
    Lar.push_back(L2);
    while(!dgGraph.isempty())
    {
        dgGraph.DFSsearch(dataarray,minsupmap,Lar);
        dgGraph.deleteVex();
        dataarray.erase(dataarray.begin());
    }
    Rules(Lar,items);
}


