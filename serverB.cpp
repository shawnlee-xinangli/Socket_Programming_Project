/*
UDP Source code from: Socket Programming Reference - (Beej's-Guide)
                      "https://www.jianshu.com/p/6c065429d3c1"
*/
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <list>
#include <queue>
#include <sstream>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MY_PORT  31553
#define MAINSERVER_PORT 32553
#define IP_ADDRESS  "127.0.0.1"

using namespace std;
struct Graph{
    string country;
    unordered_map<int, int> userIdToIndex;
    unordered_map<int, int> indexToUserId;
    bool matrix[105][105];
    int degree[105];
    int size = 0;
};

struct Entry {
    int id;
    int value;
    Entry(int id, int value):id(id),value(value){}
};

bool operator< (Entry a,Entry b){
    if(a.value == b.value)  return a.id > b.id;
    else return a.value < b.value;
};

void init(bool input[105][105], int degree[105]){
    for (int i = 0; i < 105; i++) {
        degree[i] = 0;
        for (int j = 0; j < 105; j++) {
            input[i][j] = false;
        }
    }
};

void addNeibor(struct Graph& graph, string line){
    stringstream ss(line);

    int index = graph.size;

    int userId;
    ss >> userId;

    if (graph.userIdToIndex.find(userId) == graph.userIdToIndex.end()){
        graph.userIdToIndex.insert(pair<int, int>(userId,index));
        graph.indexToUserId.insert(pair<int, int>(index,userId));
        index++;
    }

    int neiborId;
    while (ss >> neiborId) {
        if (graph.userIdToIndex.find(neiborId) == graph.userIdToIndex.end()){
            graph.userIdToIndex.insert(pair<int, int>(neiborId,index));
            graph.indexToUserId.insert(pair<int, int>(index,neiborId));
            index++;
        }

        int userIndex = graph.userIdToIndex.at(userId);
        int neiborIndex = graph.userIdToIndex.at(neiborId);

        if (!graph.matrix[userIndex][neiborIndex] && userIndex != neiborIndex){
            graph.degree[userIndex]++;
            graph.degree[neiborIndex]++;
            graph.matrix[userIndex][neiborIndex] = true;
            graph.matrix[neiborIndex][userIndex] = true;
        }
    }

};

void buildGraph(ifstream& infile, list<struct Graph>& countryList){
    string line;
    unordered_map<int, int> userIdToIndex;
    unordered_map<int, int> indexToUserId;

    while(getline(infile,line)){
        if (!(line[0]>='0' && line[0] <='9')){
            cout << "A new country: " + line << endl;
            struct Graph toAdd;
            toAdd.country = line;
            init(toAdd.matrix,toAdd.degree);
            countryList.push_back(toAdd);
        }else {

            struct Graph graph = countryList.back();
            countryList.pop_back();
            addNeibor(graph,line);
            graph.size = graph.userIdToIndex.size();
            countryList.push_back(graph);
        }
    }
    cout<< "Number of Countries in ServerB:" << countryList.size() <<endl;
    for (auto graph : countryList) {
        cout<< graph.country << " ";
    }
    cout << endl;
    cout << endl;
};

int getComNeibor(struct Graph& graph, int userIndex, int otherIndex){
    int count = 0;
    for (int neibor = 0; neibor < graph.size; neibor++){
        if (neibor == userIndex) continue;
        if (graph.matrix[userIndex][neibor] && graph.matrix[neibor][otherIndex]){
            count++;
        }
    }
    return count;
};

int getRecomd(struct Graph& graph, int userId){
    int userIndex = graph.userIdToIndex.at(userId);
    if (graph.degree[userIndex] == graph.size - 1){
        return -1;
    }

    priority_queue<Entry> pq;

    for (int otherIndex = 0; otherIndex < graph.size; otherIndex++){
        if (otherIndex == userIndex) continue;
        if (!graph.matrix[userIndex][otherIndex]){
            pq.push(Entry(graph.indexToUserId.at(otherIndex), getComNeibor(graph, userIndex, otherIndex)));
        }
    }

    if (pq.top().value == 0){
        int res = pq.top().id;
        int maxDegree = graph.degree[graph.userIdToIndex.at(pq.top().id)];
        pq.pop();
        while (!pq.empty()){
            struct Entry entry = pq.top();
            int index = graph.userIdToIndex.at(entry.id);
            if (graph.degree[index] > maxDegree || (graph.degree[index] == maxDegree && index < res)){
                res = entry.id;
                maxDegree = graph.degree[index];
            }
            pq.pop();
        }
        return res;
    }

    return pq.top().id;

};

void buildUp(list<struct Graph>& countryList){
    ifstream infile;

    string path = "./data2.txt";
    infile.open(path);

    cout << "Reading from the file "<< path << " and building graph" << endl;

    buildGraph(infile,countryList);

    infile.close();

};

void process(char recv_buf[1024],char send_buf[1024], list<struct Graph>& countryList){
    string str ="";
    if (recv_buf[0] == '0') {
        for(auto x : countryList){
            str += x.country +" ";
        }
        str.copy(send_buf,str.length(),0);
        send_buf[str.length()] = '\0';
        cout<< "The server B has sent a country list to Main Server"<<endl;
    }else {
        string country = "";
        string userIdString = "";
        int index = 2;

        while (recv_buf[index] != ' '){
            country += recv_buf[index];
            index++;
        }
        index++;
        while (recv_buf[index] != ' '){
            userIdString += recv_buf[index];
            index++;
        }
        int userId = stoi(userIdString);
        for(auto graph : countryList){
            if (graph.country == country){
                if(graph.userIdToIndex.find(userId) == graph.userIdToIndex.end()){
                    cout<< "User " << userIdString << " does not show up in "<< country << endl;
                    str = "User " + userIdString +" not found";
                    str.copy(send_buf,str.length(),0);
                    send_buf[str.length()] = '\0';
                    cout<< "The server B has sent \"User "+ userIdString << " not found\" to Main Server" << endl;
                }else {
                    int recomId = getRecomd(graph,userId);
                    cout<< "The server B is searching possible friends for User"<< userIdString +" …"<< endl;
                    if (recomId < 0) {
                        str = "No Recommendation for User " + userIdString;
                        str.copy(send_buf,str.length(),0);
                        send_buf[str.length()] = '\0';
                    }else {
                        int numComNeibors = getComNeibor(graph,graph.userIdToIndex.at(userId),graph.userIdToIndex.at(recomId));
                        str = "Recommend new friend: "+ to_string(recomId) + "(with number of common neibors = " + to_string(numComNeibors) +")";
                        str.copy(send_buf, str.length(),0);
                        send_buf[str.length()] = '\0';
                        cout << "Here are the result: User " << recomId <<endl;
                    }
                    cout << "The server B has sent the result(s) to Main Server" << endl;
                }
            }
        }
    }
}

int UDP(list<struct Graph>& countryList){
    /* build UDP socket*/
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    /* set server address */
    struct sockaddr_in addr_serv;
    //reset addr_serv
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(MY_PORT);
    addr_serv.sin_addr.s_addr = inet_addr(IP_ADDRESS);

//    int len_serv = sizeof(addr_serv);

    /* bind socket */
    if(bind(sock_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0){
        perror("bind error:");
        exit(1);
    }


    int recv_num;
    int send_num;

    struct sockaddr_in addr_client;
    int len_client;
    //reset addr_client
    memset(&addr_client, 0, sizeof(addr_client));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(MAINSERVER_PORT);

    addr_serv.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    len_client = sizeof(addr_client);
    char send_buf[1024];
    char recv_buf[1024];

    while(1)
    {
        memset(recv_buf, 0, sizeof(recv_buf));
        memset(send_buf, 0, sizeof(send_buf));

        printf("server wait:\n");
        recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_client, (socklen_t *)& len_client);


        if(recv_num < 0){
            perror("recvfrom error:");
            exit(1);
        }


        recv_buf[recv_num] = '\0';
        
        process(recv_buf, send_buf,countryList);
     
        send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&addr_client, len_client);

        if(send_num < 0){
            perror("sendto error:");
            exit(1);
        }
    }

    close(sock_fd);

    return 0;
}

int main() {
    list<struct Graph> countryList;
    buildUp(countryList);
    cout<< "The server B is up and running using UDP on port " << MY_PORT <<endl;
    UDP(countryList);
    return 0;
}