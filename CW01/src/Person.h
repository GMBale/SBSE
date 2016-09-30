int calcCycle(int*);
int calcCross(int*);

struct Person{
	int *nodes;
	int fitness;
	int cycle;
	int cross;

	Person () {
		cycle = -1;
		cross = -1;
		fitness = -1;
	}
	Person (int *arr, int n) {
		int i,j,c = 0;
		nodes = new int[n+1];
		for(i = 1; i <= n; i++){
			if(arr[i] == 1){ j = i; break;}
		}
		for(i = j; i <= n; i++){
			nodes[++c] = arr[i];
		}
		for(i = 1; i < j; i++){
			nodes[++c] = arr[i];
		}
        if(nodes[2] > nodes[n]){
            for(i = 2; i <= (n+1)/2; i++){
                c = nodes[i];
                nodes[i] = nodes[n-i+2];
                nodes[n-i+2] = c;
            }
        }
		cycle = calcCycle(nodes);
		cross = 0;
		//cross = calcCross(nodes);
		nodes[0] = nodes[n];
	}
    /*
	bool operator <(const Person &p) const
	{
		return cycle < p.cycle;
	}*/
};
