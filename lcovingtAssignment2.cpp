//Assignment 2 MultiThreaded Version
//To compile: g++ -O3 -w lcovingtAssignment2.cpp -lpthread -o lcovingtAssignment2
//To run: ./lcovingtAssignment2 filePath numberOfThreads
// ./lcovingtAssignment2 networkDatasets/toyGraph1.txt


#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>  /* atoi */
#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>

using namespace std;


typedef vector<vector<int> > AdjacencyMatrix;


//GLOBAL VARIABLES
const int MAX_NUMBER_OF_THREADS = 16;

int numberOfNodes;

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
    pthread_mutex_lock(&mutex);

    globalSum = globalSum + amount;

    pthread_mutex_unlock(&mutex);
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

void *ThreadWork(void* threadID){

    long my_threadID =(long) threadID;

    int remainder = (numberOfNodes%numberOfThreads);
    int z = (numberOfNodes/numberOfThreads);

    int start, end;
    if(my_threadID == 0){
	start = my_threadID*z;
        end = start + z + remainder;
    }
    else{
        start = (my_threadID*z)+remainder;
        end = start + z;
    }

    for(int i = start; i < end; i++){
	calcAndSaveIndividualClustering(i);
    }

    return NULL;


}

int main(int argc, char** argv){

    char* filePath = argv[1];
    numberOfThreads = atoi(argv[2]);

    long       thread;
    pthread_t* thread_handles;

    if(numberOfThreads > MAX_NUMBER_OF_THREADS || numberOfThreads < 1){
	cout << "Must use 1 - 16 threads only";
       return -1;
    }

    initialize(filePath);

    if(numberOfThreads > numberOfNodes){
	numberOfThreads = numberOfNodes;
    }

    thread_handles = (pthread_t*) malloc(numberOfThreads*sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);


    for(thread = 0; thread < numberOfThreads; thread++){
	//create threads here
	pthread_create(&thread_handles[thread], NULL, ThreadWork, (void*) thread);
    }

    for( thread = 0; thread < numberOfThreads; thread++){
	//join threads here
	pthread_join(thread_handles[thread], NULL);
    }

    free(thread_handles);
    pthread_mutex_destroy(&mutex);

    float graphClusteringCoefficient = globalSum / ((float)numberOfNodes);

    cout <<"The graph's clustering coefficient is: " << graphClusteringCoefficient << "\n";

}

