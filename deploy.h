#ifndef __route_h__
#define __route_h__

#include "lib_io.h"
/* #include "ns.h" */
#include <vector>
#include <iostream>					// for cout etc.
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <queue>
#include <stack>
/* #include <mcheck.h> */
#include "ns.h"

using namespace std;

# define MAX 1000
# define INF 2047483600

/* #define GA_ELITRATE		0.10f		// elitism rate */
/* #define GA_MUTATIONRATE	0.25f		// mutation rate */
/* #define GA_MUTATIONRATE	0.8f		// mutation rate */
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE
/* #define GA_POPSIZE		50		// ga population size */
#define GA_MAXITER		5		// maximum iterations





extern float GA_ELITRATE;				// elitism rate
extern float GA_MUTATIONRATE;
extern int GA_POPSIZE;
extern float HanMingDis;
extern int PopInitRate;

extern int TIME_LIMIT;


struct ga_struct;
typedef vector<ga_struct> ga_vector;// for brevity

typedef struct edge Edge;
typedef struct mcmf MCMF;


struct inspire_interface;
struct graph_init;


/* **********************************    MCMF Fun    **************************************** */
struct edge{
    int from,to,cap,flow,cost;
};

struct mcmf{
    int n,m,s,t; //n:v_num   m:e_num
    int server_cost;
    vector<Edge> edges;
    vector<int> G[MAX];
    int inq[MAX];
    int d[MAX];
    int p[MAX];
    int a[MAX];
    int cus[MAX];

    void init(int n){
        this->n = n;
        for(int i = 0;i < n;i++) G[i].clear();
        edges.clear();
        memset(this->inq,0,sizeof(this->inq));
        memset(this->d,0,sizeof(this->d));
        memset(this->p,0,sizeof(this->p));
        memset(this->a,0,sizeof(this->a));
        memset(this->cus,0,sizeof(this->cus));
    }

    void clearup(){
        vector<Edge>().swap(this->edges);
        for(int i = 0;i<MAX;i++)
            vector<int>().swap(this->G[i]);

    }

    void AddEdge(int from,int to,int cap,int cost){
        edges.push_back((Edge){from,to,cap,0,cost});
        edges.push_back((Edge){to,from,0,0,-cost});
        m = edges.size();
        G[from].push_back(m-2);
        G[to].push_back(m-1);
    }

    void AddSuperS(int to){
        edges.push_back((Edge){this->s,to,INF,0,0});
        edges.push_back((Edge){to,this->s,-INF,0,0});
        m = edges.size();
        G[this->s].push_back(m-2);
        G[to].push_back(m-1);
    }

    void AddSuperT(int from,int cap){
        edges.push_back((Edge){from,this->t,cap,0,0});
        edges.push_back((Edge){this->t,from,-INF,0,0});
        m = edges.size();
        G[from].push_back(m-2);
        G[this->t].push_back(m-1);
    }

    void SaveCustomer(int customer,int netnode){
        /* save the num. of the customer */
        this->cus[netnode] = customer;
    }

    bool FlowCheck(int flow){
        vector<int> ::iterator it;
        int max_flow = 0;
        for(it = G[this->t].begin();it != G[this->t].end();it ++){
           max_flow += edges[(*it)^1].cap;
        }
        return (flow == max_flow);
    }

    vector<int> ServerCheck(){
        vector<int> server;
        server.clear();
        vector<int> ::iterator it;
        for(it = G[this->s].begin();it != G[this->s].end();it ++){
            if(edges[(*it)].flow > 0){
                server.push_back(edges[(*it)].to);
            }

        }
        return server;
    }

    void edge_check(Edge * e){
        int cost = abs(e->cost);
        if(e->flow > 0)
            if(e->flow < e->cap)
                e->cost = cost;
            else
                e->cost = INF;
        else if(e->flow < 0)
            if(abs(e->flow) < e->cap)
                e->cost = cost;
            else
                e->cost = INF;
        else
            if(e->from == this->t || e->to == this->s)
                e->cost = INF;
            else
                e->cost = cost;
    }

    void test(){
        vector<Edge> ::iterator it;

        for(it = this->edges.begin();it != edges.end();it ++){
            cout<<"M  tail="<<(*it).to<<" head="<<(*it).from<<" flow="<<(*it).cap<<" cost="<<(*it).cost<<endl;
            cout<<endl;
        }
    }
    //other functions
    //...
};


/* **********************************    The end     **************************************** */

/* **********************************    SPFA QUEUE     **************************************** */
typedef struct QElemType
{
    int NetId;
    int dist;
}QElemType;

typedef struct QNode
{
    QElemType data;
    struct QNode *next;
}QNode, *QueuePtr;

// for LLL SLF
typedef struct LinkQueue
{
    QueuePtr front;
    QueuePtr rear;
    long long sum;
    int item_num;
    void init_Queue()
    {
        this->front = this->rear = (QNode *)malloc(sizeof(QNode));
        this->front->next = NULL;
        this->rear->next = NULL;
        this->sum = 0;
        this->item_num = 0;
    }

    // (from rear)
    void en_Queue_Rear(QElemType e)
    {
        QueuePtr temp = (QueuePtr)malloc(sizeof(QNode));
        if(temp)
        {
            temp->data = e;
            temp->next = NULL;
            this->rear->next = temp;
            this->rear = temp;
            this->rear->next = NULL;
            this->sum += e.dist;
            this->item_num ++;
        }
    }

    //  (from top)
    void en_Queue_Top(QElemType e)
    {
        QueuePtr temp = (QueuePtr)malloc(sizeof(QNode));
        if(temp)
        {
            temp->data = e;
            if(this->front == this->rear) {
                this->front->next = temp;
                this->rear = temp;
            }
            else
                temp->next = this->front->next;
            this->front->next = temp;
            this->sum += e.dist;
            this->item_num ++;
        }
    }

    //  (have Top Node)
    void de_Queue()
    {
        if(this->front == this->rear)
            return;
        QueuePtr temp = this->front->next;
        if(this->front->next == this->rear)
            this->rear = this->front;
        this->front->next = temp->next;
        this->sum -= temp->data.dist;
        this->item_num --;
        free(temp);
    }

    void de_Queue_where(int aver,QElemType &out)
    {
        QueuePtr tos;
        QueuePtr pre;
        tos = this->front->next;
        pre = this->front;
        while(pre != this->rear){
            if(tos->data.dist <= aver){
                break;
            }
            pre = tos;
            tos = tos->next;
        }
        if(tos == NULL){
            return;
        }
        else{
            out.NetId= tos->data.NetId;
            out.dist= tos->data.dist;
            pre->next = tos->next;
            this->item_num --;
            this->sum -= out.dist;
            if(tos == this->rear){
                this->rear = pre;
            }

            tos->next = NULL;
            pre = NULL;
            free(tos);
        }
    }

    int is_Empty()
    {
        if(this->rear== this->front)
            return 1;
        return 0;
    }

    int getlength_Queue()
    {
        QueuePtr temp = this->front;
        int i = 0;
        while(temp != this->rear)
        {
            ++i;
            temp = temp->next;
        }
        return i;
    }

    void clear()
    {
        QueuePtr temp = this->front->next;
        while(temp)
        {
            QueuePtr tp = temp;
            temp = temp->next;
            free(tp);
        }
        temp = this->front;
        this->front = this->rear = NULL;
        free(temp);
    }

    void print_Queue()
    {
        if(this->rear== this->front)  {
            cout << "The Queue is empty !!!!" << endl;
            return;
        }
        QueuePtr temp = this->front->next;
        while(temp != this->rear)
        {
            cout << temp->data.NetId << '\t';
            cout << "******************" << '\t';
            cout << temp->data.dist << endl;
            temp = temp->next;
        }
        cout << temp->data.NetId << '\t';
        cout << "******************" << '\t';
        cout << temp->data.dist << endl;
        printf("\n");
        cout << "the item_num is : " << this->item_num << endl;
    }

    QElemType GetTop()
    {
        QElemType e;
        e.NetId = -1;
        e.dist = -1;
        if(this->front == this->rear)
            return e;
        e = this->front->next->data;
        return e;
    }
    void Qswap(LinkQueue &P){
        QueuePtr tmp = NULL;
        long long sum_tmp;

        tmp = this->front;
        this->front = P.front;
        P.front = tmp;

        tmp = this->rear;
        this->rear = P.rear;
        P.rear = tmp;

        sum_tmp = this->sum;
        this->sum = P.sum;
        P.sum = sum_tmp;
    }
}LinkQueue;

/* **********************************    The end     **************************************** */

/* **********************************    generic     **************************************** */

struct ga_struct
{
	string  server;			// 1:biuld server here ;  0:not
	int serverNum;
	vector<int> server_id;
	int fitness;					// its fitness

    void clearup(){
        this->server.erase();
        this->serverNum = 0;
        this->fitness = 0;
        this->server_id.erase(this->server_id.begin());
        vector<int>().swap(this->server_id);
    }

    void print_ga(){
        cout << "the num of server is :  " << this->serverNum << endl;
        vector<int>:: iterator it;
        int i = 0;
        for(it = this->server_id.begin();it!=this->server_id.end();it++){
            cout <<"server["<< i << "]"<< '=' << (*it) << ';' <<'\n';
            i++;
        }
        cout << endl;
    }
};

/* **********************************    The end     **************************************** */


/* **********************************    main        **************************************** */

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);
void topo_init(char * topo[MAX_EDGE_NUM],int line_num,MCMF * cdn,MCFS * qiwei);

bool BellmanFord(int s,int t,int &flow,int &cost,MCMF *cdn);
void CompTrace(MCMF *cdn,char *trace);

int FinalMincost(int s,int t,MCMF *cdn,char *output);  // compute the path

bool Ga_For_MCF(MCFS * f_cdn,graph_init * clean_graph,char * output);
inspire_interface  MFCSolver(MCFS *cdn,vector<int> server,graph_init* clean_graph);

/* **********************************    The end     **************************************** */


/* *************************************************************************** */
/*
 * generic algorithm defination
 */

inline void calc_fitness(ga_vector &population,graph_init *clean_graph);
bool init_population(ga_vector &population, ga_vector &buffer,graph_init *clean_graph);
inline bool fitness_sort(ga_struct x, ga_struct y);
inline void sort_by_fitness(ga_vector &population);
void elitism(ga_vector &population, ga_vector &buffer, int esize );
inline void mutate(ga_struct &member);
void mate(ga_vector &population, ga_vector &buffer);
inline void print_best(ga_vector &gav);
inline void swap(ga_vector *&population, ga_vector *&buffer);


bool hanMing(ga_vector population, ga_struct citizen);
int hanMingDis(ga_struct popu_citizen,ga_struct citizen);
/* *************************************************************************** */


#endif
