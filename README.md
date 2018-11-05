# Dynamic_hashing
@author kartik vishwakarma
 		2017csm1001
 		Assignemnt: 1
 		Implementation to databases



Note : since after 5000 insertion 50 search operation is executing, 
		to inseating 5000 key may took time 5-8  minutes, 
		for sake of correctness
		change paramaters as follow:

			#define Bucket_size 40
			#define record_size 50
			#define search_size 10

----------------------------------------------------------
make:

 type make to execute all .cpp file
 after make it genarate "./dataset  ./extend ./linear"

 create Dataset:
 	./dataset

 run Linear hashing:
 	./linear < dataset.txt

 run Extendible hashing:
	./extend < dataset.txt 
-----------------------------------------------------------------

all parmater are define using #define directrive, you may change these paramaeter for experiments.

---------------------------------------------------------------
debugging:
	gdb ./linear

	run < dataset.txt
	backtrace 

---------------------------------------------------------------
