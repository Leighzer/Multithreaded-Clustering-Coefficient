//Assignment 2 Serial Version
//To compile: g++ -O3 -w lcovingtAssignment2Serial.cpp -lpthread -o lcovingtAssignment2Serial
//To run: ./lcovingtAssignment2Serial filePath
// ./lcovingtAssignment2Serial networkDatasets/toyGraph1.txt


#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>  /* atoi */
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;





typedef vector<vector<int> > AdjacencyMatrix;


//GLOBAL VARIABLES
const int MAX_NUMBER_OF_THREADS = 16;

float numberOfNodes;

AdjacencyMatrix adjMatrix;

float globalSum;


//MultiThreaded Variables
int numberOfThreads;
pthread_mutex_t mutex;





void initialize(char* filePath){//initialize all variables to support our calculations

    fstream myfile(filePath,std::ios_base::in);
    int u,v;
    int maxNode = 0;
    vector<pair<int,int> > allEdges;
    while(myfile >> u >> v)
    {
        allEdges.push_back(make_pair(u,v));
        if(u > maxNode)
          maxNode = u;

        if(v > maxNode)
          maxNode = v;
    }

    numberOfNodes = (float) maxNode +1;  //Since nodes starts with 0
    //cout<<"Graph has "<< n <<" nodes"<<endl;

    adjMatrix = AdjacencyMatrix(numberOfNodes,vector<int>(numberOfNodes));

    //populate the adjacency matrix
    for(int i =0; i<allEdges.size() ; i++){
       u = allEdges[i].first;
       v = allEdges[i].second;
       adjMatrix[u][v] = 1;
       adjMatrix[v][u] = 1;
    }

    globalSum = 0;

}

bool isFriend(int person1, int person2){

    if(adjMatrix[person1][person2] == 1 || adjMatrix[person2][person1] == 1){
	return true;
    }

    return false;

}

void addToGlobalSum(float amount){
    //mutex business here for multithread

    globalSum = globalSum + amount;

    //end mutex business here for multithread
}

void debugCalcAndSavePrint(int person, float degree, float m, float coef){

    cout << "Person: " << person << "\n";
    cout << "Degree: " << degree << "\n";
    cout << "M Value: " << m << "\n";
    cout << "Calc'd Coef:" << coef << "\n";

    cout << "\n";

}

void calcAndSaveIndividualClustering(int person){

    vector<int> neighbors = vector<int>(0);

    for(int i = 0; i < numberOfNodes; i++){
	if(isFriend(person, i)){
	    neighbors.push_back(i);
	}
    }

    float degree = (float) neighbors.size();

    float m = 0.0;

    for(int i = 0; i < neighbors.size(); i++){
	for(int j = i; j < neighbors.size(); j++){
	    if(isFriend(neighbors[j],neighbors[i])){
		m = m + 1;
	    }
	}
    }

    if(m == 0.0){
        return;
    }

    float individualClusteringCoef = (2.0*m)/(degree*(degree - 1.0));

    addToGlobalSum(individualClusteringCoef);

}


int main(int argc, char** argv){

    char* filePath = argv[1];
    //numberOfThreads = argv[2];

    initialize(filePath);

    for(int i = 0; i < numberOfNodes; i++){
	calcAndSaveIndividualClustering(i);
    }

    float graphClusteringCoefficient = globalSum / numberOfNodes;

    cout <<"The graph's clustering coefficient is: " << graphClusteringCoefficient << "\n";

}

