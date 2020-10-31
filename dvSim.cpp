//Mina Guirguis CS4310
//Project 1 Distance Vector Simulator
//Author Jeffrey C. Slocum

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
    struct packet* recieved;
    struct packet* send;

};

struct packet{

    int srcNode;
    int destNode;
    vector <int> destination;
    vector <int> cost;
    struct packet* next;
    
};


///global variables
struct node *nHead;
int packetRecieved, packetCreated = 0;
int lastNodeUpdated = -1;
bool tableUpdated = true;
int rounds = 0;
string fn;


///function headers
void viewTopogrophy();
void createTable(string);
void createHead(int, int, int);
void createNode(int, int, int);
void createDV();
void sendDV();
void addMissingNodes(struct node*);
void updateNodes(struct node*);
void initialNeighborUpdate(int, int, int);
void whatsInR();
void whatsInS();
void updateTables();
void packetRoute();
bool findNode(int);
int findCost(struct node*, struct packet*);


int main(int argc, char *argv[]){

    fn = argv[1];

    createTable(fn);

    while(tableUpdated){
        createDV();
        sendDV();
        updateTables();
        if(tableUpdated)
            rounds++;
    };

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
        int sending = 0;
        int recieving = 0;
        
        while(temp->id != nodeToFind){
            temp=temp->link;
        }

        packet* s = new packet;
        s=temp->send;
        while(s != NULL){
            sending++;
            s=s->next;
        }

        packet* r = new packet;
        r=temp->recieved;
        while(r != NULL){
            recieving++;
            r=r->next;
        }

        cout << "Node #" << temp->id<< endl;
        cout << "< destination, cost, nexthop > " << endl;
        for(int i=0; i<temp->destination.size(); i++){
            cout << "<" << temp->destination.at(i) << ", " << temp->cost.at(i) << ", "<< temp->nextHop.at(i) << ">" << endl;
        }
        cout << "size: " << temp->destination.size() << endl;
        //cout << "sending: " << sending << endl;
        //cout << "recieving: " << recieving << endl;

        cout << "Neighbors: <";
        for(int i=0; i<temp->neighbors.size(); i++){
            cout << temp->neighbors.at(i) << ", ";
        }
        cout << ">" << endl << endl;
        total++;
        nodeToFind--;
    }

    cout << endl << "---------------------------------------------------" << endl;
    cout << endl << "Total Nodes: " << total << endl;
    cout << "Total rounds: " << rounds << endl;
    cout << "packetsCreated: " << packetCreated << endl; 
    //cout << "packetRecieved: " << packetRecieved << endl;
    cout << "lastNodeUpdated: " << lastNodeUpdated << endl;

    cout << endl << "---------------------------------------------------" << endl;

}

void createHead(int i, int j, int k){
    nHead->id = i;
    nHead->destination.push_back(j);
    nHead->cost.push_back(k);
    nHead->nextHop.push_back(j);
    nHead->neighbors.push_back(j);
    nHead->link = NULL;
    nHead->send=NULL;
    nHead->recieved=NULL;

    node* temp = new node;
    temp->id = j;
    temp->destination.push_back(i);
    temp->cost.push_back(k);
    temp->nextHop.push_back(i);
    temp->neighbors.push_back(i);
    temp->link = NULL;
    temp->send = NULL;
    temp->recieved = NULL;

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
    newNode->recieved=NULL;
    newNode->send=NULL;
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


void createDV(){
    node* temp = nHead;
    
    while(temp != NULL){

        for(int i=0; i<temp->neighbors.size(); i++){
    
            packet* tempPack = new packet;
            tempPack->srcNode = temp->id;
            tempPack->destNode = temp->neighbors.at(i);
            tempPack->next = NULL;

            for(int j=0; j<temp->cost.size(); j++){
                tempPack->destination.push_back(temp->destination.at(j));
                tempPack->cost.push_back(temp->cost.at(j));
            }

            if(temp->send == NULL)
                temp->send = tempPack;
            else{
                packet* s = new packet;
                s = temp->send;
                while(s->next !=NULL){
                    s=s->next;
                }
                s->next = tempPack;
            }
            packetCreated++;
        }
        temp = temp->link;
    }

}


void sendDV(){

    node* temp = nHead;
    while(temp != NULL){
        packet* sHead = temp->send;
        while(sHead != NULL){
            node* find = nHead;
            while(find->id != sHead->destNode){
                find = find->link;
            }

            packet* s = new packet;
            if(find->recieved == NULL){
                find->recieved = sHead;
                sHead=sHead->next;
                s = find->recieved;
                s->next=NULL;
            }
            else{
                
                s = find->recieved;
                while(s->next != NULL){
                    s=s->next;   
                }
                s->next = sHead;
                sHead = sHead->next;
                s=s->next;
                s->next= NULL;
            }
            packetRecieved++;
        }
        temp->send=NULL;
        temp= temp->link;
    }
}


void updateTables(){
    node* temp = nHead;
    while(temp != NULL){

        tableUpdated = false;
        addMissingNodes(temp);
        updateNodes(temp);
        temp->recieved=NULL;
        temp=temp->link;
    }
}

void addMissingNodes(node *temp){
    packet* sHead = temp->recieved;
    //loop sent packets
    while(sHead != NULL){
        //loop through vectors in sent packets
        for(int i=0; i<sHead->destination.size(); i++){
            int tableSize=temp->destination.size();
            //loop through the table
            for(int j=0; j<tableSize; j++){
                //if not found in the table
                //and is not itself
                //add it to the table
                if(!(find(temp->destination.begin(), temp->destination.end(), sHead->destination.at(i)) != temp->destination.end()) && temp->id != sHead->destination.at(i)){

                    int cost = 0;
                    cost = findCost(temp, sHead);

                    temp->destination.push_back(sHead->destination.at(i));
                    temp->cost.push_back(sHead->cost.at(i) + cost);
                    temp->nextHop.push_back(sHead->srcNode);
                    tableUpdated = true;
                    lastNodeUpdated = temp->id;
                }
            }         
        }
        sHead = sHead->next;
    }
}

void updateNodes(node *temp){

    packet* pack = temp->recieved;
    while(pack != NULL){
        //loop through vectors in sent packets
        for(int i=0; i<pack->destination.size(); i++){
            //loop through the table
            for(int j=0; j<temp->destination.size(); j++){
                if(pack->destination.at(i) == temp->destination.at(j)){

                    int cost = 0;
                    cost = findCost(temp, pack);

                    /*
                    cout << "node #" << temp->id << endl;
                    cout << "from: " << pack->srcNode << endl;
                    cout << "dest: " << pack->destination.at(i) << " & " << temp->destination.at(j) << endl;
                    cout << "current cost: " << temp->cost.at(j) << endl;
                    cout << "cost to src: " << cost << endl;
                    cout << "total cost: " << temp->cost.at(j) + cost << endl << endl;
                    */

                    if((pack->cost.at(i) + cost) < temp->cost.at(j)){
                        temp->cost.at(j) = pack->cost.at(i) + cost;
                        temp->nextHop.at(j) = pack->srcNode;
                        tableUpdated = true;
                        lastNodeUpdated = temp->id;
                    }
                }
            }
        }
        pack=pack->next;
    }
}


void whatsInR(){
    node* temp = nHead;
    cout << endl << endl <<  "whats in R" << endl;
    while(temp != NULL){
        cout << "Node #" << temp->id << endl;
        cout << "< source, destination > " << endl;
        packet* r = new packet;
        r=temp->recieved;
        while(r != NULL){
            cout << "<" << r->srcNode << ", " << r->destNode << ">" << endl;
            for(int i=0; i<r->destination.size(); i++){
                cout << "desination: " << r->destination.at(i) << "  cost: " << r->cost.at(i) << endl;
            }
            r=r->next;
        }
        temp=temp->link;
    }
}


void whatsInS(){
    node* temp = nHead;
    cout << endl << endl << "whats in S" << endl;
    while(temp != NULL){

        cout << "Node #" << temp->id << endl;
        cout << "< source, destination > " << endl;

        packet* r = new packet;
        r=temp->send;
        while(r != NULL){
            cout << "<" << r->srcNode << ", " << r->destNode << ">" << endl;
            r=r->next;
        }
        temp=temp->link;
    }
}


int findCost(node *temp, packet *sHead){
    node* findCost = temp;
    int cost = 0;
    for(int k=0; k<findCost->destination.size(); k++){
        if(findCost->destination.at(k) == sHead->srcNode){
            cost = findCost->cost.at(k);
        }
    }
return cost;
}


void packetRoute(){
    node* temp = nHead;
    int bigNode, nodeToFind = 0;
    while(temp != NULL){

        if(temp->id > bigNode){
            bigNode = temp->id;
        }
        temp = temp->link;
    }

    if(bigNode<7)
        nodeToFind = 3;
    else if(bigNode<23)
        nodeToFind = 7;
    else
        nodeToFind = 23;

    temp = nHead;
    while(temp->id != 0){
        temp = temp->link;
    }
    cout << "Node to find: " << nodeToFind << endl;

    vector <int> route;
    route.push_back(0);
    int currentNode = 0;
    int nextHop = 0;
    while(temp->id != nodeToFind){
        for(int i=0; i<temp->destination.size(); i++){
            if(temp->destination.at(i) == nodeToFind){
                route.push_back(temp->nextHop.at(i));
                currentNode=temp->id;
                nextHop = temp->nextHop.at(i);
                break;
            }
        }
        temp = nHead;
        while(temp->id != nextHop){
            temp = temp->link;
        }   
    }
    cout << "< ";
    for(int k =0; k<route.size(); k++){
        cout << route.at(k) << ", ";
    }
    cout << ">" << endl;


}
