#include <stdio.h>
#include <iostream>
#include <string>
#include <cassert>
#include <ctime>
#include <algorithm>
using namespace std;
#define PR(x) cout<<#x"="<<x<<endl;

void init(char flag[20][20]);//初始化棋盘    
void draw(char flag[20][20]);//划出指定棋盘    
int test(char flag[20][20],char);//判断是否棋局结束    
bool check(int,int);//检查玩家输入坐标是否合法    

int main(){   
	//设置棋盘大小 ，可以设置成变量 x，y    
	char flag[20][20];   
	//初始化数据    
	init(flag);   

	for(int x =0;x<400;  ){//棋盘上总共能乘放下 x*y 个棋子    
		int i,j;   
		if(x%4==0){   
			cout<<"请A方走：";   
			cin>>i;   
			cin>>j;   
			while(flag[i][j]=='X'||flag[i][j]=='O')//判断输入位置是否有棋子    
			{   
				cout<<endl;   
				cout<<"此地已经有棋子！请重新走：";    
				cin>>i;   
				cin>>j;   
			}       
			if(check(i,j)==false)   
			{   
				cout<<endl;   
				cout<<"输入坐标不合法！请重新输入：";    
				cin>>i;   
				cin>>j;                     
			}                                       
			flag[i][j]='X';//指定位置放上棋子    
			draw(flag);   
			if(test(flag,'X'))//判断是否获胜（A）    
			{   
				char replay;   
				cout<<"A方胜利！是否再来一局？Y/N:";   
				cin>>replay;   
				if (replay=='Y'||replay=='y')   
				{   
					main();   
				} else  
				{   
					break;   
				}                                                

			}   
			x+=2;   
		}else{   
			cout<<"请B方走：";   
			cin>>i;   
			cin>>j;   
			while(flag[i][j]=='X'||flag[i][j]=='O')   
			{   
				cout<<endl;   
				cout<<"此地已经有棋子！请重新走：";    
				cin>>i;   
				cin>>j;   
			}    
			if(check(i,j)==false)   
			{   
				cout<<endl;   
				cout<<"输入坐标不合法！请重新输入：";    
				cin>>i;   
				cin>>j;                     
			}   
			flag[i][j]='O';   
			draw(flag);   
			if(test(flag,'O'))//判断是否获胜（B）    
			{   
				char replay;   
				cout<<"B方胜利！是否再来一局？Y/N:";   
				cin>>replay;   
				if (replay=='Y'||replay=='y')   
				{   
					main();   
				} else  
				{   
					break;   
				}                                                

			}   
			x+=2;   
		}   
	}   
	return 0;   
}   

void init(char flag[][20]){   
	for(int i=0;i<20;i++){    
		for(int j=0;j<20;j++){   
			flag[i][j]='|';   
			cout<<"-";   
			cout<<flag[i][j];   
		}   
		cout<<endl;   
	}          
}   

int test(char flag[][20],char f){   
	int mark =0;   
	for(int i=0;i<15;i++){   
		for(int j=0;j<15;j++){   
			if(//判断横着，数着，斜着三种成立情况    
				(f==flag[i][j]&&f==flag[i+1][j]&&f==flag[i+2][j]&&f==flag[i+3][j]&&f==flag[i+4][j])   
				||(f==flag[i][j]&&f==flag[i][j+1]&&f==flag[i][j+2]&&f==flag[i][j+3]&&f==flag[i][j+4])   
				||(f==flag[i][j]&&f==flag[i+1][j+1]&&f==flag[i+2][j+2]&&f==flag[i+3][j+3]&&f==flag[i+4][j+4]))   
			{   
				cout<<endl;         
				mark =1;                                                                                                                                 
			}   
		}   
	}   
	return mark;   
}   

void draw(char flag[][20] ){   
	for(int i=0;i<20;i++){   
		for(int j=0;j<20;j++){   
			cout<<"-";   
			cout<<flag[i][j];   
		}   
		cout<<endl;   
	}   
}   
bool check(int x,int y){   
	if(x>20 || y>20 || x<0 || y<0)    
		return false;   
	else  
		return true;   
}   
