#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <time.h>
#include <chrono>
#include <set>
#include <vector>
#include <algorithm>
#include <map>
#include <iomanip>

using namespace std;
typedef std::chrono::high_resolution_clock Clock;

map<int, set<int> > getRecords(const string &inputFile, bool debug);

bool friendSearch(map<int, set<int> > &users, int payer, int payee, int depth, bool debug=false);
bool friendTwoSearch(map<int, set<int> > &users, set<int> &payer_friends, int payee, int depth, bool debug=false);
bool friendThreeSearch(map<int, set<int> > &users, set<int> &payer_friends, set<int> &payee_friends, bool debug=false);

int main(int argc, char** argv){

	if(argc<6){
		cout<<"Usage : ./antifraud <batch_input> <stream_input> <output_1> <output_2> <output_3> <XTRA_FLAGS>"<<endl;
		cout<<"Please specify at least input and output files!"<<endl;
		return 0;
	}

	//grab batch and stream input file names
	string batch_input = argv[1];
	string stream_input = argv[2];

	//grab output file names and open them
	ofstream outfile1, outfile2, outfile3;
	outfile1.open(argv[3]);
	outfile2.open(argv[4]);
	outfile3.open(argv[5]);

	//set special control flags [1)debug and 2) depth of friendship, 3 being friendship to the 4th order]
	bool debug=false;
	int depth=3;
	for(int i=6;i<argc;++i){
		if(string(argv[i])=="--debug")debug=true;
		else if(string(argv[i])=="--depth"){
			depth=atoi(argv[i+1]);
		}
	}

	//map every user in the batch file with a set of friends 
	auto rec_start = Clock::now();
	map<int, set<int> > users = getRecords(batch_input, debug);
	auto rec_end = Clock::now();

	if(debug)cout<<"Grabbed records from batch file in "<<chrono::duration_cast<chrono::seconds>(rec_end - rec_start).count()<<" seconds"<<endl;

	//start streaming new transactions
	ifstream  streamData(stream_input.c_str());
  string streamLine;

	cout<<"Starting to stream transactions ... "<<endl;
  int event_counter=0;
	auto start_stream = Clock::now();
  while(getline(streamData,streamLine)){

		//skip the header line
		if(streamLine.find("time") == 0 )continue;
    event_counter++;

    if(event_counter%100000==0) cout << event_counter << endl;

		//grab a line as a transaction
    stringstream lineStream(streamLine);
    string cell;		
		int counter=0;
		int id1,id2;
  	while(getline(lineStream,cell,',')){
			if(counter==1)id1=stoi(cell);
			else if(counter==2)id2=stoi(cell);
		
			counter++;	
		}

		//assess friendships between id1 and id2, from the map users
		auto t1 = Clock::now();
		bool checkTrust = friendSearch(users, id1, id2, 0, false);
		auto t2 = Clock::now();

		if (checkTrust)
			outfile1<<"trusted";
		else if(!checkTrust)
			outfile1<<"unverified";
		if (debug)
			outfile1<<" ... "<<chrono::duration_cast<chrono::microseconds>(t2 - t1).count()<<" microseconds";
		outfile1<<endl;

		if(depth>0){
			t1 = Clock::now();
			bool checkTrust = friendSearch(users, id1, id2, 1,false);
			t2 = Clock::now();

			if (checkTrust)
				outfile2<<"trusted";
			else if(!checkTrust)
				outfile2<<"unverified";
			if (debug)
				outfile2<<" ... "<<chrono::duration_cast<chrono::microseconds>(t2 - t1).count()<<" microseconds";
			outfile2<<endl;
			
			if(depth==3){
				t1 = Clock::now();
				bool checkTrust = friendSearch(users, id1, id2, depth, debug);
				t2 = Clock::now();

				if (checkTrust)
					outfile3<<"trusted";
				else if(!checkTrust)
					outfile3<<"unverified";
				if (debug)
					outfile3<<" ... "<<chrono::duration_cast<chrono::microseconds>(t2 - t1).count()<<" microseconds";
				outfile3<<endl;
			}
		}
	}
	auto end_stream = Clock::now();
	cout<<"Done streaming in "<<chrono::duration_cast<chrono::minutes>(end_stream - start_stream).count()<<" minutes"<<endl;

	outfile1.close();
	outfile2.close();
	outfile3.close();
	return 0;
}

//-------------------------------------------------------------------------------

map<int, set<int> > getRecords(const string &batchFile, bool debug){

	FILE *file;
	file = fopen(batchFile.c_str(),"r");

	const int headerSize=65536;
	char header[headerSize];
	fgets(header, headerSize, file);

	//Fill each line into a map<user, set<friends> > ... friends could be payers or payees!
	map<int, set<int> > users;
	char time[256];
	int id1, id2;
	float amount;
	int event_counter;
	while (!feof(file)){
    event_counter++;
    if(event_counter%100000==0 && debug) cout << event_counter << endl;

		fscanf(file,"%20[^,], %i, %i, %f", time, &id1, &id2, &amount);

	  //message field has length of largely varying sizes; handle separately; 'header' is just a buffer
		fgets(header, headerSize, file);

		//add payee to payer's friends
		map<int, set<int> >::iterator it_id1 = users.find(id1);
	
		if(it_id1 != users.end()){
			it_id1->second.insert(id2);
		}
		else {
			set<int> id1_friends;
			id1_friends.insert(id2);
			users.insert(make_pair(id1, id1_friends));		
		}	

	 	//add payer to payee's friends
		map<int, set<int> >::iterator it_id2 = users.find(id2);

	  if(it_id2 != users.end()){
			it_id2->second.insert(id1);		
		}
		else {
			set<int> id2_friends;
			id2_friends.insert(id1);
			users.insert(make_pair(id2, id2_friends));		
		}	
	}
	return users;
}

//-------------------------------------------------------------------------------

bool friendSearch(map<int, set<int> > &users, int payer, int payee, int depth, bool debug){

/*This function looks for payee in payer's O(1) friend network.
If found, it returns true. Otherwise, depending on the requested 
depth of friend network, it either returns false or calls a function that 
processes O(2) friends
*/

	map<int, set<int> >::iterator it = users.find(payer);

	bool decision=false;
	if ( it != users.end() ){
		if(debug)cout<<"friendSearch:: Found payer in users"<<endl;
		set<int>::iterator set_it = it->second.find(payee);
		if(set_it != it->second.end() ){	
			if(debug)cout<<"friendSearch:: Found payee in payer's friend network"<<endl;
			decision=true;
		}
		else if(depth>0){
			if(debug)cout<<"friendSearch:: Payee not in payer's friend network, proceeding to O(2) network"<<endl;
			decision = friendTwoSearch(users, it->second, payee, depth, debug);
		}
	}

	if(decision){
		if(debug)cout<<"friendSearch:: Transaction verified"<<endl;
		return true;
	}
	else
		return false;
}

//-------------------------------------------------------------------------------

bool friendTwoSearch(map<int, set<int> > &users, set<int> &payer_friends, int payee, int depth, bool debug){

/* If payer and payee are O(2) friends, then they must have a friend in common.
This function looks for an intersection between payer's friends and payee's friends.
If present, it returns true. Otherwise, depending on the requested depth it either returns 
false or passes the baton to a function that processes O(3) and O(4) friendships. 
*/

	map<int, set<int> >::iterator it_payee = users.find(payee);
	bool decision=false;
	if ( it_payee != users.end() ){
		if(debug)cout<<"friendTwoSearch:: Found payee in users, now searching for common denominators ..."<<endl;
		set<int> intersect;
		set_intersection(it_payee->second.begin(),it_payee->second.end(),payer_friends.begin(),payer_friends.end(),inserter(intersect,intersect.begin()));
		if( intersect.size() > 0 )decision=true;	
		if(debug)cout<<"friendTwoSearch:: Finished search for common denominators"<<endl;

		if( !decision && depth==3 ){
			if(debug)cout<<"friendTwoSearch:: No friendship at O(2), proceeding to O(3)/O(4)"<<endl;
			decision = friendThreeSearch(users, payer_friends, it_payee->second, debug);
			if(debug)cout<<"friendTwoSearch:: Finished with O(3)/O(4) analysis"<<endl;
		}
	}
	
	if(decision)
		return true;
	else
		return false;
}

//-------------------------------------------------------------------------------

bool friendThreeSearch(map<int, set<int> > &users, set<int> &payer_friends, set<int> &payee_friends, bool debug){

/* O(4) friends must share a friend in their set of friends of friends.
For O(3) either payer_friends intersects with payee_friends' friends, or payee_friends intersects 
with payer_friends' friends. 
This function looks for O(3) intersections first. 
If present, it returns true, otherwise it looks for O(4) intersections and returns true or false
*/

	set<int>::iterator payer_friends_it;
	set<int>::iterator payee_friends_it;

	bool decision=false;
	//Let's check if O(3) is satisfied before going O(4)
	for( int payer_friend : payer_friends ){
		map<int, set<int> >::iterator it = users.find(payer_friend);
		if(it != users.end()){
			set<int> intersect;
			set_intersection(it->second.begin(),it->second.end(),payee_friends.begin(),payee_friends.end(),inserter(intersect,intersect.begin()));
			if( intersect.size() > 0 ){
				decision=true;	
				break;
			}
		}
	}
	if(!decision){
		for( int payee_friend : payee_friends ){
			map<int, set<int> >::iterator it = users.find(payee_friend);
			if(it != users.end()){
				set<int> intersect;
				set_intersection(it->second.begin(),it->second.end(),payer_friends.begin(),payer_friends.end(),inserter(intersect,intersect.begin()));
				if( intersect.size() > 0 ){
					decision=true;	
					break;
				}
			}
		}
	}
	//If O(3) failed, try O(4)
	if(!decision){
		if(debug)cout<<"friendThreeSearch:: Looping through "<<payer_friends.size()<<" payer's friends"<<endl;
		for( int payer_friend : payer_friends ){
			map<int, set<int> >::iterator it = users.find(payer_friend);
			if(it != users.end()){
				if(debug)cout<<"friendThreeSearch:: Looping through "<<payee_friends.size()<<" payee's friends"<<endl;
				for( int payee_friend : payee_friends ){
					map<int, set<int> >::iterator it_payee = users.find(payee_friend);
					if(it_payee != users.end()){
						set<int> intersect;
						set_intersection(it->second.begin(),it->second.end(),it_payee->second.begin(),it_payee->second.end(),inserter(intersect,intersect.begin()));
						if( intersect.size() > 0 ){
							decision=true;	
							break;
						}
					}	
				}
			}	
		}
	}

	return decision;
}
