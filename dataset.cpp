#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unordered_set>
#include <iterator>
#define range 100000
#define mod 800000

using namespace std;

int main(){
	ofstream out;
	//out.open("data.txt");
	out.open("dataset.txt");
	int num;
	unordered_set<int> points;
	unordered_set<int>::iterator it;
	srand(0);
	
	while(points.size()<mod){
		num = rand()%mod;
		points.insert(num);
	}
	out<<mod<<"\n";
	for(it = points.begin(); it != points.end(); it++)	
		out<<*it<<" ";
	
	/*
	for(int i=0; i<=mod; i++){
		num = rand()%mod;
		out<<num<<" ";
	}*/

	cout<<"\n";
	out.close();
}

