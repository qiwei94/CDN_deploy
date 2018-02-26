/*
 * *************************************************************************
 *                             HuaWei Code
 *
 * *************************************************************************
 */
#include "deploy.h"
/* **********************        generic        ************************** */
float GA_ELITRATE = 0.10;
float GA_MUTATIONRATE = 0.25;
int GA_POPSIZE = 90;

/* **********************        hanming        ************************** */
float HanMingDis = 0.9;
int PopInitRate = 50;

/* **********************       time_limit      ************************** */
int TIME_LIMIT = 85;

/* **********************        control        ************************** */
int GA_TARSIZE = 0;
int is_first_calcu = 1;


void topo_init(char * topo[MAX_EDGE_NUM],int line_num,MCMF * cdn,MCFS * qiwei){
    int pNum[line_num][4];
    int tmp = 0;
    int k = 0;
    for(int i = 0;i < line_num;i++)
        for(int j = 0;j < (int)strlen(topo[i]);j++)
            if(topo[i][j] == '\r' || topo[i][j] == '\n'){
                pNum[i][k++]= tmp;
                tmp = 0;
                k = 0;
                break;
            }
            else if(topo[i][j] != ' ')
                tmp = tmp * 10 + (int)(topo[i][j] - 48);
            else{
                pNum[i][k++]= tmp;
                tmp = 0;
            }
    /* create cdn graph */
    /*
     * case 0: get edge_num,cus_num,net_num 0
     * case 1: get server cost 2
     * case 2: get edge 4～3+edge_num
     * case 3: get customer 5+edge_num ~ 4+edge_num+cus_num
     * start[4] = {0,2,4,5+edge_num}
     * end[4] = {0,2,4+edge_num,5+edge_num+cus_num}
     */
    /* case 0 */
    int net_num = pNum[0][0];
    int edge_num = pNum[0][1];
    int cus_num = pNum[0][2];

    cdn->init(net_num + 2);
    cdn->s = net_num;
    cdn->t = net_num +1;

    /* case 1 */
    int server_cost = pNum[2][0];
    cdn->server_cost = server_cost;
    /* case 2,3 pre */
    int start[4] = {0,2,4,-1};
    int end[4] = {0,2,-1,-1};
    start[3] = 5 + edge_num;
    end[2] = 4 + edge_num;
    end[3] = 5 + edge_num + cus_num;
    /* case 2 */
    NSarc *NSarc;

    for( NSarc = qiwei->arc_array_begin ; NSarc != qiwei->arc_array_end ; NSarc++ )
    {
        if(NSarc->flow!=0)
        {
            int from=(NSarc->from)-(qiwei->node_array_begin);
            int to=(NSarc->to)-(qiwei->node_array_begin);
            int flow=NSarc->flow;
            int cost=NSarc->cost;
            // cout<<"wei  from="<<from<<" to="<<to<<" flow="<<flow<<" cost="<<cost<<endl;
            // cout<<endl;

            if(to == cdn->s)
                cdn->AddSuperS(to);
            else
                cdn->AddEdge(from,to,flow,cost);
        }

    }

    /* case 3 */
    for(int i = start[3];i < end[3];i++){
        cdn->AddSuperT(pNum[i][1],pNum[i][2]);
        cdn->SaveCustomer(pNum[i][0],pNum[i][1]);
    }

}

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
char * topo_file = new char[MAX_EDGE_NUM](); MCMF *cdn = new MCMF;
    MCFS *f_cdn = new MCFS;

/* ***************************************************************************** */
    graph_init *clean_graph=new graph_init;
    MCFS* qiwei=new MCFS;
    qiwei->ReadData(topo,line_num,clean_graph);//读数据到graph中
    GA_TARSIZE = clean_graph->G_n - 1;

/* ***************************      Time Limit      **************************** */
    int TopoSize = 0;

    if(GA_TARSIZE < 300)
        TopoSize = 1;
    else if(GA_TARSIZE < 700)
        TopoSize = 2;
    else
        TopoSize = 3;

    switch(TopoSize){
        case 0:
            break;
        case 1:
            GA_ELITRATE = 0.05;
            GA_MUTATIONRATE = 0.6;
            GA_POPSIZE = 90;
            PopInitRate = 20;
            HanMingDis = 0.8;
            TIME_LIMIT = 30;
            break;
        case 2:
            GA_ELITRATE = 0.05;
            GA_MUTATIONRATE = 0.6;
            GA_POPSIZE = 70;
            PopInitRate = 10;
            HanMingDis = 0.9;
            TIME_LIMIT = 50;
            break;
        case 3:
            GA_ELITRATE = 0.05;
            GA_MUTATIONRATE = 0.6;
            GA_POPSIZE = 70;
            PopInitRate = 10;
            HanMingDis = 0.85;
            TIME_LIMIT = 85;
            break;
        default:
            break;
    }
    // cout << "GA_ELITRATE" << '\t' << "GA_MUTATIONRATE" << '\t' << "GA_POPSIZE" << '\t' << "PopInitRate" << '\t' << "HanMingDis" << '\t' << "TIME_LIMIT" << endl;
    // cout << GA_ELITRATE << '\t' << GA_MUTATIONRATE << '\t' << GA_POPSIZE << '\t' << PopInitRate << '\t' << HanMingDis << '\t' << TIME_LIMIT << endl;
    cout << "GA_ELITRATE" << '\t' << GA_ELITRATE << endl;
    cout << "GA_MUTATIONRATE" << '\t' << GA_MUTATIONRATE << endl;
    cout << "GA_POPSIZE" << '\t' << GA_POPSIZE << endl;
    cout << "PopInitRate" << '\t' << PopInitRate << endl;
    cout << "HanMingDis" << '\t' << HanMingDis << endl;
    cout << "TIME_LIMIT" << '\t' << TIME_LIMIT << endl;



/* ***************************************************************************** */

    int cost = INF;
    if(Ga_For_MCF(f_cdn,clean_graph,topo_file)){
        topo_init(topo,line_num,cdn,f_cdn);

        cost = FinalMincost(cdn->s,cdn->t,cdn,topo_file);
    }
    cout << "The cost of spfa is :" << cost <<endl;


    write_result(topo_file, filename);

    delete cdn;
    delete qiwei;

}

void InsertServer(MCMF * cdn,vector<int> server_id){
    int n = server_id.size();
    for(int i = 0;i < n;i++){
        cdn->AddSuperS(server_id[i]);
    }
}

bool BellmanFord(int s,int t,int &flow,int &cost,MCMF *cdn){
    for(int i = 0;i < cdn->n;i++) cdn->d[i]=INF;
    memset(cdn->inq,0,sizeof(cdn->inq));
    cdn->d[s] = 0;
    cdn->inq[s] = 1;
    cdn->p[s] = 0;
    cdn->a[s] = INF;

/* *******************     The min        ******************* */
    LinkQueue Q;
    Q.init_Queue();
    QElemType source = {cdn->s,0};
    Q.en_Queue_Rear(source);
    while(!Q.is_Empty()){
        QElemType to = Q.GetTop();

        Q.de_Queue();
        int u = to.NetId;

        cdn->inq[u] = 0;
        int Gsize = (int)cdn->G[u].size();
        for(int i = 0;i < Gsize;i++){
            Edge& e = cdn->edges[cdn->G[u][i]];
            if(e.cap > e.flow && cdn->d[e.to] > cdn->d[u] + e.cost){
                cdn->a[e.to] = min(cdn->a[u],e.cap - e.flow);
                cdn->d[e.to] = cdn->d[u] + e.cost;
                cdn->p[e.to] = cdn->G[u][i];
                if(!cdn->inq[e.to] && cdn->d[e.to] <= cdn->d[t]) {
                    QElemType q_in;
                    QElemType q_front;
                    q_in.NetId = e.to;
                    q_in.dist = cdn->d[e.to];
                    if(!Q.is_Empty()){
                        q_front = Q.GetTop();
                        if(abs(q_in.dist) > abs(q_front.dist)){
                            Q.en_Queue_Rear(q_in);
                        }
                        else{
                            Q.en_Queue_Top(q_in);
                        }
                    }
                    else{
                        Q.en_Queue_Rear(q_in);
                    }
                    cdn->inq[e.to] = 1;
                }
            }
            if(cdn->d[t] == 0)  break;
        }

        if(cdn->d[t] == 0)  break;
    }

/* *******************     The end    ******************* */

    if(cdn->d[t] == INF) return false;
    int u = t;
    while(u != s){

        cdn->edges[cdn->p[u]].flow += cdn->a[t];
        cdn->edges[cdn->p[u]^1].flow -= cdn->a[t];
        cdn->edge_check(&cdn->edges[cdn->p[u]]);
        cdn->edge_check(&cdn->edges[cdn->p[u]^1]);
        u = cdn->edges[cdn->p[u]].from;

        /* update the new cost 0 for an fixed server */
        if(cdn->edges[cdn->p[u]].from == s){
                /* update flow and cost in this turn */
                flow += cdn->a[t];
                cost += cdn->d[t] * cdn->a[t];
                /* ***************************************** */
        }
        /* ***************************************** */
    }

    return true;

}

void CompTrace(MCMF *cdn,char *trace){  // for the output of trace
    int s = cdn->s;
    int t = cdn->t;
    int u = t;
    stack<int> tmp_trace;
    tmp_trace.push(u);
    while(u != s){
        u = cdn->edges[cdn->p[u]].from;
        tmp_trace.push(u);
    }
    char *str = new char[MAX]();
    int record = -1;
    while(!tmp_trace.empty()){
        u = tmp_trace.top();
        tmp_trace.pop();
        if(u != s && u != t){
            sprintf(str,"%d",u);
            if(record == -1){
                sprintf(trace,"%s%s",trace,str);
            }
            else{
                sprintf(trace,"%s%s%s",trace," ",str);
            }
            record = u;
        }
    }
    /* save the customer num. and added flow in this turn. */
    sprintf(str,"%d",cdn->cus[record]);
    sprintf(trace,"%s%s%s",trace," ",str);

    sprintf(str,"%d",cdn->a[t]);
    sprintf(trace,"%s%s%s",trace," ",str);
    /* *************************************************** */
    delete str;
}

int FinalMincost(int s,int t,MCMF *cdn,char *output){
    int flow = 0,cost = 0;
    int n = 0;
    char * ttmp = new char[MAX_EDGE_NUM]();
    char * trace = new char[MAX]();
    char * str = new char[MAX]();
    while(BellmanFord(s,t,flow,cost,cdn))
    {
        CompTrace(cdn,trace);
        n++;
        sprintf(ttmp,"%s%s%s",ttmp,"\n",trace);
        memset(trace,0,sizeof(*trace));
    }
    /* compute server cost first */
    cost += (cdn->G[s]).size() * cdn->server_cost;
    /* **************************** */
    /* check the correctness of the flow */
    if(!cdn->FlowCheck(flow))
        cost = INF;

    /* ******************************* */
    sprintf(str,"%d",n);
    sprintf(output,"%s%s%s%s",str,"\n",ttmp,"\0");
    delete ttmp;
    delete trace;
    delete str;

    return cost;

}

inspire_interface  MFCSolver(MCFS *cdn,vector<int> server,graph_init* clean_graph){

    graph_init tmp = *clean_graph;
    graph_init *dirt_graph = &tmp;

    inspire_interface result;

    result.server_list=server;
    cdn->Add_Super_Arc(server,dirt_graph);		//加server到graph中	junwei need
    cdn->InitNet(dirt_graph);					//用graph初始化MCFS
    cdn->qiwei_solve(&result);

    return result;
}

/* ********************************************************************************** */
/*
 * genetic algorithm defination
 */

ga_struct init_key_serevr(graph_init *clean_graph){
    ga_struct citizen;
    vector<int> ::iterator it;

    citizen.serverNum = 0;
    citizen.fitness = 0;
    citizen.server_id.clear();
    citizen.server.erase();

    for(int i=0; i<clean_graph->G_n-1; i++)
    {
        if(clean_graph->Deficit[i]!=0)
        {
            citizen.server_id.push_back(i);
            citizen.serverNum ++;
        }
    }

    for(int k =0;k<GA_TARSIZE;k++)
        citizen.server += '0';
    for(it = citizen.server_id.begin();it != citizen.server_id.end();it++)
        citizen.server[*it] = '1';

    cout << "*******************" << endl;
    cout << citizen.server << endl;
    cout << "*******************" << endl;

    return citizen;
}

// void init_population(ga_vector &population, ga_vector &buffer,graph_init *clean_graph)
// {
	// int tsize = GA_TARSIZE;
    // population.push_back(init_key_serevr(clean_graph));
	// for (int i=1; i<GA_POPSIZE; i++) {
		// ga_struct citizen;
		// citizen.serverNum = 0;
		// citizen.fitness = 0;
		// citizen.server_id.clear();

		// citizen.server.erase();
		// for (int j=0; j<tsize; j++){
            // citizen.server += ((1+rand()%100) > 50)?'1':'0';
			// if(citizen.server[j]=='1'){
				// citizen.serverNum++;
				// citizen.server_id.push_back(j);
			// }
		// }

		// population.push_back(citizen);
	// }
	// buffer.resize(GA_POPSIZE);
// }

bool init_population(ga_vector &population, ga_vector &buffer,graph_init *clean_graph)
{
    int tsize = GA_TARSIZE;
    // 首选在所有的client进行布点
    population.push_back(init_key_serevr(clean_graph));
    int count = 0;
    for (int i=1; i<GA_POPSIZE; i++) {
        count ++;
        ga_struct citizen;
        citizen.serverNum = 0;
        citizen.fitness = 0;
        citizen.server_id.clear();

        citizen.server.erase();
        for (int j=0; j<tsize; j++){
            citizen.server += ((1+rand()%100) > PopInitRate)?'1':'0';
            if(citizen.server[j]=='1'){
                citizen.serverNum++;
                citizen.server_id.push_back(j);
            }
        }
        if (hanMing(population,citizen)){
            population.push_back(citizen);
        }
        else
            i--;
        if(count > 500)
            return false;
    }
    // cout << "The count is :           " <<  count << endl;
    buffer.resize(GA_POPSIZE);
    return true;
}
//如果相同字符个数大于长度的一半，则不采用，return false
bool hanMing(ga_vector population, ga_struct citizen)
{
    int tsize = GA_TARSIZE;
    for (int i=0;i<population.size();i++){
        if (hanMingDis(population[i],citizen) > tsize * HanMingDis)
            return false;
    }
    return true;
}

//返回字符相同的个数
int hanMingDis(ga_struct popu_citizen,ga_struct citizen)
{
    int tsize = GA_TARSIZE;
    int count = 0;
    for (int i=0;i<tsize;i++){
        if (popu_citizen.server[i] == citizen.server[i])
            count += 1;
    }
    return count;
}

inline void calc_fitness(ga_vector &population,graph_init *clean_graph)
{

    int esize = 0;

    /* *******    elite population need not to calculate conce more     ******* */
    if(is_first_calcu){
        esize = 0;
        is_first_calcu = 0;
    }
    else
        esize = GA_POPSIZE * GA_ELITRATE;

    /* ************************************************************************ */

    for (int i=esize; i<GA_POPSIZE; i++) {
        MCFS* cdn=new MCFS;
        inspire_interface result = MFCSolver(cdn,population[i].server_id,clean_graph);
        cdn->DAlloc();
        delete cdn;
        if(result.link_cost == INF)
            population[i].fitness = INF;
        else{
            population[i].fitness = result.link_cost + result.server_used.size()*clean_graph->server_cost;
            population[i].server_id = result.server_used;
            population[i].serverNum = result.server_used.size();
            // cout <<"link.cost is :             "<< result.link_cost << endl;
            // cout <<"size is :                  "<< result.server_used.size() << endl;
            // cout <<"server_cost is :           "<<  clean_graph->server_cost << endl;
            // cout << "The cost is :             "<<population[i].fitness << endl;
            vector<int> ::iterator it;
            for(int k =0;k<GA_TARSIZE;k++)
                population[i].server[k] = '0';
            for(it = population[i].server_id.begin();it != population[i].server_id.end();it++)
                population[i].server[*it] = '1';
        }
    }
}

inline bool fitness_sort(ga_struct x, ga_struct y) {
	return (x.fitness < y.fitness);
}

inline void sort_by_fitness(ga_vector &population){
	sort(population.begin(), population.end(), fitness_sort);
}

void elitism(ga_vector &population, ga_vector &buffer, int esize ){
	for (int i=0; i<esize; i++) {
		buffer[i].serverNum = population[i].serverNum;
		buffer[i].server = population[i].server;
		buffer[i].fitness = population[i].fitness;
		buffer[i].server_id = population[i].server_id;
	}
}

inline void mutate(ga_struct &member){
	int tsize = GA_TARSIZE;
	int ipos = rand() % tsize;
	if(member.server[ipos]=='1') member.server[ipos] = '0';
	else member.server[ipos] = '1';
}

int SetParent(ga_vector &population){
    int TotalFitness = 0;
    int EachFitnes[GA_POPSIZE/2];
    int cnt;
    for(int i = 0;i < GA_POPSIZE/2;i++){
        // cout << "*************************:    "<< i << " : " <<population[i].fitness << endl;
        EachFitnes[i] = INF/population[i].fitness;
        TotalFitness += EachFitnes[i];
    }
    int dRange = rand() % TotalFitness;
    int dCursor = 0;
    for(cnt = 0;cnt < GA_POPSIZE/2;cnt++){
        dCursor += EachFitnes[cnt];
        if(dCursor > dRange)
            break;
    }
    return cnt;
}

void mate(ga_vector &population, ga_vector &buffer){
    int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = GA_TARSIZE, spos, i1, i2;
	elitism(population, buffer, esize);

	for (int i=esize; i<GA_POPSIZE; i++) {
        /* ***********************    seclection   *********************** */
        // i1 = rand() % (GA_POPSIZE / 2);
        // i2 = rand() % (GA_POPSIZE / 2);
        i1 = SetParent(population);
        i2 = SetParent(population);
        // cout << i1 << endl;
        // cout << i2 << endl;
        /* ***********************       end       *********************** */

        /* ***********************    crossover    *********************** */
		spos = rand() % tsize;
        buffer[i].server.erase();
        buffer[i].server = population[i1].server.substr(0, spos) +
                        population[i2].server.substr(spos, tsize - spos);

        /* ***********************       end       *********************** */
        // muntrace();

		if (rand() < GA_MUTATION) mutate(buffer[i]);
		buffer[i].server_id.clear();
		buffer[i].serverNum = 0;

		for (int j = 0; j < tsize; ++j)
			if(buffer[i].server[j]=='1'){
				buffer[i].serverNum++;
				buffer[i].server_id.push_back(j);
			}

	}
}

inline void print_best(ga_vector &gav)
{ cout << "Best: " << gav[0].server << " (" << gav[0].fitness << ")" << endl; }

inline void swap(ga_vector *&population, ga_vector *&buffer){
	ga_vector *temp = population; population = buffer; buffer = temp;
}


/* ********************************************************************************** */

/* ***************************   main solver     ************************************ */

bool Ga_For_MCF(MCFS * f_cdn,graph_init * clean_graph,char * output){
	srand(unsigned(time(NULL)));
    time_t begin,end;
	ga_vector pop_alpha, pop_beta;
	ga_vector *population, *buffer;
    if(!init_population(pop_alpha, pop_beta,clean_graph)){
        // cout << "The init of population failed!" << endl;
        return false;
    }
	population = &pop_alpha;
	buffer = &pop_beta;
    begin = time(NULL);
	for (int i=0; i<GA_MAXITER; i++) {
		calc_fitness(*population,clean_graph);		// calculate fitness
		sort_by_fitness(*population);				// sort them
		mate(*population, *buffer);					// mate the population together
		swap(population, buffer);					// swap buffers
        end = time(NULL);
        if(end - begin > TIME_LIMIT){
            break;
            cout << "The loop num. is :   " << i << endl;
        }
	}
    print_best(*population);
    inspire_interface result;
    result = MFCSolver(f_cdn,pop_alpha[0].server_id,clean_graph);
    return true;
}

/* ********************************************************************************** */
