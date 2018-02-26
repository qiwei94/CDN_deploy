#ifndef __ns_h__
#define __ns_h__

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cstddef>
#include <string.h>
#include <iostream>
#include <vector>
#include "lib_io.h"
using namespace std;

#define MAX_NODE_NUM 1005              //??????????????/?????????????待修改
#define ARC_CAP_MAX 100000           //??????????????/?????????????待修改
#define INF 2047483600
#define Candidate_num_little 30
#define Candidate_num_medium 50
#define Candidate_num_big 200
#define Hot_list_little 5
#define Hot_list_medium 10
#define Hot_list_big 20

#define NO_SOLUTION 0
#define QIWEI_FIND 1
#define Bukejie 2


#define Arc_Basic 0
#define Arc_Low 1
#define Arc_Up 2

struct NSarc;
struct NSnode;

template<class T>
T ABS( const T x )
{
    return( x >= T( 0 ) ? x : - x );
}

template<class T>
void Swap( T &v1, T &v2 )
{
    T temp = v1;
    v1 = v2;
    v2 = temp;
}

template <typename T>
class Get_Type_Max
{
public:
    Get_Type_Max() {}
    operator T()
    {
        return( std::numeric_limits<T>::max() );
    }
};

struct NSnode
{
    NSnode *pred;
    NSnode *thread;
    int depth;
    NSarc *Entering_Arc;
    int balance;
    int potential;
};


struct NSarc
{
    NSnode *from;
    NSnode *to;
    int cap;
    int flow;
    int cost;
    char flag;
};


struct Candidiate
{
    NSarc *arc;
    int absRC;
};

struct graph_init
{
    int G_n;
    int G_m;
    vector<int>  CAP;
    vector<int>  Deficit;  //node的收支
    vector<int> From;    //起始点
    vector<int> TO;    // 终止点
    vector<int>  COST;
    int server_cost;
    graph_init() {}
    void operator=(graph_init *a)
    {
        G_n=a->G_n;
        G_m=a->G_m;
        for(int i=0; i<a->G_m; i++)
        {
            CAP.push_back(a->CAP[i]);
            From.push_back(a->From[i]);
            TO.push_back(a->TO[i]);
            COST.push_back(a->COST[i]);
        }
        for(int i=0; i<a->G_n; i++)
        {
            Deficit.push_back(a->Deficit[i]);
        }
        server_cost = a->server_cost;
    }
};


struct inspire_interface
{
    vector<int> server_list;	//启发式给定的server list
    int link_cost;				//MCFS算完的立链路上的cost
    vector<int> server_used;	//用到的server
    vector<int> server_balance;	//用到的server上的balance
};

struct MCFS
{
    /*******************************basic of the graph**********************************************/
    int n;
    int node_max_num;
    int m;
    int arc_max_num;
    int status;

    /******************************* arc **********************************************/
    NSarc *arc_array_begin;
    NSarc *Fake_Arc;
    NSarc *arc_array_end;
    NSarc *Fake_Arc_End;

    /******************************* node **********************************************/
    NSnode *node_array_begin;
    NSnode *Fake_Root;
    NSnode *node_array_end;

    /******************************* candidate list *************************************/
    Candidiate *candy;
    int Candy_Group_Num;
    int Tmp_Candy_Size;
    int Now_Candy;
    int Candy_Size;
    int Hotlist_Size;
    int MAX_COST;

    /*构造函数*/
    MCFS( int nmx = 0, int mmx = 0 )
    {
        node_max_num = nmx;
        arc_max_num = mmx;
        n = m = 0;
        node_array_begin = NULL;
        arc_array_begin = NULL;
        candy = NULL;
        status = NO_SOLUTION;
        MAX_COST = int( 1e7 );

        if( node_max_num && arc_max_num )
            MAlloc();
        else
            node_max_num = arc_max_num = 0;
    }

    void MAlloc( void );
    void DAlloc( void );

    NSnode* Father( NSnode *n, NSarc *a );

    void Refresh_Tree( NSarc *h, NSarc *k, NSnode *h1, NSnode *h2, NSnode *k1, NSnode *k2 );

    void Potential( NSnode *r, int delta );

    int Cost_Reduce( NSarc *a );

    void InitNet(graph_init *graph);
    void ReadData(char * topo[MAX_EDGE_NUM],int line_num,graph_init *graph);
    void Add_Super_Arc(vector<int> server_list,graph_init* graph);


    void MllocCandy(void);
    void InitBase(void);
    void Init_Candy( void );

    NSarc* Candy_Pivot( void );
    void Simplex( void );

    long Get_Arc_Cost( void )
    {
        if( status == QIWEI_FIND )
        {
            long arc_cost = 0;
            NSarc *arc;
            for( arc = arc_array_begin ; arc != arc_array_end ; arc++ )
            {
                if( ( arc->flag == Arc_Basic ) || ( arc->flag == Arc_Up ) )
                    arc_cost += arc->cost * arc->flow;
            }
            for( arc = Fake_Arc ; arc != Fake_Arc_End ; arc++ )
            {
                if( ( arc->flag == Arc_Basic ) || ( arc->flag == Arc_Up ) )
                    arc_cost += arc->cost * arc->flow;
            }
            return arc_cost ;
        }

        else
        {
            cout<<"qiwei do not solved yet"<<endl;
            return( Get_Type_Max<long>() );
        }
    }

    void qiwei_solve(inspire_interface* inter)
    {
        Simplex();
        if( status == QIWEI_FIND )
        {
            //return MCFGetFO();
            inter->link_cost=Get_Arc_Cost();
            qiwei_server_used(inter);
        }
        else
        {
            /* cout<<"sorry , qiwei can not solve"<<endl; */
            inter->link_cost=INF;
        }
    }

    void qiwei_server_used(inspire_interface* inter)
    {
        int server_num=inter->server_list.capacity();
        NSarc* arc=arc_array_begin+m-server_num;
        for(arc=arc_array_begin+m-server_num; arc<arc_array_end; arc++)
        {
            int flow=arc->flow;
            int to=(arc->to)-node_array_begin;
            if(flow!=0)
            {
                inter->server_used.push_back(to);
                inter->server_balance.push_back(flow);
            }
        }
    }

};


#endif

















