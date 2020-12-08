/*
TCP Source code from: Socket Programming Reference - (Beej's-Guide)
                      "https://www.jianshu.com/p/3b233facd6bb"

UDP Source code from: Socket Programming Reference - (Beej's-Guide)
                      "https://www.jianshu.com/p/6c065429d3c1"
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unordered_map>

using namespace std;

#define ServerA_PORT 30553
#define ServerB_PORT 31553
#define UDP_PORT 32553
#define TCP_PORT 33553
#define IP_ADDRESS  "127.0.0.1"

void readCountryfromA(unordered_map<string, int>& map, char recv_buf[1024], int recv_num){
    string word = "";
    for (int i = 0; i < recv_num; i++){
        char x = recv_buf[i];
        if (x == ' '){
            map.insert(pair<string, int>(word,0));
            word = "";
        }
        else{
            word += x;
        }
    }
}

void readCountryfromB(unordered_map<string, int>& map, char recv_buf[1024], int recv_num){
    string word = "";
    for (int i = 0; i < recv_num; i++){
        char x = recv_buf[i];
        if (x == ' '){
            map.insert(pair<string, int>(word,1));
            word = "";
        }
        else{
            word += x;
        }
    }
}

int getCountryListFromA(unordered_map<string, int>& map){
    /* build UDP socket */
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    /* set address */
    struct sockaddr_in addr_serv;
    int len;
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    addr_serv.sin_port = htons(ServerA_PORT);
    len = sizeof(addr_serv);


    int send_num;
    int recv_num;
    char send_buf[1024] = "0 ServerA, would you please give me the countryList?";
    char recv_buf[1024] = "";


    send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&addr_serv, len);

    if(send_num < 0)
    {
        perror("sendto error:");
        exit(1);
    }

    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);

    if(recv_num < 0)
    {
        perror("recvfrom error:");
        exit(1);
    }

    recv_buf[recv_num] = '\0';

    readCountryfromA(map, recv_buf, recv_num);

    close(sock_fd);

    return 0;
}

int getCountryListFromB(unordered_map<string, int>& map){
    /* buil UDP socket */
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    /* set address */
    struct sockaddr_in addr_serv;
    int len;
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    addr_serv.sin_port = htons(ServerB_PORT);
    len = sizeof(addr_serv);


    int send_num;
    int recv_num;
    char send_buf[1024] = "0 ServerB, would you please give me the countryList?";
    char recv_buf[1024] = "";


    send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&addr_serv, len);

    if(send_num < 0)
    {
        perror("sendto error:");
        exit(1);
    }

    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);

    if(recv_num < 0)
    {
        perror("recvfrom error:");
        exit(1);
    }

    recv_buf[recv_num] = '\0';

    readCountryfromB(map, recv_buf, recv_num);


    close(sock_fd);

    return 0;
}

int processUDP(char recv_buf[1024], char tcp_buf[255]){
    
    int index = 0;

    while (recv_buf[index] != '\0'){
        tcp_buf[index] = recv_buf[index];
        index++;
    }
    tcp_buf[index] = '\0';
    return index;
}

int getInfoFromA(string country, int userId, char tcp_buf[255]){
    /* build UDP socket */
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    /* set address */
    struct sockaddr_in addr_serv;
    int len;
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    addr_serv.sin_port = htons(ServerA_PORT);
    len = sizeof(addr_serv);


    int send_num;
    int recv_num;

    string sendMes = "1 "+ country + " " + to_string(userId)+ " ";
    char send_buf[1024] = "";
    int sendMeslen = sendMes.length();
    sendMes.copy(send_buf,sendMeslen,0);
    send_buf[sendMeslen] = '\0';

    char recv_buf[1024] = "";


    send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&addr_serv, len);

    if(send_num < 0)
    {
        perror("sendto error:");
        exit(1);
    }

    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);

    if(recv_num < 0){
        perror("recvfrom error:");
        exit(1);
    }

    recv_buf[recv_num] = '\0';

    int mesLen = processUDP(recv_buf,tcp_buf);


    if(recv_buf[0] == 'U'){
        cout<< "The Main Server has received \"User ID: Not found\" from server A"<< endl;
    }else {
        cout << "The Main server has received searching result(s) of User "<< userId <<" from server A"<<endl;
    }

    close(sock_fd);

    return mesLen;
}

int getInfoFromB(string country, int userId, char tcp_buf[255]){
    /* build UDP socket */
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    /* set address */
    struct sockaddr_in addr_serv;
    int len;
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    addr_serv.sin_port = htons(ServerB_PORT);
    len = sizeof(addr_serv);


    int send_num;
    int recv_num;

    string sendMes = "1 "+ country + " " + to_string(userId)+ " ";
    char send_buf[1024] = "";
    int sendMeslen = sendMes.length();
    sendMes.copy(send_buf,sendMeslen,0);
    send_buf[sendMeslen] = '\0';

    char recv_buf[1024] = "";


    send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&addr_serv, len);

    if(send_num < 0)
    {
        perror("sendto error:");
        exit(1);
    }

    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);

    if(recv_num < 0){
        perror("recvfrom error:");
        exit(1);
    }

    recv_buf[recv_num] = '\0';

    int mesLen = processUDP(recv_buf,tcp_buf);


    if(recv_buf[0] == 'U'){
        cout<< "The Main Server has received \"User " << userId<<": Not found\" from server B"<< endl;
    }else {
        cout << "The Main server has received searching result(s) of User "<< userId <<" from server B"<<endl;
    }

    close(sock_fd);

    return mesLen;
}

int processTCP(char recv_buf[255],char send_buf[255], unordered_map<string, int>& map){
    string country = "";
    string userIdStr = "";
    string reply = "";
    
    int len = 0;

    int index = 0;
    while (recv_buf[index] != ' '){
        country += recv_buf[index];
        index++;
    }

    index++;
    while (recv_buf[index] != ' '){
        userIdStr += recv_buf[index];
        index++;
    }

    int userId = stoi(userIdStr);

    cout<< "The Main server has received the request on User " << userId << " in " <<country
        << " from client using TCP over port " << TCP_PORT << endl;

    if (map.find(country) == map.end()){
        reply = "Country " + country + " not found";
        reply.copy(send_buf,reply.length(),0);
        cout << country << " does not show up in server A&B" << endl;
        cout << "The Main Server has sent \"Country Name: Not found\" to client"
             <<  " using TCP over port "<< TCP_PORT << endl;
        return reply.length();
    }

    int serverId = map.at(country);

    if (serverId == 0){
        cout << country << " shows up in server A" <<endl;
        cout << "The Main Server has sent request from User "<< userIdStr
             <<" to server A using UDP over port " << UDP_PORT <<endl;

        len = getInfoFromA(country, userId, send_buf);

    }else {
        cout << country << " shows up in server B" <<endl;

        cout << "The Main Server has sent request from User "<< userIdStr
             <<" to server B using UDP over port " << UDP_PORT <<endl;

        len = getInfoFromB(country, userId, send_buf);

    }

    return len;
}

int TCP(unordered_map<string, int>& map){
    
    // socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        cout << "Error: socket" << endl;
        return 0;
    }

    pid_t childpid;

    // set socket and bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        cout << "Error: bind" << endl;
        return 0;
    }

    // listen
    if(listen(listenfd, 5) == -1) {
        cout << "Error: listen" << endl;
        return 0;
    }

    // accept
    int conn;
    char clientIP[INET_ADDRSTRLEN] = "";
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    while (true) {
        cout << "...listening" << endl;
        conn = accept(listenfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (conn < 0) {
            cout << "Error: accept" << endl;
            continue;
        }
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        cout << "...connect " << clientIP << ":" << ntohs(clientAddr.sin_port) << endl;

        if ((childpid = fork()) == 0){
            close(listenfd);

            char recv_buf[255];
            char send_buf[255];
            while (true) {
                memset(recv_buf, 0, sizeof(recv_buf));
                memset(send_buf, 0, sizeof(send_buf));

                int len = recv(conn, recv_buf, sizeof(recv_buf), 0);
                recv_buf[len] = '\0';
                if (strcmp(recv_buf, "exit") == 0) {
                    cout << "...disconnect " << clientIP << ":" << ntohs(clientAddr.sin_port) << endl;
                    break;
                }

                int send_len = processTCP(recv_buf, send_buf,map);

                cout << "Sending \""<< send_buf <<"\" to client" << endl;

                send(conn, send_buf, send_len, 0);


                if (send_buf[0] == 'U'){
                    cout << "The Main Server has sent Error to client using TCP over "<< UDP_PORT << endl;
                }else {
                    cout<< "The Main Server has sent searching result(s) to client using TCP over port " << TCP_PORT <<endl;
                }
            }
            close(conn);
            break;
        }

    }
    close(listenfd);
    return 0;
}

int main()
{
    unordered_map<string, int> country_backend_mapping;
    cout<< "The Main server is up and running." << endl;

    getCountryListFromA(country_backend_mapping);
    cout<< "The Main server has received the country list from serverA using UDP over port " << UDP_PORT <<endl;
    getCountryListFromB(country_backend_mapping);
    cout<< "The Main server has received the country list from serverB using UDP over port " << UDP_PORT <<endl;

    cout<< "Countries in the ServerA are: " << endl;
    for (auto& x: country_backend_mapping) {
        if (x.second == 0) {
            cout << x.first << endl;
        }
    }
    cout << endl;

    cout<< "Countries in the ServerB are: " << endl;
    for (auto& x: country_backend_mapping) {
        if (x.second == 1) {
            cout << x.first << endl;
        }
    }
    cout << endl;

    TCP(country_backend_mapping);

    return 0;
}