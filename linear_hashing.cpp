#include <iostream>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <cmath>

#define Bucket_size 40
#define record_size 5000
#define search_size 50

using namespace std;


const int list_size = (int)1e7;
const int value_bucket_size = (int)(0.70*list_size);
const int overflow_bucket_size = (int)(0.30*list_size);

int overflow_bucket_index = value_bucket_size;
int next_to_split=0;
int split_range;
int overflow_curr=0;
double N,B,b,bs,s;


typedef struct Node{
	bool overflow;
	int empty_space;
	vector<int> bucket;
	int I;
	struct Node* next;
}node;

void insert(int, int, int, bool, bool, int&);
void disp(void);
void split_and_rehash(int, int&);
node* newnode(int, int, int);

//int directory [directory_size];
node* bucket_list [list_size];


bool empty=true;
int split_fun=1;
int mod;

node* newnode(int data, int I, int overflow){
	node* ptr = new node;

	ptr->overflow = overflow;
	ptr->empty_space = Bucket_size-1;
	ptr->bucket.push_back(data);
	ptr->I = I;
	ptr->next = NULL;

	return ptr;
}

void remove_from_overflow_bucket(int data){
	//cout<<data<<"\n";
	for(int i = overflow_bucket_index; i<list_size; i++){
		if(bucket_list[i] != NULL){
			vector<int> v = bucket_list[i]->bucket;
			vector<int>::iterator it;
			it = find(v.begin(),v.end(), data); 
			if(it!= v.end()){
				v.erase(it);
				bucket_list[i]->empty_space += 1;
				
			}

			if(v.size()==0)
				bucket_list[i]=NULL;
		}
	}
}

void split_and_rehash(int data, int &split_cost){
	if(bucket_list[next_to_split]){
		vector<int> v = bucket_list[next_to_split]->bucket;
		node* tmp = bucket_list[next_to_split]->next;

		while(tmp != NULL){
			split_cost += 1;
			for(int p=0; p<tmp->bucket.size();p++)
				v.push_back(tmp->bucket[p]);
			tmp = tmp->next;
		}
		v.push_back(data);
		bucket_list[next_to_split]=NULL;
		
		//split_fun = split_fun <<1;
		int index;

		for(int i=0; i<v.size(); i++){
			index = v[i]%(split_fun*2);
			insert(v[i], 2*split_fun ,index, false, true,split_cost);
			remove_from_overflow_bucket(v[i]);
			//int over_ind = search(v[i]);
			//bucket_list[over_ind]=NULL;
		}

		next_to_split += 1;
		if(next_to_split == split_fun){
			next_to_split = 0;
			//split_range = split_range<<1;
			split_fun = split_fun<<1;
		}
	}
}

void insert(int data, int split_fun, int index, bool overflow, bool stop, int &split_cost){
	//int index = data&((1<<split_fun)-1);
	// if bucket is empty 
	//cout<<index<<" bucket_list[index] == NULL  " <<(bucket_list[index] == NULL)<<"\n";
	
	if(bucket_list[index]==NULL){
		split_cost += 1;
		bucket_list[index] = newnode(data,split_fun,overflow);
	}
	
	else if(bucket_list[index] != NULL){
		split_cost += 1;
		// case 1: bucket have space to insert data
		// case 2: bucket is full but no overflow bucket
		// case 3: bucket is full and have overflow bucket

		// case 1:
		if(bucket_list[index]->empty_space){
			bucket_list[index]->bucket.push_back(data);
			bucket_list[index]->empty_space -= 1;
		}
		// case 2:
		else if(bucket_list[index]->empty_space==0){
			if(!stop)
				split_and_rehash(data, split_cost);
			else{
				int add = overflow_bucket_index + (overflow_curr)%overflow_bucket_size;
				//cout<<data<<" "<<split_fun<<" "<<add<<"\n";
				overflow_curr += 1;
				split_cost += 1;
				insert(data, split_fun, add, true, true,split_cost);
				bucket_list[index]->next = bucket_list[add];
				//cout<<"test: "<<bucket_list[index]->next->bucket.size()<<"\n";
			}
			
		}
	}

}

double search(int data, bool &flag){
	//cout<<"searching...\n";
	double ans=1;
	int split_pointer, index; 
	split_pointer = split_fun; //(next_to_split+split_fun)%split_fun ;
	if(next_to_split==0)
		index=0;
	else
	 index = data%(split_pointer);
	
	int id;

	if(next_to_split<index){
		if((split_pointer>>1 )==0)
			index = 0;
		else
			index = data%(split_pointer>>1);
	}

	node* ptr = bucket_list[index];
	while(ptr != NULL){
		ans += 1;
		vector<int> v = ptr->bucket;
		vector<int>::iterator it;
		it = find(v.begin(), v.end(), data);
		if(it != v.end()){
			id = it - v.begin();
			//cout<<"data : "<<data<<" found at bucket: "<<index<<" index: "<<id<<"\n";
			return ans;
		}
		ptr = ptr->next;
	}
	//cout<<"key not found\n";
	flag=false;
	return ans;
}


void disp(){
	//cout<<"disp funct() called..\n";
	int buck_no=1;
	for(int i=0; i<list_size; i++){
		if(bucket_list[i]!=NULL){
			//cout<<"elements..\n";
			//vector<int> tmp;
			cout<<i<<" # ";
			node* ptr=bucket_list[i];
			while(ptr != NULL){
				for(int k=0;k<ptr->bucket.size();k++)
					cout<<ptr->bucket[k]<<" ";
				cout<<" | ";
				buck_no += 1;
				ptr = ptr->next;
			}
		cout<<"\n";
		}
	}
	cout<<"\n";
}

double bucket_in_hash_table(){
	double ans=0;
	for(int i=0; i<list_size;i++){
		if(bucket_list[i] != NULL){
			ans++;
		}
	}
	return ans;
}

double bucket_capacity(){
	return (double)Bucket_size;
}

int main(){
	int n,data,index,counter, no_search;
	int key[record_size];
	srand(0);
	ofstream out;

	for(int i=0;i<list_size;i++)
		bucket_list[i]=NULL;

	cin>>n;
	counter=0;
	no_search=0;
	int split_cost=0;
	while(n--){
		
		cin>>data;
		index = data%split_fun;
		//int split_cost=0;
		insert(data, split_fun, index, false, false, split_cost);
		N++;
		B = bucket_in_hash_table();
		b = bucket_capacity();


		counter = (counter+1)%record_size;
		key[counter]=data;
		bool flag;
		if(counter==0){
			for(int i=0; i<search_size;i++){
				int idx = rand()%search_size;
				flag=true;
				bs += search(key[idx], flag);
				if(flag)
					s++;
				if(b && B && bs && s){
					cout<<"--------------------------------------\n";
					cout<<"Space utilization: "<<(N/(B*b))<<"\n";
					out.open("linear_space_utlilzation_40.txt", ios_base::app);
					out<<(N/(B*b))<<"\n";
					out.close();
					
					cout<<"Average search cost: "<<(bs/s)<<"\n";
					out.open("linear_search_cost_40.txt", ios_base::app);
					out<<(bs/s)<<"\n";
					out.close();

					cout<<"Split Cost: "<<((double)split_cost/N)<<"\n";
					out.open("linear_split_cost_40.txt", ios_base::app);
					out<<((double)split_cost/N)<<"\n";
					out.close();
					cout<<"\n--------------------------------------\n\n";
				}
			}
		}
	}
	//disp();
}
