#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <time.h>
#include <set>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

map<int, set<int> > getRecords(const string &inputFile);

bool friendSearch(map<int, set<int> >, int, int, int, int);
bool friendfriendSearch(map<int, set<int> >, set<int>, int, int, int, int);

int main(int argc, char** argv){

	if(argc<5)cerr<<"Please input a file"<<endl;
	string inputfile = argv[1];
	string testfile = argv[2];
	int depth = stoi(string(argv[3]));

	ofstream outfile;
	outfile.open(argv[4]);

	clock_t rec = clock();
	map<int, set<int> > payers = getRecords(inputfile);
	rec = clock() - rec;

	outfile<<"It took "<<rec/CLOCKS_PER_SEC<<" seconds to grab "<<payers.size()<<" records"<<endl;

	ifstream  testData(testfile);
  string testLine;

  while(std::getline(testData,testLine)){

		//skip the header line
		if(testLine.find("time") == 0 )continue;

		//grab a line as a transaction
    stringstream lineStream(testLine);
    string cell;		
		int counter=0;
		int id1,id2;
  	while(std::getline(lineStream,cell,',')){
			if(counter==1)id1=stoi(cell);
			else if(counter==2)id2=stoi(cell);
		
			counter++;	
		}
	
		//'payers' starts as an empty set and builds up as we run through transactions
		clock_t t = clock();
		bool checkTrust = friendSearch(payers, id1, id2, depth, 0);
    t = clock() - t; 
		if (checkTrust)outfile<<"trusted ... "<<(float)t/CLOCKS_PER_SEC<<endl;
		else outfile<<"unverified ... "<<(float)t/CLOCKS_PER_SEC<<endl;
	}

	outfile.close();
	return 0;
}

map<int, set<int> > getRecords(const string &inputFile){

	FILE *file;
	file = fopen(inputFile.c_str(),"r");

	const int headerSize=65536;
	char header[headerSize];
	fgets(header, headerSize, file);

	//Fill each line into a map<payer, set<payees> >
	map<int, set<int> > payers;
	char time[256];
	int id1, id2;
	float amount;
	while (!feof(file)){
		fscanf(file,"%20[^,], %i, %i, %f", time, &id1, &id2, &amount);
		fgets(header, headerSize, file);

		map<int, set<int> >::iterator it = payers.find(id1);
		set<int> friendSet;
	
		if(it != payers.end()){
			friendSet = it->second;
			friendSet.insert(id2);
			it->second = friendSet;
		}
		else {
			friendSet.insert(id2);
			pair<int, set<int> > setPair = make_pair(id1, friendSet);
			payers.insert(setPair);		
		}	
	}
	return payers;
}

//map<int, set<int> > getRecords(const string &inputFile){
//
//	std::ifstream  data(inputFile);
//  std::string line;
//
//	map<int, set<int> > payers;
//  while(std::getline(data,line)){
//		if(line.find("time") == 0 )continue;
//
//    std::stringstream lineStream(line);
//    std::string cell;		
//
//		int counter=0;
//		int id1, id2;
//  	while(std::getline(lineStream,cell,',')){
//			if(counter==1){
//				id1=stoi(cell);
//			}
//			else if(counter==2){
//				id2=stoi(cell);
//			}	
//			counter++;	
//		}
//
//		map<int, set<int> >::iterator it = payers.find(id1);
//		set<int> friendSet;
//		
//		if(it != payers.end()){
//			friendSet = it->second;
//			friendSet.insert(id2);
//			it->second = friendSet;
//		}
//		else {
//			friendSet.insert(id2);
//			pair<int, set<int> > setPair = make_pair(id1, friendSet);
//			payers.insert(setPair);		
//		}	
//	}
//
//	return payers;
//}

bool friendSearch(map<int, set<int> > payers, int payer, int payee, int depth, int counter=0){

//	cout<<"Check 0"<<endl;
	map<int, set<int> >::iterator it = payers.find(payer);

//	cout<<"Check 1"<<endl;
	//update payers if new payer
	if ( it != payers.end() ){
		set<int> friends = it->second;
		set<int>::iterator set_it = friends.find(payee);
		if(set_it != friends.end() ){	
			return true;
		}
		else if (depth > 0){
			counter++;
			set<int>::iterator f_it;
			int ff=0;
			set<int> failedFriends;
			for(f_it = friends.begin(); f_it != friends.end(); ++f_it){
	//			cout<<"Entering for loop, at friend :"<<ff<<endl;
				int ffriend = *f_it;
				if (friendfriendSearch(payers, failedFriends, ffriend, payee, depth, counter)){
	//				cout<<"found friend through recursion"<<endl;
					return true;	
				} 
				failedFriends.insert(ffriend);
				ff++;
			}			
	//		cout<<"For loop ended with no friends"<<endl;
			return false;
		}	
		else return false;
	}
	else {
		return false;
	}
}

//bool friendSearch(map<int, set<int> > payers, int payer, int payee, int depth, int counter=0){
//
////	cout<<"Check 0"<<endl;
//	map<int, set<int> >::iterator it = payers.find(payer);
//
////	cout<<"Check 1"<<endl;
//	//update payers if new payer
//	if ( it != payers.end() ){
//		set<int> friends = it->second;
//		set<int>::iterator set_it = friends.find(payee);
//		if(set_it != friends.end() ){	
//			return true;
//		}
//		else if (depth > 0){
//			counter++;
//			set<int>::iterator f_it;
//			int ff=0;
//			for(f_it = friends.begin(); f_it != friends.end(); ++f_it){
//	//			cout<<"Entering for loop, at friend :"<<ff<<endl;
//				int ffriend = *f_it;
//				if (friendfriendSearch(payers, ffriend, payee, depth, counter)){
//	//				cout<<"found friend through recursion"<<endl;
//					return true;	
//				} 
//				ff++;
//			}			
//	//		cout<<"For loop ended with no friends"<<endl;
//			return false;
//		}	
//		else return false;
//	}
//	else {
//		return false;
//	}
//}

bool friendfriendSearch(map<int, set<int> > payers, set<int> failedFriends, int payer, int payee, int depth, int counter=1){
//	cout<<"Counter is :"<<counter<<endl;
	if(counter > depth){ //depth of friendship ... 3 is (friend)^4
	//	cout<<"recursion depth too much, returning false"<<endl;
		return false;
	}

	//if it has failed before, it will fail again
	set<int>::iterator failed_it = failedFriends.find(payer);
	if(failed_it == failedFriends.end())
		return false;

//	cout<<"Check 0"<<endl;
	map<int, set<int> >::iterator it = payers.find(payer);

//	cout<<"Check 1"<<endl;
	//update payers if new payer
	if ( it != payers.end() ){
		set<int> friends = it->second;
		set<int>::iterator set_it = friends.find(payee);
		if(set_it != friends.end() ){	
			return true;
		}
		else{
			counter++;
			if(counter>depth){
//				cout<<"recursion depth too much, returning false"<<endl;
				return false;	
			}
			set<int>::iterator f_it;
			set<int> failed_friendfriendFriends;
			for(f_it = friends.begin(); f_it != friends.end(); ++f_it){
				int ffriend = *f_it;
				if (friendfriendSearch(payers, failed_friendfriendFriends, ffriend, payee, depth, counter)){
					return true;	
				} 
				failed_friendfriendFriends.insert(ffriend);
			}			
			return false;
		}	
	}
	else {
		return false;
	}
}	
