/*
TCP Source code from: Socket Programming Reference - (Beej's-Guide)
                      "https://www.jianshu.com/p/3b233facd6bb"
*/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 33553
#define IP_ADDRESS  "127.0.0.1"


using namespace std;

bool validateCountry(string input){
    for (int i = 0; i < input.length(); i++) {
        if (!((input[i]>='a' && input[i] <='z') || (input[i]>='A' && input[i] <='Z'))){
            return false;
        }
    }
    return true;
}

bool validateID(string input){
    for (int i = 0; i < input.length(); i++) {
        if (!(input[i]>='0' && input[i] <='9') ){
            return false;
        }
    }
    return true;
}

int main() {
    cout << "Client is up and running!" << endl;

    // build socket
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1) {
        cout << "Error: socket" << endl;
        return 0;
    }


    // make connection
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    if (connect(client, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Error: connect" << endl;
        return 0;
    }
    cout << "...connect" << endl;
    cout << endl;


    char data[255];
    char buf[255];
    while (true) {
        memset(buf, 0, sizeof(buf));
        memset(data, 0, sizeof(data));

        string country = "";
        string userId = "";
        bool isValidCountry = false;
        while (!isValidCountry){
            cout << "Please enter Country Name: ";
            cin >> country;

            isValidCountry = validateCountry(country);
            if (!isValidCountry){
                cout<< "Country Name is not valid. Please Re-enter..." << endl;
            }
        }


        if (country == "exit"){
            userId.copy(data,userId.length(),0);
            send(client, data, strlen(data), 0);
            cout << "...disconnect" << endl;
            break;
        }

        bool isValidId = false;
        while (!isValidId){
            cout << "Please enter the User ID: ";
            cin >> userId;

            isValidId = validateID(userId);
            if (!isValidId){
                cout<< "User ID is not valid. Please Re-enter..." << endl;
            }
        }


        string str = country+ " " + userId +" ";
        str.copy(data,str.length(),0);
        data[str.length()] = '\0';

        // send user request
        send(client, data, strlen(data), 0);
        cout<<"Client has sent User"<< userId << " and "<< country <<" to Main Server using TCP" <<endl;


        int len = recv(client, buf, sizeof(buf), 0);

        buf[len] = '\0';

        if(buf[0] == 'C' || buf[0] == 'U'){
            cout << buf << endl;
        }else if (buf[0] == 'N'){
            cout << buf << " because User " << userId << " has connected with all others in this country" <<endl;
        }else {
            cout<< buf << " to user "<< userId << endl;
        }
        cout << endl;
        cout << "-----Start a new request-----" << endl;
        cout << endl;

    }
    close(client);
    return 0;
}
