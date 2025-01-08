#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class socketConnection {

public:
    socketConnection(){

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr("127.0.0.1");
        address.sin_port = htons(9734);

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

    }
    ~socketConnection(){
        closeConnection();
    }
    void sendMsg(int msg);
    void closeConnection();

private:
    int sockfd;
    struct sockaddr_in address;
    int msg = 1;
    bool flag = true;
};

void socketConnection::sendMsg(int msg){
    if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0 && flag){
        std::cout << "Server is offline\n";
    } else {
            send(sockfd, &msg, sizeof(msg), 0);
            flag = false;
        }
}

void socketConnection::closeConnection(){
    close(sockfd);
}

int main(){
    socketConnection client;
    int a;
    while(std::cin>>a){
        client.sendMsg(a);
    }

    return 0;
}