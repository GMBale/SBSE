#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include "Node.h"
#include "Person.h"
#define DEFAULT_p 8
#define DEFAULT_f -1
using namespace std;

void printPopulation();
void printArray(int*);
void arrayCopy(int*, int*);

FILE *in;
FILE *out;
FILE *oout;
int popLimit = DEFAULT_p;
int fitLimit = DEFAULT_f;
char line[100];

int solution;
//int solutionCross;
int* solutionNodes;

int n;
Node* nodes;
int** e;
int* rPermu;

int cur = 0;
int nextPop = 1;
vector<Person> population[2];

static bool comp(const Person &p, const Person &q) {
    if(p.cycle == q.cycle){
        for(int i = 2; i <= n; i++){
            if(p.nodes[i] == q.nodes[i]) continue;
            return p.nodes[i] < q.nodes[i];
        }
        return false;
    }
    return p.cycle < q.cycle;
}

int EUC_2D(int i, int j)
{
	int dx = nodes[i].x - nodes[j].x;
	int dy = nodes[i].y - nodes[j].y;
	return (int) (sqrt(dx*dx+dy*dy) + 0.5);
}

int skipUntil(const char *str)
{
    while(true){
        fscanf(in, "%s", line);
        if(strlen(str) == strlen(line) && strncmp(line, str, strlen(line)) == 0) return 0;
        if(strlen(str)-1 == strlen(line) && strncmp(line, str, strlen(line)-1) == 0) return 1;
    }
}

void setOptionValue(char* opt, char* valStr)
{
    if(strncmp(opt,"-p",2) == 0){
        popLimit = stoi(valStr);
    }else if(strncmp(opt,"-f",2) == 0){
        fitLimit = stoi(valStr);
    }else{
        printf("Option Error!\n");
    }
}

void randomPermute() {
    int k,j,temp;
    for (k = 1; k <= n; k++){
        rPermu[k] = k;
    }
    for (k = n; k > 1; k--) {
        j = (rand() % k) + 1;
        temp = rPermu[j];
        rPermu[j] = rPermu[k];
        rPermu[k] = temp;
    }
}

int calcCycle(int* nodeIdxs)
{
    int sum = 0, i;
    if(fitLimit > 0 || fitLimit == -1){
        if(fitLimit > 0) fitLimit--;
        nodeIdxs[0] = nodeIdxs[n];
        for(i = 0; i < n; i++){
            sum += e[nodeIdxs[i]][nodeIdxs[i+1]];
        }
        return sum;
    }else{
        oout = fopen("opt.tour.csv", "w");
        for(i=1;i<=n;i++){
            fprintf(oout,"%d\n",solutionNodes[i]);
        }
        fclose(oout);
        fflush(out);
        exit(0);
    }
}

bool onSegment(Node p, Node q, Node r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
       return true;
 
    return false;
}
 
// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Node p, Node q, Node r)
{
    // See http://www.geeksforgeeks.org/orientation-3-ordered-points/
    // for details of below formula.
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);
 
    if (val == 0) return 0;  // colinear
 
    return (val > 0)? 1: 2; // clock or counterclock wise
}
 
// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
int doIntersect(Node p1, Node q1, Node p2, Node q2)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
 
    // General case
    if (o1 != o2 && o3 != o4)
        return 1;
 
    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return 2;
 
    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return 3;
 
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return 4;
 
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return 5;
 
    return 0; // Doesn't fall in any of the above cases
}

int checkOnePair(int* nodeIdxs, int i, int j)
{
    int s,c, tmp;
    int arr[n+1];
    int flag;
    flag = doIntersect(nodes[nodeIdxs[i]], nodes[nodeIdxs[i+1]], nodes[nodeIdxs[j]], nodes[nodeIdxs[j+1]]);
    switch(flag){
        case 1:
            return 1;
            //printf("CROSS %d %d %d %d\n", i, i+1, j, j+1);
        case 2:
            tmp = nodeIdxs[j];
            for(s = j; s > i+1; s--){
                nodeIdxs[s] = nodeIdxs[s-1];
            }
            nodeIdxs[s] = tmp;
            break;
        case 3:
            tmp = nodeIdxs[j+1];
            for(s = j+1; s > i+1; s--){
                nodeIdxs[s] = nodeIdxs[s-1];
            }
            nodeIdxs[s] = tmp;
            break;
        case 4:
            c=0;
            arr[++c] = 1;
            for(s = j+1; s <= n ; s++){
                arr[++c] = nodeIdxs[s];
            }
            for(s = 2; s <= j ; s++){
                arr[++c] = nodeIdxs[s];
            }
            for(s = 1; s <= n ; s++){
                nodeIdxs[s] = arr[s];
            }
            break;
        case 5:
            tmp = nodeIdxs[i+1];
            for(s = i+1; s < j; s++){
                nodeIdxs[s] = nodeIdxs[s+1];
            }
            nodeIdxs[j] = tmp;
            break;
        case 0:
            break;
    }
//    if(flag > 1){
//        printf("On the line!\n");
//    }
    return 0;
}

int calcCross(int* nodeIdxs)
{
    int i,j;
    int cnt = 0;
    int flag = 0;
    int s,c,tmp;
    nodeIdxs[0] = nodeIdxs[n];
    for(i = 0; i < n - 2; i++){
        for(j = i + 2; j < n; j++){
            if(i == 0 && j+1 == n) continue;
            cnt += checkOnePair(nodeIdxs, i, j);
            //else if(
            /*else{
                printf("NOT %d %d %d %d\n", i, i+1, j, j+1);
            }*/
        }
    }
    return cnt;
}

bool untwistOne(int *a)
{
    int i,j,k,c;
    bool flag = false;
    int nodeIdxs[n+1];
    vector<int> idx[2];
    for(i = 1; i <= n; i++){
        nodeIdxs[i] = a[i];
    }
    nodeIdxs[0] = nodeIdxs[n];
    for(i = 0; i < n - 2; i++){
        for(j = i + 2; j < n; j++){
            if(i == 0 && j+1 == n) continue;
            if(checkOnePair(nodeIdxs, i, j) == 1){
                idx[0].push_back(i);
                idx[1].push_back(j);
            }
        }
    }
    if(idx[0].size() > 0){
        int r = rand() % (int)idx[0].size();
        i = idx[0][r];
        j = idx[1][r];
        c = 0;
        for(k = 1; k <= i; k++){
            rPermu[++c] = nodeIdxs[k];
        }
        for(k = j; k > i; k--){
            rPermu[++c] = nodeIdxs[k];
        }
        for(k = j+1; k <= n; k++){
            rPermu[++c] = nodeIdxs[k];
        }
        rPermu[0] = rPermu[n];
        for(k = 0; k <= n; k++){
            nodeIdxs[k] = rPermu[k];
        }
        flag = true;
    }

    for(i = 1; i <= n; i++){
        rPermu[i] = nodeIdxs[i];
    }
    return flag;
}

void untwist(int *a)
{
    int i,j,k,c;
    bool flag = true;
    int nodeIdxs[n+1];
    for(i = 1; i <= n; i++){
        nodeIdxs[i] = a[i];
    }
    nodeIdxs[0] = nodeIdxs[n];
    //int cycle = 0;
    //int lastCycle = calcCycle(nodeIdxs);
    //int llastCycle = -1;
    int it = 0;
    while(flag){
        //printf("untwist %d\n",++it);
        flag = false;
        for(i = 0; i < n - 2; i++){
            for(j = i + 2; j < n; j++){
                if(i == 0 && j+1 == n) continue;
                if(checkOnePair(nodeIdxs, i, j) == 1){
                    flag = true;
                    c = 0;
                    for(k = 1; k < i; k++){
                        rPermu[++c] = nodeIdxs[k];
                    }
                    for(k = j; k >= i; k--){
                        rPermu[++c] = nodeIdxs[k];
                    }
                    for(k = j+1; k <= n; k++){
                        rPermu[++c] = nodeIdxs[k];
                    }
                    rPermu[0] = rPermu[n];
                    for(k = 0; k <= n; k++){
                        nodeIdxs[i] = rPermu[k];
                    }
                }
                /*else{
                    printf("NOT %d %d %d %d\n", i, i+1, j, j+1);
                }*/
            }
        }
    }
    for(i = 1; i <= n; i++){
        rPermu[i] = nodeIdxs[i];
    }
}

double Seq(Person &a, Person &b)
{
    int i,j,c = 0;
    int d[n+1][n+1];
    for(i = 1; i <= n; i++){
        if(a.nodes[i] != b.nodes[i]) break;
        c++;
    }
    for(j = n; j > i; j--){
        if(a.nodes[i] != b.nodes[i]) break;
        c++;
    }
    return (double)c / n;
}

double LCS(Person &a, Person &b)
{
    int i,j;
    int d[2][n+1];
    int t = 0, nt= 1;;
    for(i = 0; i <= n; i++){
        d[t][i] = 0;
    }
    for(i = 1; i <= n; i++){
        for(j = 1; j <= n; j++){
            d[nt][j] = d[t][j];
            if(a.nodes[i] == b.nodes[j]){
                d[nt][j] = d[t][j-1]+1;
            }else{
                if(d[nt][j] < d[nt][j-1]){
                    d[nt][j] = d[nt][j-1];
                }
            }
        }
        t = 1 - t;
        nt = 1 - nt;
    }
    return (double)d[t][n] / n;
}

void crossover(Person &a, Person &b)
{
    int i,c = 0;
    int length;
    int pos;
    bool visit[n+1];
    for(i = 0; i <= n; i++){
        visit[i] = false;
    }
    pos = rand() % n + 1;
    //pos = 0;
    /*
    if(a.cycle < b.cycle){
        if(a.cross >= b.cross){
            length = 19*n/20 + (rand() % (n/20));
        }else{
            length = 75*n/100 + (rand() % (n/10));
        }
    }else{
        length = (int)(n/2);
    }*/
    length = (int)(n/2);
    //printf("len = %d, pos = %d\n", length, pos);
    for(i = 0; i < length; i++){
        rPermu[++c] = a.nodes[pos];
        visit[a.nodes[pos]] = true;
        pos++;
        if(pos > n) { pos = 1;}
    }
    for(i = 1; i <= n; i++){
        if(!visit[b.nodes[i]]){
            rPermu[++c] = b.nodes[i];
        }
    }
    //printArray(a.nodes);
    //printArray(b.nodes);
    //printArray(rPermu);
}

void mutate(Person &p, int threshold3)
{
    int threshold = 35;
    int threshold2 = 15;
    int p1,p2,tmp;
    int i,c,j;
    int r = rand()%1000;
    int arr[n+1];
    if(r < threshold){
        /*randomPermute();
        untwist(rPermu);
        for(i=1;i<=n;i++){
            p.nodes[i] = rPermu[i];
        }
        p.nodes[0] = p.nodes[n];
        */
        //printf("Mutated %d!!!!!!!!!!\n", r);
        p1 = rand()%n + 1;
        p2 = rand()%n + 1;
        if(p1 > p2){
            tmp = p1;
            p1 = p2;
            p2 = p1;
        }
        for(i=p1;i<p1+(p2-p1+1)/2; i++){
            tmp = p.nodes[i];
            p.nodes[i] = p.nodes[p2-i+p1];
            p.nodes[p2-i+p1] = tmp;
        }
    }else if(r < threshold + threshold2){
        randomPermute();
        for(i=1;i<=n; i++){
            p.nodes[i] = rPermu[i];
        }
        p.nodes[0] = p.nodes[n];
    }else if(r < threshold + threshold2 + threshold3){
        i = rand()% (n-1) + 1;
        tmp = p.nodes[i];
        p.nodes[i] = p.nodes[i+1];
        p.nodes[i+1] = tmp;
    }
    if(r < threshold + threshold2 + threshold3){
        c = 0;
		for(i = 1; i <= n; i++){
            arr[i] = p.nodes[i];
			if(arr[i] == 1){ j = i;}
		}
		for(i = j; i <= n; i++){
			p.nodes[++c] = arr[i];
		}
		for(i = 1; i < j; i++){
			p.nodes[++c] = arr[i];
		}
        p.nodes[0] = p.nodes[n];
    }
}

void GAMain()
{
    out = fopen("output.csv", "w");
    double startTime = static_cast<double> (clock()) / 1000000;
    int iter;
    int i,j;
    int len;
    int toPop;
    int first = popLimit/5, second = 4*popLimit/5;
    double avCycle, avCycle1, avCycle2, avCycle3;
    double lastTime = 0;
    double curTime = 0;
    for(iter = 0;; iter++){
        curTime = static_cast<double>(clock()) / 1000000 - startTime;
        avCycle = 0;
        printf("%d %.3lf %d ",iter, curTime, solution);
        fprintf(out, "%d %.3lf %d ",iter, curTime, solution);
        /*
        untwist(population[cur][0].nodes);
        int untwistedCycle = calcCycle(rPermu);
        if(solution > untwistedCycle){
            solution = untwistedCycle;
            solutionCross = calcCross(rPermu);
            arrayCopy(solutionNodes, rPermu); 
        }*/
        if(solution > population[cur][0].cycle){
            solution = population[cur][0].cycle;
            //solutionCross = population[cur][0].cross;
            //solutionCross = calcCross(rPermu);
            arrayCopy(solutionNodes, population[cur][0].nodes); 
            oout = fopen("opt.tour.csv", "w");
            for(i=1;i<=n;i++){
                fprintf(oout,"%d\n",solutionNodes[i]);
            }
            fclose(oout);
        }
        //printPopulation();
        for(i = 0; i < popLimit; i++){
            //avCross+=population[cur][i].cross;
            avCycle+=population[cur][i].cycle;
            bool isUnt = untwistOne(population[cur][i].nodes);
            if(isUnt){
                population[nextPop].push_back(Person(rPermu, n));
            }else{
                crossover(population[cur][i], population[cur][rand()%popLimit]);
                population[nextPop].push_back(Person(rPermu, n));
            }
        }
        for(i = 0; i < popLimit; i++){
            //avCross+=population[cur][i].cross;
            avCycle+=population[cur][i].cycle;
            crossover(population[cur][i], population[cur][popLimit-1-i]);
            population[nextPop].push_back(Person(rPermu, n));
        }
        for(i = 0; i < popLimit - 1; i++){
            //avCross+=population[cur][i].cross;
            avCycle+=population[cur][i].cycle;
            crossover(population[cur][i], population[cur][i+1]);
            population[nextPop].push_back(Person(rPermu, n));
        }
        avCycle+=population[cur][i].cycle;
        crossover(population[cur][popLimit - 1], population[cur][0]);
        population[nextPop].push_back(Person(rPermu, n));

        fprintf(out, "%.3lf\n", avCycle/(popLimit*3));
        fflush(out);
        sort(population[nextPop].begin(), population[nextPop].end(), comp);
        len = population[nextPop].size();
        //fprintf(out, "Before LCS Loop\n");
        //fflush(out);
        for(i = 0; i < population[nextPop].size() - 1; i++){
            //fprintf(out, "Before LCS\n");
            //fflush(out);
            double lcs = LCS(population[nextPop][i], population[nextPop][i+1]);
            //fprintf(out, "After LCS\n");
            //fflush(out);
            if(lcs > 0.6){
                //printf("seq = %.3lf\n", lcs);
                //fprintf(out, "Erase Before\n");
                //fflush(out);
                population[nextPop].erase(population[nextPop].begin()+i+1);
                //fprintf(out, "Erase After\n");
                //fflush(out);
                i--;
            }
        }
        //fprintf(out, "After LCS Loop\n");
        //fflush(out);
        len = population[nextPop].size();
        //printf("remain pop = %d\n",(int)len);
        for(i = popLimit; i < len; i++){
            population[nextPop].pop_back();
        }
        //fprintf(out, "Pop over\n");
        //fflush(out);
        len = population[nextPop].size();
        for(i = len; i < popLimit; i++){
            randomPermute();
            population[nextPop].push_back(Person(rPermu, n));
        }
        //fprintf(out, "Fill random\n");
        //fflush(out);
        len = population[nextPop].size();
        for(i = 0; i < len; i++){
            mutate(population[nextPop][i], 5);
        }
        //fprintf(out, "Mutation\n");
        //fflush(out);
        population[cur].erase(population[cur].begin(), population[cur].end());
        cur = nextPop;
        nextPop = 1 - nextPop;
    }
}

void arrayCopy(int *a, int *b)
{
    int i;
    for(i = 1; i <= n; i++){
        a[i] = b[i];
    }
}

void printArray(int* arr)
{
    int i;
    printf("arr=[");
    for(i = 1; i <= n; i++){
        printf("%d",arr[i]);
        if(i<n) printf(", ");
    }
    printf("]\n");
}

void printPopulation()
{
    int i,j;
    int len = population[cur].size();
    //printf("[");
    for(i = 0; i < len; i++){
        //printf("%d", population[cur][i].cycle);
        //if(i+1<len) printf(", ");
        printf("%d:[", population[cur][i].cycle);
        for(j = 1; j <= n; j++){
            printf("%d",population[cur][i].nodes[j]);
            if(j<n) printf(", ");
        }
        printf("]\n");
    }
    //printf("]\n");
}

int main(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));
    int fileIdx = 1;
    int i,j;
    switch(argc){
        case 0:
        case 1:
            printf("Enter input .tsp file!\n");
            break;
        case 3:
            printf("Option Error!\n");
            break;
        case 2:
        case 4:
        case 6:
            if(argc >= 4){
                setOptionValue(argv[1], argv[2]);
            }
            if(argc == 6){
                setOptionValue(argv[3], argv[4]);
            }
            fileIdx = argc - 1;
            break;
        case 5:
            printf("Option Error!\n");
            break;
    }
    printf("%d %d %s\n", popLimit, fitLimit, argv[fileIdx]);
    in = fopen(argv[fileIdx], "r");
    int flag = skipUntil("DIMENSION:");
    if(flag == 1){
        skipUntil(":");
    }
    fscanf(in, "%d", &n);
    printf("N = %d\n", n);
    skipUntil("NODE_COORD_SECTION");
    nodes = new Node[n+1];
    for(i = 0; i < n; i++){
        int id;
        double x,y;
        fscanf(in, "%d %lf %lf", &id, &x, &y);
        nodes[id].id = id;
        nodes[id].x = x;
        nodes[id].y = y;
    }
    e = new int*[n+1];
    rPermu = new int[n+1];
    solutionNodes = new int[n+1];
    for(i = 1; i <= n; i++){
        e[i] = new int[n+1];
        for(j = 1; j <= n; j++){
			e[i][j] = EUC_2D(i, j);
        }
    }
    skipUntil("EOF");
    printf("Start!\n");
    for(i = 0; i < popLimit; i++){
        randomPermute();
        population[cur].push_back(Person(rPermu, n));

    }
    sort(population[cur].begin(), population[cur].end(), comp);
    solution = population[cur][0].cycle;
    arrayCopy(solutionNodes, population[cur][0].nodes);
    //printPopulation();
    GAMain();
    return 0;
}
