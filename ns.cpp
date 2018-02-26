#include "ns.h"
#include "deploy.h"

void MCFS::Add_Super_Arc(vector<int> server_list,graph_init* graph)
{
    int server_num=server_list.capacity();
    /* cout<<"afaewfasfdwqaefaf server_num="<<server_num<<endl; */
    for(int i = 0; i<server_num; i++)
    {
        graph->From.push_back(graph->G_n);
        graph->TO.push_back(int(server_list[i]+1));
        graph->CAP.push_back(ARC_CAP_MAX);
        graph->COST.push_back(0);//super S 到 server cost =0
        graph->G_m++;
        /* cout<<"i="<<i<<" "<<Index(server_list[i]+1)<<endl; */
    }

}

void MCFS::ReadData(char * topo[MAX_EDGE_NUM],int line_num,graph_init *graph)
{
    int pNum[line_num][4];	//
    int tmp = 0;
    int k = 0;
    for(int ii = 0; ii < line_num; ii++)					//
        for(int jj = 0; jj < (int)strlen(topo[ii]); jj++)
            if(topo[ii][jj] == '\r' || topo[ii][jj] == '\n')
            {
                pNum[ii][k++]= tmp;
                tmp = 0;
                k = 0;
                break;
            }
            else if(topo[ii][jj] != ' ')
                tmp = tmp * 10 + (int)(topo[ii][jj] - 48);
            else
            {
                pNum[ii][k++]= tmp;
                tmp = 0;
            }
    ////////////////////////////////////////create the MCFS STRUCTURE//////////
    /* create cdn graph */
    /*
     * STEP 0: get edge_num,cus_num,node_num 0
     * STEP 1: get server cost 2
     * STEP 2: get edge 4～3+edge_num
     * STEP 3: get customer 5+edge_num ~ 4+edge_num+cus_num
     * start[4] = {0,2,4,5+edge_num}
     * end[4] = {0,2,4+edge_num,5+edge_num+cus_num}
     */
    int node_num = pNum[0][0];
    int edge_num = pNum[0][1];
    int cus_num = pNum[0][2];
    int server_cost = pNum[2][0];
    cout<<"********************node_num"<<node_num<<endl;

    int start[4] = {0,2,4,-1};
    int end[4] = {0,2,-1,-1};
    end[2] = 4 + edge_num;	//边情况end
    start[3] = 5 + edge_num;  //消费节点begin
    end[3] = 5 + edge_num + cus_num;//消费节点end

    graph->G_n=node_num+1;//The number of nodes Add one node:Surper S
    graph->G_m=edge_num*2;//The number of arcs twice the num of edge and the num of server
    graph->server_cost = server_cost;
    cout<<"the Index tm=="<<graph->G_m<<endl;


    for( int i = 0 ; i < graph->G_n ; i++ )           // all deficits are 0
    {
        graph->Deficit.push_back(0);                      // unless otherwise stated
    }

    /************************NODE INFO*******************************/
    int sum_supply=0;
    int connect_node_num=0;
    int node_demand=0;
    int i=0;
    for(i = start[3]; i < end[3]; i++) //消费节点情况
    {
        connect_node_num=pNum[i][1];
        node_demand=pNum[i][2];		//demand or supply ?????????????????????????????
        graph->Deficit[connect_node_num] -= (-node_demand);
        sum_supply+=node_demand;
        //cout<<"sum_supply==="<<sum_supply<<endl;
    }

    graph->Deficit[graph->G_n-1]=(-(sum_supply));//最后那个节点作为超级源节点
    //cout<<"i+1="<<i+1<<" tDfct[i+1]"<<tDfct[i+1]<<endl;
    /*************************ARC INFO*******************************/
    int arc_begin=0;
    int arc_end=0;
    int arc_cap=0;
    int arc_cost=0;
    int j=0;


    for(int i = start[2]; i < end[2]; i++)
    {
        arc_begin=pNum[i][0];
        arc_end=pNum[i][1];
        arc_cap=pNum[i][2];
        arc_cost=pNum[i][3];
        graph->From.push_back(arc_begin+1);
        graph->TO.push_back(arc_end+1);
        graph->CAP.push_back(arc_cap);
        graph->COST.push_back(arc_cost);

        //add both the direction arc
        graph->From.push_back(arc_end+1);
        graph->TO.push_back(arc_begin+1);
        graph->CAP.push_back(arc_cap);
        graph->COST.push_back(arc_cost);
    }

}

void MCFS::InitNet(graph_init *graph)
{
    int nmx=graph->G_n;
    int mmx=graph->G_m;
    int pn=graph->G_n;
    int pm=graph->G_m;
    int* pU=&(graph->CAP[0]);
    int* pC=&(graph->COST[0]);
    int* pDfct=&(graph->Deficit[0]);
    int* pSn=&(graph->From[0]);
    int* pEn=&(graph->TO[0]);

    delete[] candy;
    candy = NULL;

    if( ( nmx != node_max_num ) || ( mmx != arc_max_num ) ) //图改变之后，看两者大小是否一样
    {
        if( node_max_num && arc_max_num )     //如果内存已经分配了
        {
            DAlloc();
            node_max_num = arc_max_num = 0;
        }

        if( nmx && mmx )
        {
            node_max_num = nmx;
            arc_max_num = mmx;
            MAlloc();
        }
    }
    if( ( ! node_max_num ) || ( ! arc_max_num ) )
        // 如果有一个内存没有分配正确，那就都设为0
        node_max_num = arc_max_num = 0;

    if( node_max_num )    // if the new dimensions of the memory are correct，如果两者内存都分配正确
    {
        n = pn;     //图中存放的点的总数设为pn
        m = pm;     //图中的边数

        node_array_end = node_array_begin + n;
        Fake_Root = node_array_begin + node_max_num;//
        NSnode *Fake_Root;

        for( NSnode *NSnode = node_array_begin ; NSnode != node_array_end ; NSnode++ )
            NSnode->balance = pDfct[ NSnode - node_array_begin ];  // initialize nodes 初始化节点的balance

        arc_array_end = arc_array_begin + m;
        Fake_Arc = arc_array_begin + arc_max_num;
        Fake_Arc_End = Fake_Arc + n;
        /* cout<<"qiwei_stopDummyP= "<<n<<" "<<dummyArcsP<<endl; */
        for( NSarc *arc = arc_array_begin ; arc != arc_array_end ; arc++ )
        {
            //这里是最重要的函数，是为了将图形放到结构体中
            arc->cost = pC[ arc - arc_array_begin ];
            arc->cap = pU[ arc - arc_array_begin ];
            arc->from = node_array_begin + pSn[ arc - arc_array_begin ] - 1;
            arc->to = node_array_begin + pEn[ arc - arc_array_begin ] - 1;
        }

        MllocCandy();

        status = NO_SOLUTION;
    }
    InitBase();
}

void MCFS::MllocCandy(void) //对于不同规模的图，使用不同的候选者ule参数
{
    if( m < 10000 )
    {
        Candy_Size = Candidate_num_little;
        Hotlist_Size = Hot_list_little;
    }
    else if( m > 100000 )
    {
        Candy_Size = Candidate_num_big;
        Hotlist_Size = Hot_list_big ;
    }
    else
    {
        Candy_Size = Candidate_num_medium;
        Hotlist_Size = Hot_list_medium;
    }

    candy = new Candidiate[ Hotlist_Size + Candy_Size + 1 ];

}

void MCFS::DAlloc(void)
{

    delete[] node_array_begin;
    delete[] arc_array_begin;
    node_array_begin = NULL;
    arc_array_begin = NULL;

    delete[] candy;
    candy = NULL;
}

void MCFS::MAlloc( void )
{
    node_array_begin = new NSnode[ node_max_num + 1 ];
    arc_array_begin = new NSarc[ arc_max_num + node_max_num ];
    Fake_Arc = arc_array_begin + arc_max_num;
}

NSnode* MCFS::Father( NSnode *n, NSarc *a )
{
    if( a == NULL )
        return NULL;

    if( a->from == n )
        return( a->to );
    else
        return( a->from );
}



void MCFS::Refresh_Tree( NSarc *h, NSarc *k, NSnode *h1, NSnode *h2, NSnode *k1, NSnode *k2 )
{
    int delta = (k1->depth) + 1 - (k2->depth);
    NSnode *root = k2;
    NSnode *dad;
    NSnode *previousNode = k1;
    NSnode *lastNode;
    NSarc *arc1 = k;
    NSarc *arc2;
    bool fine = false;
    while( fine == false )
    {
        if( root == h2 )
            fine = true;
        dad = Father( root, root->Entering_Arc );
        int level = root->depth;
        NSnode *node = root;
        while ( ( node->thread ) && ( ( node->thread )->depth > level ) )
        {
            node = node->thread;
            node->depth = node->depth + delta;
        }

        root->depth = root->depth + delta;

        if( root->pred )
            ( root->pred )->thread = node->thread;
        if( node->thread )
            ( node->thread )->pred = root->pred;
        lastNode=node;

        NSnode *nextNode = previousNode->thread;
        root->pred = previousNode;
        previousNode->thread = root;
        lastNode->thread = nextNode;
        if( nextNode )
            nextNode->pred = lastNode;

        previousNode = lastNode;
        delta = delta + 2;
        arc2 = root->Entering_Arc;
        root->Entering_Arc = arc1;
        arc1 = arc2;
        root = dad;
    }
}


void MCFS::Potential( NSnode *r, int delta )
{
    int level = r->depth;
    NSnode *n = r;
	n->potential = n->potential + delta;
	n = n->thread;
    while ( ( n ) && ( n->depth > level ) )
    {
        n->potential = n->potential + delta;
        n = n->thread;
    }
}


int MCFS::Cost_Reduce( NSarc *tmp )
{
    int reduce = (tmp->from)->potential - (tmp->to)->potential;
    reduce = reduce + tmp->cost;
    return( reduce );
}


void MCFS::InitBase(void)
{
    NSarc *arc;
    NSnode *NSnode;
    for(arc = arc_array_begin; arc != arc_array_end; arc++)
    {
        arc->flow=0;
        arc->flag=Arc_Low;
    }
    for(arc=Fake_Arc; arc!=Fake_Arc_End; arc++)
    {
        NSnode = node_array_begin + ( arc - Fake_Arc );
        if( NSnode->balance > 0 )
        {
            arc->from = Fake_Root;
            arc->to = NSnode;
            arc->flow = NSnode->balance;
        }
        else
        {
            arc->from = NSnode;
            arc->to = Fake_Root;
            arc->flow = -NSnode->balance;
        }
        arc->cost = MAX_COST;
        arc->flag = Arc_Basic;
        arc->cap = Get_Type_Max<int>();
    }
    Fake_Root->balance = 0;
    Fake_Root->pred = NULL;
    Fake_Root->thread = node_array_begin;
    Fake_Root->Entering_Arc = NULL;
    Fake_Root->potential = MAX_COST;
    Fake_Root->depth = 0;
    for( NSnode = node_array_begin ; NSnode != node_array_end ; NSnode++)
    {
        NSnode->pred = NSnode - 1;
        NSnode->thread = NSnode + 1;
        NSnode->Entering_Arc = Fake_Arc + (NSnode - node_array_begin);
        if( NSnode->balance > 0 )
            NSnode->potential = 2 * MAX_COST;
        else
            NSnode->potential = 0;

        NSnode->depth = 1;
    }

    node_array_begin->pred = Fake_Root;
    ( node_array_begin + n - 1 )->thread = NULL;
}


void MCFS::Init_Candy( void )
{
    Candy_Group_Num = ( ( m - 1 ) / Candy_Size ) + 1;
    Now_Candy = 0;
    Tmp_Candy_Size = 0;
}


NSarc* MCFS::Candy_Pivot( void )
{
    int next = 0;
    int i;
    int minimeValue;
    if( Hotlist_Size < Tmp_Candy_Size )
        minimeValue = Hotlist_Size;
    else
        minimeValue = Tmp_Candy_Size;

    for( i = 2 ; i <= minimeValue ; i++ )
    {
        NSarc *arc = candy[i].arc;
        int red_cost = Cost_Reduce( arc );

        if((( red_cost < 0) && ( arc->flag == Arc_Low ) ) ||
                ((red_cost>0) && ( arc->flag == Arc_Up ) ) )
        {
            next++;
            candy[ next ].arc = arc;
            candy[ next ].absRC = ABS( red_cost );
        }
    }

    Tmp_Candy_Size = next;
    int oldGroupPos = Now_Candy;
    do
    {
        NSarc *arc;
        for( arc = arc_array_begin + Now_Candy ; arc < arc_array_end ; arc += Candy_Group_Num )
        {
            if( arc->flag == Arc_Low )
            {
                int red_cost = Cost_Reduce( arc );
                if( red_cost < 0 )
                {
                    Tmp_Candy_Size++;
                    candy[ Tmp_Candy_Size ].arc = arc;
                    candy[ Tmp_Candy_Size ].absRC = ABS( red_cost );
                }
            }
            else if( arc->flag == Arc_Up )
            {
                int red_cost = Cost_Reduce( arc );
                if(red_cost>0)
                {
                    Tmp_Candy_Size++;
                    candy[ Tmp_Candy_Size ].arc = arc;
                    candy[ Tmp_Candy_Size ].absRC = ABS( red_cost );
                }
            }
        }

        Now_Candy++;
        if( Now_Candy == Candy_Group_Num )
            Now_Candy = 0;

    }
    while( ( Tmp_Candy_Size < Hotlist_Size ) && ( Now_Candy != oldGroupPos ) );

    if( Tmp_Candy_Size )
    {

        int tmp=candy[ 1 ].absRC;
        NSarc *tmp_arc=candy[1].arc;
        for(int i=1; i<Tmp_Candy_Size+1; i++)
        {
            if(candy[i].absRC>tmp)
            {
                tmp=candy[i].absRC;
                tmp_arc=candy[i].arc;
            }
        }
        return tmp_arc;
    }
    else
        return( NULL );
}


void MCFS::Simplex( void )
{

    status = NO_SOLUTION;
    NSarc *enteringArc;
    NSarc *leavingArc;
    Init_Candy();

    while( status == NO_SOLUTION )      //还没找到可行解
    {
        enteringArc = Candy_Pivot();	//从候选者列表中找出entering arc
        /*
        1、find the leaving arc
        这条entering arc，构成了cycle w，也就是pivot cycle
        w中包含T中从K到L的唯一的一条路径
        若(K,L)属于L，将w的方向定为和(K,L)一样
        若(K,L)属于U，将w的方向定为和(K,L)反向
        正边:theta=u-x
        负边:theta=x

        theta>0:nondegenerate
        theta=0:degenarate

        找到pivot cycle:从K和L分别向上找，找到第一个相交的点，做法用到了一个技巧：利用深度
        从K和L向上找，直到找到在同一层，且是同一个的点，这个点就是当前的共同祖先点。
        在这个pivot cycle中进行流量增广，找到leaving arc。

        2、updating the tree
        找到leaving tree之后，update tree：leaving arc被删除之后，整个树被分为两部分，有root
        的被称为T1，剩下的部分称为T2，同样(K,L)也各有一个点在T1和T2中，并且PI(root)=0，利用depth
        和thread可以快速更新每个node的potential.
        */
        if( enteringArc )
        {
            NSarc *arc;
            NSnode *K;
            NSnode *L;
            int t;
            int theta;	//用来保存entering上的流量
            if( enteringArc->flag == Arc_Up )	//如果enteringarc是UP，cycle的方向和entering arc相同，且theta=x
            {
                K = enteringArc->to;
                L = enteringArc->from;
                theta = enteringArc->flow;
            }
            else	//否则方向相反，theta=U-X
            {
                K = enteringArc->from;
                L = enteringArc->to;
                theta = enteringArc->cap - enteringArc->flow;
            }

            NSnode *memK1 = K;
            NSnode *memK2 = L;
            leavingArc = NULL;



            //向下是为了寻找leaving ARC
            bool leavingReducesFlow = Cost_Reduce( enteringArc )>0;
            //计算theta,找到eaving arc 和 cycle的root
            bool leave;

            while( K != L )	//确定pivot cycle
            {
                if( K->depth > L->depth )
                {
                    arc = K->Entering_Arc;
                    if( arc->from != K )
                    {
                        t = arc->cap - arc->flow;
                        leave = false;
                    }
                    else
                    {
                        t = arc->flow;
                        leave = true;
                    }

                    if( t < theta )
                    {
                        theta = t;
                        leavingArc = arc;
                        leavingReducesFlow = leave;
                    }

                    K = Father( K, arc );
                }
                else
                {
                    arc = L->Entering_Arc;
                    if( arc->from == L )
                    {
                        t = arc->cap - arc->flow;
                        leave = false;
                    }
                    else
                    {
                        t = arc->flow;
                        leave = true;
                    }

                    if( t <= theta )
                    {
                        theta = t;
                        leavingArc = arc;
                        leavingReducesFlow = leave;
                    }

                    L = Father(L, arc);
                }
            }



            if( leavingArc == NULL )
                leavingArc = enteringArc;


            /***********************************update the flow********************************************/
            // Update flow with "theta" and UPdate tree
            K = memK1;
            L = memK2;

            if(theta!=0)
            {
                if( enteringArc->from == K )
                    enteringArc->flow = enteringArc->flow + theta;
                else
                    enteringArc->flow = enteringArc->flow - theta;

                while( K != L )
                {
                    if( K->depth > L->depth )
                    {
                        arc = K->Entering_Arc;
                        if( arc->from != K )
                            arc->flow = arc->flow + theta;
                        else
                            arc->flow = arc->flow - theta;

                        K = Father(K, K->Entering_Arc);
                    }
                    else
                    {
                        arc = L->Entering_Arc;
                        if( arc->from == L )
                            arc->flow = arc->flow + theta;
                        else
                            arc->flow = arc->flow - theta;

                        L = Father(L, L->Entering_Arc);
                    }
                }
            }



            /***********************************update the tree********************************************/

            if( enteringArc != leavingArc )
            {
                bool leavingBringFlowInT2 = ( leavingReducesFlow ==( ( leavingArc->from )->depth > ( leavingArc->to )->depth ) );
                if( leavingBringFlowInT2 == ( memK1 == enteringArc->from ) )
                {
                    L = enteringArc->from;
                    K = enteringArc->to;
                }
                else
                {
                    L = enteringArc->to;
                    K = enteringArc->from;
                }
            }

            if( leavingReducesFlow )
                leavingArc->flag = Arc_Low;
            else
                leavingArc->flag = Arc_Up;

            if( leavingArc != enteringArc )
            {
                enteringArc->flag = Arc_Basic;
                NSnode *h1;
                NSnode *h2;
                if( ( leavingArc->from )->depth < ( leavingArc->to )->depth )
                {
                    h1 = leavingArc->from;
                    h2 = leavingArc->to;
                }
                else
                {
                    h1 = leavingArc->to;
                    h2 = leavingArc->from;
                }

                Refresh_Tree(leavingArc, enteringArc, h1, h2, K, L);
                L = enteringArc->to;
                int delta = Cost_Reduce(enteringArc);
                if( ( enteringArc->from )->depth > ( enteringArc->to )->depth )
                {
                    delta = -delta;
                    L = enteringArc->from;
                }

                Potential( L, delta );
            }

        }
        else
        {
            status = QIWEI_FIND;
            for( NSarc *arc = Fake_Arc ; arc != Fake_Arc_End ; arc++ )
                if(arc->flow>0)
                    status = Bukejie;

        }
    }
}

