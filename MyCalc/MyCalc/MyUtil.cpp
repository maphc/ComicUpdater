#include "StdAfx.h"
#include "MyUtil.h"
#include <cmath>

static int getIntByHexChar(char c){
	if(c>='0'&&c<='9'){
		return c-'0';
	}
	if(c>='A'&&c<='F'){
		return c-'A'+10;
	}
	if (c>='a'&&c<='f')
	{
		return c-'a'+10;
	}
	return 0;
}



MyUtil::MyUtil(void)
{
}

MyUtil::~MyUtil(void)
{
}

long MyUtil::hToD( CString cs)
{
	long s=0;
	int i,n;
	/*for ( n=cs.GetLength(), i=n-1;i>-1;i--)
	{
		s+=(long)getIntByHexChar(cs[i])*pow((long double)16,n-i-1);
		
	}*/
	for(n=cs.GetLength(),i=0;i<n;i++){
		s+=(long)getIntByHexChar(cs[i])*pow((long double)16,n-i-1);
	}
	
	return s;
	
}

void MyUtil::dToH( int dec,CString &cs )
{
	

}

