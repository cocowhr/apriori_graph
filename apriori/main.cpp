#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <map>
#include <list>
using namespace std;
#define ITEMNUM 10
#define ITEMLEN 3
class Data
{
	friend ostream &operator<<(ostream &output, const Data* data);
public:
	Data() {}
	~Data() {}
	int bv;
	bitset<ITEMNUM> tidlist;
	double sup;
};
ostream& operator<<( ostream& os,const Data* data)
{
	return os << data->bv << ":" << data->tidlist<< ":" <<data->sup;
}
typedef struct
{
	vector<string> p;     //��¼i����������ݣ�����1������ݣ�2������ݣ�����3������ݵȵȣ�
	vector<double> sup;    //�������֧�ּ���������1��е�ÿһ����¼��֧�ֶȼ���Ϊ���٣�����2��е�ÿһ����¼��֧�ֶȼ���Ϊ���ٵȵȣ�
	int n;        //�����Ŀ������õ���1����ж�������¼������Ϊ2����ж�������¼������3����ж�������¼��
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
string int2str(int n) {

	char t[24];
	int i = 0;

	while (n) {
		t[i++] = (n % 10) + '0';
		n /= 10;
	}
	t[i] = 0;
	string ret=(string)_strrev(t);
	if (ret.length()<1)
	{
		ret="0";
	}
	return ret;
}
void initData(Data *d,int items[ITEMNUM][ITEMLEN])
{
	double count=0;
	for(int j=1; j<=ITEMNUM; j++)
	{
		for(int k=1;k<=ITEMLEN;k++)
		{
			if(items[j-1][k-1]==d->bv)
			{
				count++;
				d->tidlist[ITEMNUM-j]=1;//���λΪ��һ��items
			}
		}
	}
	d->sup=count/ITEMNUM;
}
bool Comp(Data* first,Data* second)
{
	return first->sup > second->sup;
}
/*ջ*/
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

/*ͼ*/

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

	//��ӡ�ڽ�����
	void displayGraph()
	{
		for (int i = 0; i < vexnum; i++)
		{
			cout << "��" << i+1 << "�������ǣ�" << vertices[i].name<<"֧�ֶ���"<<vertices[i].data
				<< " �ڽӱ�Ϊ: ";
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
	//����һ������ͼ
	void CreateDG( vector<Data*> &dataarray, map<int,double>&minsupmap,Large& L2)
	{
		InitVertics(dataarray);
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
						L2.p.push_back(int2str(vi->bv)+int2str(vj->bv));
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
	void DFSsearch(vector<Data*> &dataarray, map<int,double>&minsupmap,vector<Large> &Lar)
	{
		//������������ǵݹ��㷨
		Stack <int>s;
		ArcNode* p;
		int* visited=new int [vexnum+1]();
		visited[0] = 1;
		s.push(0);
		while (!s.empty())
		{
			p=vertices[s.top()].firstarc;
			while(p)
			{
				if(!visited[p->adjvex])
				{
					visited[p->adjvex]=1;
					if(s.size()>=2)
					{
						Stack<int>::iterator ii= s.begin();						
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
						if(allbeside)//Vk(p->adjvex)�Ƿ��������е����Ƚ����ڽӵ�
						{
							int i=1;//Ƶ��������
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
									string point=(int2str(dataarray[p->adjvex]->bv));
									Stack<int>::iterator itor= s.begin();
									while (itor != s.end())
									{
										point+=int2str(vertices[*itor].name);
										itor++;
									}
									Lar[i-1].n++;
									Lar[i-1].p.push_back(point);
									Lar[i-1].sup.push_back(sup3);
								}
							}
							s.push(p->adjvex);
						}
					}
					else
					{
						s.push(p->adjvex);
					}
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
		//��ʼ������ͼG���ڣ�v��G��ĳ�����㡣���������ɾ��G�ж���v������ػ�
		int i,j=0;
		ArcNode *p,*q;
		p=vertices[j].firstarc;//ɾ����vΪ�����Ļ����
		while(p)
		{
			q=p;
			p=p->nextarc;
			free (q);
			arcnum--;
		}
		vexnum--;
		for(i=j; i<vexnum; i++) //����v����Ķ���ǰ��
			vertices[i]=vertices[i+1];
		for(i=0; i<vexnum; i++) //ɾ����vΪ��ȵĻ�����ұ�Ҫʱ�޸ı���Ķ���λ��ֵ
		{
			p=vertices[i].firstarc;
			while(p)
			{
				if(p->adjvex==j)
				{
					if(p==vertices[i].firstarc)//��ɾ���Ľ���ǵ�һ�����
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
						p->adjvex--;//�޸ı���Ķ���λ��ֵ(���)
					q=p;
					p=p->nextarc;
				}

			}
		}
	}
	private:
	//��ʼ���ڽ�����ı�ͷ����
	void InitVertics( vector<Data*> &dataarray)
	{
		for (int i = 0; i < vexnum; i++)
		{
			vertices[i].name=dataarray[i]->bv;
			vertices[i].data =dataarray[i]->sup;
			vertices[i].tidlist =dataarray[i]->tidlist;
		}
	}

	//����һ������
	void insertArc(int vHead, int vTail, InfoType w)
	{
		//����һ������
		ArcNode *newArcNode = new ArcNode;
		newArcNode->adjvex = vTail;
		newArcNode->nextarc = NULL;
		newArcNode->info = w;

		//arcNode ��vertics[vHead]���ڽӱ�
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

	//const���ݳ�Ա�����ڹ��캯�����ʼ��
	static const int MAX_VERTEX_NUM = 20;  //��󶥵����


	struct ArcNode          //����
	{
		int adjvex;        //�û���ָ��Ķ����λ��
		ArcNode *nextarc;  //ָ����һ������ָ��
		InfoType info;     //�û������Ϣ��ָ��
	};
	struct VNode           //ͷ���
	{
		VertexType name;    //����Ӣ����ĸ
		VertexType2 data;    //����֧�ֶ�
		VertexType3 tidlist; //tidlist
		ArcNode *firstarc;  //ָ���һ�������ڸö���Ļ���ָ��
	};

	/*VNode AdjList[MAX_VERTEX_NUM];*/
	/* AdjList[MAX_VERTEX_NUM] vertices;*/
	VNode vertices[MAX_VERTEX_NUM];
	int vexnum;             //ͼ�ĵ�ǰ������
	int arcnum;             //ͼ�Ļ���

};
/*���Ŷ�*/

void Rules(vector<Large> &L,int l[ITEMNUM][ITEMLEN] )
{
	int lk=L.size();//lkΪƵ����Ŀ������
	int tp1,tp2;

	for(int i=2; i<=lk; i++)              //���ζ�ÿ��Ƶ����Ŀ���Ƶ���������
	{

		cout<<"L"<<i<<':'<<endl;
		for(int j=0; j<L[i-1].n; j++)   //�Ե�ǰƵ����Ŀ����ÿһ����¼�����Ƶ�
		{
			int n=1;
			for(int h=0; h<i; h++)
			{
				n*=2;                 //Ƶ����Ӽ��ܸ�����ʵ���ܹ���(2^i)-2���Ӽ���
			}
			for(int h=1; h<n-1; h++)      //������Ϊ(2^i)-2���Ӽ����в���
			{
				string temp1;//����һά��������temp1����Ź�������ǰ�������α���tp1��¼��������ǰ������������
				string temp2;//����һά��������temp2����Ź��������������α���tp2��¼��������������������
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
				vector<string>::iterator itit1=find(L[len-1].p.begin(),L[len-1].p.end(),temp1);               //ͳ�ƹ�������ǰ�������ݿ������г��ֵĴ���
				//                cout<<itit1-L[len-1].p.begin()<<"�±�"<<endl;
				double counts=L[len-1].sup[itit1-L[len-1].p.begin()]*ITEMNUM;
				//                cout<<counts<<"   count"<<endl;
				if((double)(L[i-1].sup[j]*ITEMNUM/counts>=0.01))//����Ƿ���ڵ�����С���Ŷ���ֵ 0.2=min_conf
				{
					//if(temp2=="5"||temp2=="6"||temp2=="7")
					//{
					cout<<temp1<<' ';
					cout<<"==>";
					cout<<temp2<<' ';
					cout<<"\t"<<"���Ŷ�="<<(double)(L[i-1].sup[j]*ITEMNUM/counts) *100<<"%"<<"\t"<<"\t"<<"֧�ֶ�="<<(double)(L[i-1].sup[j]) *100<<"%"<<endl;
					//}
				}
			}
		}
	}
}

int main()
{
	int items[ITEMNUM][ITEMLEN];
	//1 8-18 2 18-8 3 guohy 4lvtx 5xz 6outlook 7 liulan
	items[0][0]=1;
	items[0][1]=3;
	items[0][2]=5;
	items[1][0]=2;
	items[1][1]=4;
	items[1][2]=5;
	items[2][0]=1;
	items[2][1]=4;
	items[2][2]=6;
	items[3][0]=2;
	items[3][1]=3;
	items[3][2]=5;
	items[4][0]=1;
	items[4][1]=3;
	items[4][2]=7;
	items[5][0]=1;
	items[5][1]=4;
	items[5][2]=7;
	items[6][0]=2;
	items[6][1]=3;
	items[6][2]=7;
	items[7][0]=1;
	items[7][1]=4;
	items[7][2]=5;
	items[8][0]=2;
	items[8][1]=4;
	items[8][2]=5;
	items[9][0]=1;
	items[9][1]=4;
	items[9][2]=7;
	map<int,int>itemmap;
	int id=1;
	for(int i=1; i<=ITEMNUM; i++)
	{
		for(int j=1; j<=ITEMLEN; j++)
		{
			int word=items[i-1][j-1];
			map<int,int>::iterator it;
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
	map<int,int>::iterator iter;
	for( iter=itemmap.begin(); iter!=itemmap.end(); iter++)
	{
		int a= iter->first;
		int p = iter->second;
		d[p-1]->bv=a;
	}
	for(int i=1; i<=num; i++)
	{
		initData(d[i-1],items);
		dataarray.push_back(d[i-1]);
	}
	sort(dataarray.begin(),dataarray.end(),Comp);
	map<int,double>minsupmap;
	minsupmap[1]=0.2;
	minsupmap[2]=0.1;
	minsupmap[3]=0.09;
	minsupmap[4]=0.2;
	minsupmap[5]=0.1;
	minsupmap[6]=0.3;
	minsupmap[7]=0.3;
	vector<Data*>::iterator dataiter;
	for(dataiter=dataarray.begin(); dataiter!=dataarray.end();)
	{

		if((*dataiter)->sup<minsupmap[(*dataiter)->bv])
		{
			dataiter=dataarray.erase(dataiter);    //ɾ��Ԫ�أ�����ֵָ����ɾ��Ԫ�ص���һ��λ��
		}
		else
		{
			++dataiter;    //ָ����һ��λ��
		}
	}
	vector<Large>Lar;
	Large L1;
	L1.n=dataarray.size();
	int i=1;
	for(dataiter=dataarray.begin(); dataiter!=dataarray.end(); dataiter++)
	{
		L1.p.push_back(int2str((*dataiter)->bv));
		L1.sup.push_back((*dataiter)->sup);
		i++;
	}
	Lar.push_back(L1);
	Large L2;
	L2.n=0;
	ALGraph<int,double, bitset<ITEMNUM>,int> dgGraph(L1.n);
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


