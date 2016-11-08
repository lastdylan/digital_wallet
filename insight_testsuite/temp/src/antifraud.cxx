#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <set>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

struct myTransaction {
	string time;
	int id1;
	int id2;
	double amount;
	string message;
};

struct transCompare {
	bool operator() (const myTransaction a, const myTransaction b) const{
		return a.id2 < b.id2;
	}
};

bool compare (const myTransaction a, const myTransaction b) {
		return a.id2 < b.id2;
}

vector<myTransaction> getRecords(const string &inputFile);

int main(int argc, char** argv){

	if(argc<2)cerr<<"Please input a file"<<endl;
	string inputfile = argv[1];
	string testfile = argv[2];

	vector<myTransaction> records = getRecords(inputfile);

	map<int, set<int> > payers;
	
	ifstream  testData(testfile);
  string testLine;


  while(std::getline(testData,testLine)){

		//skip the header line
		if(testLine.find("time") == 0 )continue;

		//grab a line as a transaction
    stringstream lineStream(testLine);
    string cell;		
		myTransaction transaction;
		int counter=0;
  	while(std::getline(lineStream,cell,',')){
			if(counter==0)transaction.time=cell;
			else if(counter==1)transaction.id1=stoi(cell);
			else if(counter==2)transaction.id2=stoi(cell);
			else if(counter==3)transaction.amount=stod(cell);
			else if(counter==4)transaction.message=cell;
		
			counter++;	
		}

		//has the payer been seen before?
		map<int, set<int> >::iterator it = payers.find(transaction.id1);
		if ( it != payers.end() ){
			set<int> ogSet = it->second;
			
			//is the payee a friend?
			set<int>::iterator set_it = ogSet.find(transaction.id2);

			//if payee isnt a friend, issue warning
			if ( set_it == ogSet.end() ){
				cout<<"unverified"<<endl;

				//for now add it to the friends and skip the line
			} 

		}	

		//build a set of friends from base records	
		set<int> friends;	
		for(auto trans : records){
			if(trans.id1 != transaction.id1)continue;
			friends.insert(trans.id2);	
		}

		pair<int, set<int> > setPair = make_pair(transaction.id1, friends);
		payers.insert(setPair);
		
		
	}
return 0;
}

vector<myTransaction> getRecords(const string &inputFile){

	std::ifstream  data(inputFile);
  std::string line;

	vector<myTransaction> transactions;
  while(std::getline(data,line)){
		if(line.find("time") == 0 )continue;

    std::stringstream lineStream(line);
    std::string cell;		

		myTransaction transaction;
		int counter=0;
  	while(std::getline(lineStream,cell,',')){
			if(counter==0)transaction.time=cell;
			else if(counter==1)transaction.id1=stoi(cell);
			else if(counter==2)transaction.id2=stoi(cell);
			else if(counter==3)transaction.amount=stod(cell);
			else if(counter==4)transaction.message=cell;
		
			counter++;	
		}

		transactions.push_back(transaction);
	}

	sort(transactions.begin(),transactions.end(),compare);

	return transactions;
}


