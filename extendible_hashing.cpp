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

const int directory_size = 1024;
const int list_size = (int)1e7;
const int value_bucket_size = (int)(0.70*list_size);
const int overflow_directory_size = (int)(0.20*list_size);
const int overflow_bucket_size = (int)(0.10*list_size);

int overflow_directory_index=value_bucket_size;
int overflow_bucket_index = overflow_bucket_size;

int global_depth=0;
int curr_directory_size=0;
int max_bit=0;
int max_val = -1;
int overflow_curr=0;
double N,B,b,bs,s;

typedef struct Node{
	bool overflow;
	int empty_space;
	vector<int> bucket;
	int local_depth;
	int index;
	struct Node* next;
}node;

int directory [directory_size];
node* bucket_list [list_size];//={NULL};
void insert(int,int,bool,bool,int&);

node* newnode(int data, int depth, int overflow, int index){
	node* ptr = new node;

	ptr->overflow = overflow;
	ptr->empty_space = Bucket_size-1;
	ptr->bucket.push_back(data);
	ptr->local_depth = depth;
	ptr->index = index;
	ptr->next = NULL;

	return ptr;
}

node* newdirectory(int directory_index, int bucket_index, int overflow){
	node* ptr = new node;
	ptr->overflow=overflow;
	ptr->empty_space = Bucket_size-2;
	ptr->bucket.push_back(directory_index);
	ptr->bucket.push_back(bucket_index);
	ptr->next = NULL;

	return ptr;
}

bool directory_in_range(int index){
	return (index<directory_size);
}

bool isEmpty(int index){
	return (directory[index]==-1);
}

int bits_val(int data){
	if(data>max_val)
		max_val = data;
	return  (int)(log2(max_val))+1;
}

void show(vector<int> p){
	for(int i=0; i<p.size(); i++)
		cout<<p[i]<<" ";
	cout<<"\n";
} 

//void remove_from_overflow_bucket(int data){

//}

void split_and_rehash(int data, int buck_add, int dict_add, int &split_cost){
	node* ptr = bucket_list[buck_add];
		vector<int> v = ptr->bucket;
		
		while(ptr->next != NULL){
			ptr = ptr->next;
			split_cost += 1;
			for(int i=0; i<ptr->bucket.size();i++){
				v.push_back(ptr->bucket[i]);
				//bucket_list[ptr->index] = NULL;
			}
			if(ptr->bucket.size()==0){
				bucket_list[ptr->index]=NULL;
			}

		}

		v.push_back(data);
		//show(v);
		
		
	// global_depth >  local_depth
	if(bucket_list[buck_add]->local_depth < global_depth){
		//cout<<"global_depth > local_depth \n";
		bucket_list[buck_add]->local_depth += 1;
		int depth = bucket_list[buck_add]->local_depth;
		int index;
		bucket_list[buck_add]=NULL;
		directory[dict_add]=-1;

		for(int i=0; i<v.size();i++){
			index = v[i]&((1<<depth)-1);
			insert(v[i], index, false, true, split_cost);
			//remove_from_overflow_bucket(v[i]);
		}	
	}
	//global_depth == local_depth
	else if(bucket_list[buck_add]->local_depth == global_depth){
		bucket_list[buck_add]=NULL;
		directory[dict_add]=-1;
		int index;
		//cout<<"global_depth == local_depth \n";
		global_depth += 1;
		for(int i=0; i<v.size();i++){
			index = v[i]&((1<<global_depth)-1);
			insert(v[i], index, false, true, split_cost);
		}	
	}
}

void insert(int data, int index, bool overflow, bool stop, int &split_cost){
	//int index = data&((1<<depth)-1);
	//cout<<index<<"\n";
	// check whether directory is overflow or not 
	split_cost += 1;
	if(!directory_in_range(index)){
		//cout<<"overflow directory\n";
		
		int dict_add = overflow_directory_size+(overflow_directory_index)%overflow_directory_size;
		overflow_bucket_index += 1;
		if(bucket_list[dict_add]==NULL){
			bucket_list[dict_add] = newdirectory(dict_add, index, true);
			// inserting bucket at idex and assigning directory 
			// bucket to it's address.
			split_cost += 1;
			if(bucket_list[index]==NULL)
				bucket_list[index] = newnode(data, global_depth, true, index);
			else{
				if(bucket_list[index]->bucket.size()>=2){
					bucket_list[index]->bucket.push_back(dict_add);
					bucket_list[index]->bucket.push_back(index);
				}

				else{
					split_cost += 1;
					int add = overflow_bucket_index+(overflow_curr)%overflow_bucket_size;
					bucket_list[add] = newnode(data, global_depth, true, index);
					bucket_list[index]->overflow = false;
					bucket_list[index]->next = bucket_list[add];
				}
			}
		}
	}
	
	else{ // case 1: directory empty, insert simply
		if(directory[index] == -1){
			directory[index] = index;
			bucket_list[index] = newnode(data, global_depth, false, index);
		}	

		else if(directory[index]>=0){
			int buck_add = index;//directory[index];

			if(bucket_list[buck_add]->empty_space){
				bucket_list[buck_add]->bucket.push_back(data);
				bucket_list[buck_add]->empty_space -= 1;
			}
			else if(bucket_list[buck_add]->empty_space==0){
				split_cost += 1;
				if(!stop)
					split_and_rehash(data,buck_add, index,split_cost);
					
					//adding overflow data into overflow bucket.
				else{

					int add = overflow_bucket_index+(overflow_curr)%overflow_bucket_size;
					
					overflow_curr += 1;
					//insert(data, add, true, true);
					if(bucket_list[add] == NULL){
						bucket_list[add] = newnode(data, global_depth, false,add);
						bucket_list[buck_add]->next = bucket_list[add];
					}
					else{ 
						cout<<"never called ..";
						bucket_list[add]->bucket.push_back(data);
					}

				}
			}
		}
	}

}

void disp(){
	//int n = sizeof(directory)/sizeof(directory[0]);
	cout<<"\n------------------------------------------\n";
	
	for(int i=0;i<directory_size;i++){
		
		if(directory[i]!=-1){
			cout<<directory[i]<<" # ";	
			int idx = directory[i];
			//cout<<"GD: "<<global_depth<<"\tLD: "<<bucket_list[idx]->local_depth<<"\n";
			
			for(int p=0; p<bucket_list[idx]->bucket.size();p++)
				cout<<bucket_list[idx]->bucket[p]<<"  ";
			if(bucket_list[idx]->next != NULL){
				//cout<<"overflow bucket here!!\n";
				cout<<"| ";
				node* tmp = bucket_list[idx]->next;
				while(tmp != NULL){
					for(int k=0; k<tmp->bucket.size();k++)
					cout<<tmp->bucket[k]<<" ";
					tmp = tmp->next;		
				}
			}
			cout<<"\n";	
		}
		//cout<<"\n";	
	}

	cout<<"\n------------------------------------------\n";
}

void get_bucket(int add, int data, double &ans, bool &flag){
	//cout<<add<<"\n";
	if(add>=0 && bucket_list[add] != NULL){
			ans += 1;
			vector<int>v;
			vector<int>::iterator it;
			node* ptr = bucket_list[add];
			v = ptr->bucket;
			it = find(v.begin(), v.end(), data);
			if(it != v.end()){
				int idx = it - v.begin();
				//cout<<"data: "<<data<<" found at bucket: "<<add<<" index: "
				//	<<idx<<"\n";
					flag = true;
					return ;
			}
			else{
				while(ptr->next != NULL){
					ans += 1;
					ptr = ptr->next;
					v.clear();
					v = ptr->bucket;
					it = find(v.begin(), v.end(), data);
					if(it != v.end()){
						int idx = it - v.begin();
						//cout<<"data: "<<data<<" found at bucket: "<<add<<" index: "
						//	<<idx<<"\n";
						flag = true;
						return ;
					}
				}
				
			}
		}
	//else
	//cout<<"Invalid address !!!"; 
}

double search(int data, bool &flag){
	double ans=1;
	int index = data&((1<<global_depth)-1);

	if(directory_in_range(index)){
		//cout<<"within directory contains\n";
		int add = directory[index];
		get_bucket(add,data, ans, flag);
		if(flag)
			return ans;
	}
	else{
		//cout<<"overflow directory contains\n";
		int buck_add=-1, dict_add=-1;
		node* ptr = bucket_list[index];
		while(ptr!= NULL){
			ans += 1;
			for(int i=0; i<ptr->bucket.size();i+=2){
				if(ptr->bucket[i]==index){
					dict_add = ptr->bucket[i];
					buck_add = ptr->bucket[i+1];
					break;
				}
			}
			ptr=ptr->next;
		}
		if(buck_add!=-1){
			get_bucket(buck_add,data, ans,flag);
			if(flag)
				return ans;
		}
	}
	//cout<<"Error!!!!  Data not found....\n";
	flag = false;
	return ans;
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
	int n, data, index,counter, no_search;;
	int key[record_size];
	srand(0);
	ofstream out;

	for(int i=0;i<directory_size;i++)
		directory[i]=-1;
	for(int i=0; i<list_size;i++)
		bucket_list[i]=NULL;
	cin>>n;
	counter=0;
	no_search=0;
	int split_cost=0;
	while(n--){
		cin>>data;
		index = data&((1<<global_depth)-1);
		insert(data, index,false, false, split_cost);
		N++;
		B = bucket_in_hash_table();
		b = bucket_capacity();

		counter = (counter+1)%record_size;
		key[counter]=data;
		bool flag;
		if(counter==0){
			for(int i=0; i<search_size;i++){
				int idx = rand()%search_size;
				flag = true;
				bs += search(key[idx],flag);
				if(flag)
					s++;
				if(b && B && bs && s){
					cout<<"--------------------------------------\n";
					cout<<"Space utilization: "<<(N/(B*b))<<"\n";
					out.open("extend_space_utlilzation_40.txt", ios_base::app);
					out<<(N/(B*b))<<"\n";
					out.close();
					
					cout<<"Average search cost: "<<(bs/s)<<"\n";
					out.open("extend_search_cost_40.txt", ios_base::app);
					out<<(bs/s)<<"\n";
					out.close();

					cout<<"Split Cost: "<<((double)split_cost/N)<<"\n";
					out.open("extend_split_cost_40.txt", ios_base::app);
					out<<((double)split_cost/N)<<"\n";
					out.close();
					cout<<"\n--------------------------------------\n\n";
				}
			}
		}
		//disp();
		//search(data);
	}
	//disp();
}