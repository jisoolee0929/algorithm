#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01


//전역 변수를 선언해서 이 곳에 모든 정점과 간선 모으기
int vertics[16] = {0,};
int edges[16][2];
int verlen = 1;
int edglen = 0;


static void add_edge(int visited[], int depth){
	int start = visited[depth-1];
	int end = visited[depth];
	int isin = 0;
	for(int i=0; i<=edglen; i ++){
		if(start == edges[i][0] && end == edges[i][1]){
			isin = 1;
		}
	}

	if(isin ==0){
		edges[edglen][0] = start;
		edges[edglen][1] = end;
		edglen +=1;
	}

	// 배열 정렬
	for(int i = 0; i<edglen-1; i++){
		for(int j = i+1; j< edglen; j++){
			if(edges[i][0]> edges[j][0]){
				int temp = edges[i][0];
				edges[i][0] = edges[j][0];
				edges[j][0] = temp;
				int temp2 = edges[i][1];
				edges[i][1] = edges[j][1];
				edges[j][1] = temp2;
				
			}
			else if(edges[i][0] == edges[j][0] && edges[i][1] > edges[j][1] ){
				int temp = edges[i][0];
				edges[i][0] = edges[j][0];
				edges[j][0] = temp;
				int temp2 = edges[i][1];
				edges[i][1] = edges[j][1];
				edges[j][1] = temp2;
			}
		}
	}
	


}


static void add_vertic(int state){
	
	//없을 경우 추가
	int isin = 0;
	for(int j=0; j< verlen;j++){
		if(state == vertics[j]){
			isin = 1;
		}
	}
	if(isin ==0){
		vertics[verlen] = state;
		verlen +=1;

	}
	// 배열 정렬
	for(int i = 0; i<verlen-1; i++){
		for(int j = i+1; j< verlen; j++){
			if(vertics[i]> vertics[j]){
				int temp = vertics[i];
				vertics[i] = vertics[j];
				vertics[j] = temp;
			}
		}
	}

	

}


// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename( FILE *fp, int state){
	fprintf(fp,"\"<");
	for (int j = 3; j >= 0; --j) { 
        int result = state >> j & 1;
        fprintf(fp,"%d", result);
    }
	fprintf(fp,">\"\n");
}


static void print_state( int state){
	
	printf("<");
	for (int i = 3; i >= 0; --i) { 
        int result = state >> i & 1;
        printf("%d", result);
    }
	printf(">");
}



// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc( int state, int *p, int *w, int *g, int *c){
	
	int binary_state[4];

	for (int i = 3; i >= 0; --i) { 
        int result = state >> i & 1;
        binary_state[i] = result;
	}

	*p = binary_state[3];
	*w = binary_state[2];
	*g = binary_state[1];
	*c = binary_state[0];	

}	


// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end( int state){

	int binary_state[4];

	for (int i = 3; i >= 0; --i) { 
        int result = state >> i & 1;
        binary_state[i] = result;
	}
	// 농부의 상태
	int p = binary_state[3];

	// 늑대와 양의 상태
	int wg = binary_state[2] + binary_state[1];

	// 양과 양배추의 상태
	int gc = binary_state[1] + binary_state[0];


	if( (p == 0 && wg == 2) ||( p == 1 && wg == 0)){
		return 1;
	}
	else if( ( p== 0 && gc == 2) || (p == 1 && gc == 0) ){
		return 1;
	}
	else{
		return 0;
	}

}


// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP( int state){

	// 이진수 배열
	int binary_state[4];

	// state를 이진수 배열에 담기
	for (int i = 3; i >= 0; --i) { 
        
		int result = state >> i & 1;
		binary_state[i] = result;
	}

	// 농부가 1이면 0, 0이면 1로 이동
	if(binary_state[3] == 0){
		binary_state[3] = 1;
	}
	else{
		binary_state[3] = 0;
	}

	// 이진수 배열을 정수로 변환
	int result = 0;
	for (int i = 3; i >= 0; --i) { 
        result += binary_state[i]*pow(2,i);
	}

	return result;

}

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW( int state){
	
	// 이진수 배열
	int binary_state[4];

	// state를 이진수 배열에 담기
	for (int i = 3; i >= 0; --i) { 
        
		int result = state >> i & 1;
		binary_state[i] = result;
	}
	int pw = binary_state[3] + binary_state[2];

	// 농부 늑대 모두 1
	if( pw == 2 ){
		binary_state[3] = 0;
		binary_state[2] = 0;
	}
	// 농부 늑대 모두 0
	else if( pw == 0){
		binary_state[3] = 1;
		binary_state[2] = 1;
	}
	// 불가능
	else{
		return -1;
	}

	// 이진수 배열을 정수로 변환
	int result = 0;
	for (int i = 3; i >= 0; --i) { 
        result += binary_state[i]*pow(2,i);
	}

	return result;
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG( int state){
	// 이진수 배열
	int binary_state[4];


	// state를 이진수 배열에 담기
	for (int i = 3; i >= 0; --i) { 
        
		int result = state >> i & 1;
		binary_state[i] = result;
	}

	int pg = binary_state[3] + binary_state[1];

	// 농부 염소 모두 1
	if( pg == 2 ){
		binary_state[3] = 0;
		binary_state[1] = 0;
	}
	// 농부 염소 모두 0
	else if( pg == 0){
		binary_state[3] = 1;
		binary_state[1] = 1;
	}
	// 불가능
	else{
		return -1;
	}

	// 이진수 배열을 정수로 변환
	int result = 0;
	for (int i = 3; i >= 0; --i) { 
        result += binary_state[i]*pow(2,i);
	}

	return result;
}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC( int state){
	// 이진수 배열
	int binary_state[4];


	// state를 이진수 배열에 담기
	for (int i = 3; i >= 0; --i) { 
        
		int result = state >> i & 1;
		binary_state[i] = result;
	}

	int pc = binary_state[3] + binary_state[0];

	// 농부 양배추 모두 1
	if( pc == 2 ){
		binary_state[3] = 0;
		binary_state[0] = 0;
	}
	// 농부 양배추 모두 0
	else if( pc == 0){
		binary_state[3] = 1;
		binary_state[0] = 1;
	}
	// 불가능
	else{
		return -1;
	}

	// 이진수 배열을 정수로 변환
	int result = 0;
	for (int i = 3; i >= 0; --i) { 
        result += binary_state[i]*pow(2,i);
	}

	return result;
}

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited( int visited[], int depth, int state){

	for(int i =0; i<=depth; i++){
		if(visited[i] == state){
			return 1;
		}
	}
	return 0;
}
// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
static int is_possible_transition( int state1,	int state2){
	
	//dead_end 확인하기
	int result = is_dead_end(state2);
	if(result == 1){
		return 0;
	}
	

	//반대편 이동 가능 상태인지 확인
	// 상태1과 상태 2에서 차이가 발생하는 것 찾기 
	int binary_state1[4];
	int diff_state[4];

	for (int i = 3; i >= 0; --i) { 
        
		int result1 = state1 >> i & 1;
		int result2 = state2 >> i & 1;
		binary_state1[i] = result1;

		int is_change = abs(result1 - result2);
		diff_state[i] = is_change;
	}

	int all = diff_state[2]+ diff_state[1] + diff_state[0];

	// 농부와 늑대가 이동하는 경우
	if(diff_state[3]==1 && diff_state[2]==1)
	{
		result = changePW(state1);
		if(result == -1){
			return 0;
		}
		return 1;
	}
	else if (diff_state[3]==1 && diff_state[1]==1)
	{
		result = changePG(state1);
		if(result == -1){
			return 0;
		}
		return 1;
	}
	else if (diff_state[3]==1 && diff_state[0]==1)
	{
		result = changePC(state1);
		
		if(result == -1){
			return 0;
		}
		return 1;
	}
	else if (diff_state[3]==1 && all == 0)
	{
		return 1;
	}
	else{
		return 0;
	}
}

// 방문한 경로(상태들)을 차례로 화면에 출력
static void print_path( int visited[], int depth){
	printf("Goal-state found!\n");
		for(int i=0; i<=depth;i++){
			print_state(visited[i]);
			printf("\n");
		}
	printf("\n");
}



// recursive function
static void dfs_main( int initial_state, int goal_state, int depth, int visited[]){


	// initial state가 goal state와 visited 결과를 추가하고 종료
	if(initial_state == goal_state){
		for(int i =1; i<=depth;i++){
			add_edge(visited,i);
		}
	}
	printf("current state is ");
	print_state(initial_state);
	printf(" (depth %d)\n",depth);

	int state1 = changeP(initial_state);
	int state2 = changePW(initial_state);
	int state3 = changePG(initial_state);
	int state4 = changePC(initial_state);

	int states[4] = {state1,state2,state3,state4};
	int done = 0;

	for(int i=3; i > -1; i--){

		if(states[i] != -1){
			int next_state = states[i];
			// 방문 불가능한 경우 안내 메시지 발송
			if(is_possible_transition(initial_state,next_state) == 0){
				printf("\tnext-state ");
				print_state(next_state);
				printf(" is dead-end\n");

			}
			else if(is_visited(visited,depth+1,next_state)== 1){
				printf("\tnext-state ");
				print_state(next_state);
				printf(" has been visited\n");
			}

			// 방문한 적 없고, 이동 가능한 경우 이동
			if((is_visited(visited,depth+1,next_state)== 0) &&(is_possible_transition(initial_state,next_state) == 1)){
				
				int next_depth = depth + 1;
				int next_visited[16] = {0,};
				for(int i= 0; i <= depth;i++){
					next_visited[i] = visited[i];
				}	
				next_visited[next_depth] = next_state;

				if(done!=0){
					printf("back to ");
					print_state(initial_state);
					printf(" (depth %d)\n",depth);
				}
				// vetic과 edge에 추가
				add_vertic(next_state);
				// add_edge(next_visited,next_depth);

				dfs_main(next_state,goal_state,next_depth, next_visited);
				
				done = 1;
			}
		}
	}

}

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16]){

	for(int i= 0; i< edglen; i++){
		int start = edges[i][0];
		int end  = edges[i][1];
		graph[start][end] = 1;
		graph[end][start] = 1;
	}

}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num){
	for(int i = 0; i<16; i++){
		for(int j=0; j <16; j++){
			printf(" %d ", graph[i][j]);
		}
		printf("\n");

	}
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph( char *filename, int graph[][16], int num){
	FILE *fp = fopen(filename, "w"); 
	fprintf(fp,"*Vertices 16\n");
	for(int i =0; i<16; i++){
		fprintf(fp,"%d ",i+1);
		print_statename(fp,i);
	}
	fprintf(fp,"*Edges\n");
	for(int i = 0; i<edglen;i++){
		fprintf(fp," %d %d\n",edges[i][0]+1,edges[i][1]+1);
	}
    fclose(fp);  
}

////////////////////////////////////////////////////////////////////////////////
// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int initial_state, int goal_state)
{
	int depth = 0;
	int visited[16] = {0, }; // 방문한 정점을 저장
	
	dfs_main( initial_state, goal_state, depth, visited); 

	//완료된 후, 결과값 출력
	for(int i =0; i<verlen; i++){
		printf("vertic: %d\n", vertics[i]);
	}
	for(int i =0; i<edglen; i++){
		printf("edge: %d -> %d\n", edges[i][0], edges[i][1]);
	}
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int graph[16][16] = {0,};
	
	
	// 인접 행렬 출력 (only for debugging)
	// print_graph( graph, 16);
	

	// 깊이 우선 탐색
	depth_first_search( 0, 15); // initial state, goal state

	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// .net 파일 만들기
	save_graph( "pwgc.net", graph, 16);


	
	return 0;
}

