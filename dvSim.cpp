#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;


///data structures
struct node{

        int id;

        vector <int> destination;
        vector <int> cost;
        vector <int> nextHop;
        vector <int> neighbors;

        struct node* link;
    };


///global variables
struct node *nHead;


///function headers
void viewTopogrophy();
void createTable(string);
void createHead(int, int, int);
void createNode(int, int, int);
void initialNeighborUpdate(int, int, int);
bool findNode(int);



int main(int argc, char *argv[]){

    string fn = argv[1];
    createTable(fn);

    viewTopogrophy();

}







void viewTopogrophy(){
    int total =0;
    int largestNode = 0;
    int nodeToFind = 0;

    node* temp = nHead;
    while(temp != NULL){
        if(temp->id > largestNode){
            largestNode = temp->id;
        }
        temp=temp->link;
    }
    
    nodeToFind = largestNode;
    for(int x=0; x <= largestNode; x++){
        temp = nHead;
        while(temp->id != nodeToFind){
            temp=temp->link;
        }
        cout << "Node #" << temp->id<< endl;
        cout << "< destination, cost, nexthop > " << endl;
        for(int i=0; i<temp->destination.size(); i++){
            cout << "<" << temp->destination.at(i) << ", " << temp->cost.at(i) << ", "<< temp->nextHop.at(i) << ">" << endl;
        }
        cout << "size: " << temp->destination.size() << endl;

        cout << "Neighbors: <";
        for(int i=0; i<temp->neighbors.size(); i++){
            cout << temp->neighbors.at(i) << ", ";
        }
        cout << ">" << endl << endl;
        total++;
        nodeToFind--;
    }

    cout << endl  << "Total Nodes: " << total << endl;

}

void createHead(int i, int j, int k){
    nHead->id = i;
    nHead->destination.push_back(j);
    nHead->cost.push_back(k);
    nHead->nextHop.push_back(j);
    nHead->neighbors.push_back(j);
    nHead->link = NULL;

    node* temp = new node;
    temp->id = j;
    temp->destination.push_back(i);
    temp->cost.push_back(k);
    temp->nextHop.push_back(i);
    temp->neighbors.push_back(i);
    temp->link = NULL;

    nHead->link = temp;
}

void createTable(string fn){

    nHead = new node;
    nHead->id = -1;

    fstream f;
    f.open(fn);
    while(!f.eof()){
        int i, j, k;
        f >> i >> j >> k; 
        if(nHead->id == -1){
            createHead(i, j, k);
            continue;
        }
        if(!findNode(i))
            createNode(i, j, k);
        if(!findNode(j))
            createNode(j, i, k);
        if(findNode(i))
                initialNeighborUpdate(i, j, k);
        if(findNode(j))
            initialNeighborUpdate(j, i, k);
    }
    f.close();
}

bool findNode(int id){
    node* temp = nHead;
    while(temp != NULL){
        if(temp->id == id)
            return true;
        else
            temp = temp->link;
    }
    return false;
}

void createNode(int i, int j, int k){
    node* temp = nHead;
    while(temp->link != NULL){
        temp=temp->link;
    }

    node* newNode = new node;
    newNode->id = i;
    newNode->destination.push_back(j);
    newNode->cost.push_back(k);
    newNode->nextHop.push_back(j);
    newNode->neighbors.push_back(j);
    newNode->link=NULL;
    temp->link= newNode;
}

void initialNeighborUpdate(int i, int j, int k){
    node* temp = nHead;
    while(temp->id != i){
        temp=temp->link;
    }
    if(find(temp->neighbors.begin(), temp->neighbors.end(), j) != temp->neighbors.end()){
        return;
    }
    else{
        temp->destination.push_back(j);
        temp->cost.push_back(k);
        temp->nextHop.push_back(j);
        temp->neighbors.push_back(j);
    }
}