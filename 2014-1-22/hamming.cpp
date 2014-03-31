#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>

#define maxsize 10

using namespace std;

std::vector<int> bits[maxsize];

void formbits(int maxnumber)
{
	for(int i=1;i<maxnumber;i++)
	{
		for(int j=0;pow(2,j)<maxnumber;j++)
		{
			int k=(int)pow(2,j);
			int x=(i&k);
			if(x==k)
			{
				bits[j].push_back(i);
			}
		}
	}
}

void valuer(string& output,int k)
{
	int count;
	for(int i=0;i<k;i++)
	{
		count=0;
		for(int j=0;j<bits[i].size() && bits[i][j]<output.length();j++)
		{
			if(output[bits[i][j]]=='1')
				count++;
		}
		if(count%2!=0)
			output[(int)pow(2,i)]='1';
	}
}

int teststring(string output,int k)
{
	int count=0,error=0;
	for(int i=0;i<k;i++)
	{
		count=0;
		for(int j=0;j<bits[i].size() && bits[i][j]<output.length();j++)
		{
			if(output[bits[i][j]]=='1')
				count++;
		}
		//cout<<count<<" "<<i<<endl;
		if(count%2!=0)
			error+=(int)pow(2,i);
	}
	return error;
}

int main()
{
	int T;
	string input,output;
	formbits(pow(2,maxsize));
	scanf("%d",&T);
	while(T--)
	{
		cout<<"Passing string:\t";
		cin>>input;
		output=" ";
		int k;
		k=(int)(log(input.length())/log(2))+1;
		if(pow(2,k)<=(k+input.length()))
			k++;
		for(int i=1,x=0;i<=input.length()+k;i++)
		{
			bool flag=false;
			for(int j=0;j<k;j++)
				if(pow(2,j)==i)
				{
					output+="0";
					flag=true;
					break;
				}
			if(flag)
				continue;
			output+=input[x++];
		}
		valuer(output,k);
		cout<<"Hamming msg:\t"<<output<<endl;
		cout<<"Message Recieved:\t";
		string temp_input;
		input=" ";
		cin>>temp_input;
		k=(int)(log(temp_input.length())/log(2))+1;
		input+=temp_input;
		int error=teststring(input,k);
		if(error!=0)
			cout<<"Error at position "<<error<<endl;
	}
	return 0;
}