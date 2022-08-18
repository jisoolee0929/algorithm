#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INSERT_OP      0x01
#define DELETE_OP      0x02
#define SUBSTITUTE_OP  0x04
#define MATCH_OP       0x08
#define TRANSPOSE_OP   0x10

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1



int max;
int loop_num =0;
static int __GetMin3( int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if(c < min)
		min = c;
	return min;
}

////////////////////////////////////////////////////////////////////////////////
// 정렬된 문자쌍들을 출력
void print_alignment( char align_str[][8], int level)
{
	int i;
	
	for (i = level; i >= 0; i--)
	{
		printf( "%s\n", align_str[i]);
	}
}


////////////////////////////////////////////////////////////////////////////////
// 네 정수 중에서 가장 작은 값을 리턴한다.
static int __GetMin4( int a, int b, int c, int d)
{
	int min = __GetMin3( a, b, c);
	return (min > d) ? d : min;
}

// 재귀적으로 연산자 행렬을 순회하며, 두 문자열이 최소편집거리를 갖는 모든 가능한 정렬(alignment) 결과를 출력한다.
// op_matrix : 이전 상태의 연산자 정보가 저장된 행렬 (1차원 배열임에 주의!)
// col_size : op_matrix의 열의 크기
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
// level : 재귀호출의 레벨 (0, 1, 2, ...)
// align_str : 정렬된 문자쌍들의 정보가 저장된 문자열 배열 예) "a - a", "a - b", "* - b", "ab - ba"
// 문자열의 길이가 최대 8인 거슨 ab - ba이면 이여서
static void backtrace_main( int *op_matrix, int col_size, char *str1, char *str2, int n, int m, int level, char align_str[][8]){
	//opmatrix의 값을 보고 현재 레벨에서 어디로 가야할 지 결정
	
	if(level ==0){
		loop_num = 0;
	}

	int num = op_matrix[(max-n)*col_size+m];
	int minus = 16;


	if(n ==0 && m ==0){
		
		printf("[%d] ==============================\n", loop_num);
		loop_num+=1;
		print_alignment(align_str,level-1);
		printf("\n");
	}

	while (num !=0)
	{
		if(num>=minus){
			
			if(minus==16){
				align_str[level][0] = str1[n-2];
				align_str[level][1] = str1[n-1];
				align_str[level][2] = ' ';
				align_str[level][3] = '-';
				align_str[level][4] = ' ';
				align_str[level][5] = str1[m-2];
				align_str[level][6] = str1[m-1];
				align_str[level][7] = '\0';
			
				backtrace_main(op_matrix,col_size,str1,str2,n-2,m-2,level+1,align_str);
				
				

			}
			else if(minus == 8){
				align_str[level][0] = str1[n - 1];
				align_str[level][1] = ' ';
				align_str[level][2] = '-';
				align_str[level][3] = ' ';
				align_str[level][4] = str2[m - 1];
				align_str[level][5] = '\0';
				
				backtrace_main(op_matrix,col_size,str1,str2,n-1,m-1,level+1,align_str);
				
			}
			else if(minus == 4){
				align_str[level][0] = str1[n - 1];
				align_str[level][1] = ' ';
				align_str[level][2] = '-';
				align_str[level][3] = ' ';
				align_str[level][4] = str2[m - 1];
				align_str[level][5] = '\0';
				backtrace_main(op_matrix, col_size, str1, str2, n - 1, m - 1, level + 1, align_str);
							
			}
			else if(minus == 2){
				align_str[level][0] = str1[n - 1];
				align_str[level][1] = ' ';
				align_str[level][2] = '-';
				align_str[level][3] = ' ';
				align_str[level][4] = '*';
				align_str[level][5] = '\0';
				backtrace_main(op_matrix, col_size, str1, str2, n - 1, m, level + 1, align_str);
							
			}
			else if (minus ==1 ){
				align_str[level][0] = '*';
				align_str[level][1] = ' ';
				align_str[level][2] = '-';
				align_str[level][3] = ' ';
				align_str[level][4] = str2[m - 1];
				align_str[level][5] = '\0';
				backtrace_main(op_matrix, col_size, str1, str2, n, m - 1, level + 1, align_str);
							
			}
			num -=minus;
		}
		minus /= 2;
			
	}
	
}


////////////////////////////////////////////////////////////////////////////////
// backtrace_main을 호출하는 wrapper 함수
// str1 : 문자열 1
// str2 : 문자열 2
// n : 문자열 1의 길이
// m : 문자열 2의 길이
void backtrace( int *op_matrix, int col_size, char *str1, char *str2, int n, int m)
{
	char align_str[n+m][8]; // n+m strings
	
	backtrace_main( op_matrix, col_size, str1, str2, n, m, 0, align_str);
	
}



// 강의 자료의 형식대로 op_matrix를 출력 (좌하단(1,1) -> 우상단(n, m))
// 각 연산자를 다음과 같은 기호로 표시한다. 삽입:I, 삭제:D, 교체:S, 일치:M, 전위:T
void print_matrix( int *op_matrix, int col_size, char *str1, char *str2, int n, int m){
	for(int i = 0; i<= n; i++){
		for(int j= 0; j<=m; j++){
			if(j==0 && i !=n){
				printf("%c\t",str1[n-i-1]);
			}
			else if(i == n && j !=0){
				printf("%c\t",str2[j-1]);
			}
			else if(i == n && j ==0){
				printf("\t");
			}
			else
			{
				int num = op_matrix[i*col_size+j];
				int minus = 16;
				while(num !=0){
					if(num>=minus){
						if(minus==16){
							printf("T");
						}
						else if(minus == 8){
							printf("M");
						}
						else if(minus == 4){
							printf("S");
						}
						else if(minus == 2){
							printf("D");
						}
						else if (minus ==1 ){
							printf("I");
						}
						num -=minus;
						
					}
					minus /= 2;
				}
				printf("\t");
			}
		}
		printf("\n");

	}
	printf("\n\n");
}

// 두 문자열 str1과 str2의 최소편집거리를 계산한다.
// return value : 최소편집거리
// 이 함수 내부에서 print_matrix 함수와 backtrace 함수를 호출함
int min_editdistance( char *str1, char *str2){
	int n = strlen(str1);
	int m = strlen(str2);

	max= n;
	int i,j;
	int d[n+1][m+1];
	int op_matirx[(n+1)*(m+1)];


	for(i = n ;i >=0 ; i--){
		d[i][0] = n-i;
		op_matirx[(m+1)*i] = DELETE_OP;
	}

	for(j=0; j<=m; j++){
		d[n][j] = j;
		op_matirx[n*(m+1)+j] = INSERT_OP;
	}
	op_matirx[n*(m+1)] = 0;


	for(i = n-1; i>=0; i--){
		for(j = 1; j<=m; j++){
			
			op_matirx[(m+1)*i+j] = 0;
			if(str1[n-i-1] == str2[j-1]){
				int min = __GetMin3(d[i+1][j-1], d[i][j-1]+ INSERT_COST ,d[i+1][j]+DELETE_COST) ;
					if(min == d[i][j-1]+ INSERT_COST){
						op_matirx[(m+1)*i+j] += INSERT_OP;
					}
					if(min == d[i+1][j]+DELETE_COST){
						d[i][j] = min + DELETE_COST;
					}
					if(min ==d[i+1][j-1]){
						d[i][j] = d[i+1][j-1];
						op_matirx[(m+1)*i+j] = MATCH_OP;
					}
				
			}
			else{
				int min = __GetMin3(d[i+1][j-1]+SUBSTITUTE_COST, d[i][j-1]+INSERT_COST,d[i+1][j]+DELETE_COST);
				if((str1[n-i-2] == str2[j-1]) && (str1[n-i-1] == str2[j-2]) &&(min == d[i+2][j-2]+TRANSPOSE_COST)){
					d[i][j] = d[i+2][j-2]+ TRANSPOSE_COST;
					op_matirx[(m+1)*i+j] = TRANSPOSE_OP;
				}
				if(min == d[i+1][j-1]+SUBSTITUTE_COST){
					op_matirx[(m+1)*i+j] += SUBSTITUTE_OP;
					d[i][j] = min;
				}
				if(min == d[i][j-1]+INSERT_COST){
					op_matirx[(m+1)*i+j] += INSERT_OP;
					d[i][j] = min;
				}
				if(min == d[i+1][j]+DELETE_COST){
					op_matirx[(m+1)*i+j] += DELETE_OP;
					d[i][j] = min;
				}
			}
		}
	}
	print_matrix(op_matirx,m+1,str1,str2,n,m);

	backtrace(op_matirx,m+1,str1,str2,n,m);

	return(d[0][m]);

}

////////////////////////////////////////////////////////////////////////////////
// 세 정수 중에서 가장 작은 값을 리턴한다.







////////////////////////////////////////////////////////////////////////////////
int main()
{
	char str1[30];
	char str2[30];
	
	int distance;
	
	fprintf( stderr, "INSERT_COST = %d\n", INSERT_COST);
	fprintf( stderr, "DELETE_COST = %d\n", DELETE_COST);
	fprintf( stderr, "SUBSTITUTE_COST = %d\n", SUBSTITUTE_COST);
	fprintf( stderr, "TRANSPOSE_COST = %d\n", TRANSPOSE_COST);
	
	while( fscanf( stdin, "%s\t%s", str1, str2) != EOF)
	{
		printf( "\n==============================\n");
		printf( "%s vs. %s\n", str1, str2);
		printf( "==============================\n");
		
		distance = min_editdistance( str1, str2);
		
		printf( "\nMinEdit(%s, %s) = %d\n", str1, str2, distance);
	}
	return 0;
}

