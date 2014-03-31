#include <iostream>
#include <stdio.h>

using namespace std;

#define degree 3
#define divisor_length 4

//Forming the dividend
string form_dividend(string dividend,int& index,int length,string remainder)
{
	string new_dividend="";
	for(int i=0;i<remainder.length();i++)
		new_dividend+=remainder[i];
	for(;new_dividend.length()<length;index++)
	{
		new_dividend+=dividend[index];
	}
	return new_dividend;
}

//Dividing and returning remainder
string divide(string dividend ,string divisor)
{
	string remainder="";
	for(int i=0;i<dividend.length();i++)
		remainder+=((dividend[i]-'0')^(divisor[i]-'0'))+'0';
	return remainder;
}


int main()
{
	//Declaration
	string dividend="100100";
	string divisor="1101";
	string last_remainder="",new_remainder;

	//Attaching Redundacny bits
	for(int i=0;i<degree;i++)
		dividend+="0";

	//Main loop
	int k;
	for(int j,i=0; i<dividend.length();)
	{
		new_remainder=divide(form_dividend(dividend,i,divisor_length,last_remainder),divisor);
		cout<<"remainder = "<<new_remainder;
		//Calculating position of first 1 in new_remainder
		j=0;
		while(j<new_remainder.length() && new_remainder[j]=='0')
		{
			j++;
		}

		if(j==0)
			j++;
		//Forming the last_remainder from new_remainder by removing 0 from the start of it.
		last_remainder="";
		while(j<new_remainder.length())
			last_remainder+=new_remainder[j++];
		cout<<"  last_remainder = "<<last_remainder<<endl;
		//scanf("%d",&k);
	}
	//Printing final remainder
	int j=1;
	last_remainder="";
	while(j<new_remainder.length())
		last_remainder+=new_remainder[j++];
	cout<<"final_remainder = "<<last_remainder<<endl;
	return 0;
}