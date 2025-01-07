#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h> 

#include <unistd.h>

#include <signal.h>
#include <vector>
#include <iostream>

using namespace std;

volatile sig_atomic_t wasSigHup = 0;

void sigHupHandler(int r)
{
    wasSigHup = 1;
}

class socketConnection{

public:
    socketConnection(){
    
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0); //создание сокета

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; 
    server_address.sin_port = htons(9734);

    bind(server_sockfd, (struct sockaddr *)&server_address, sizeof(server_address)); //связывание сокета

    listen(server_sockfd, 5); //создание очереди на прослушивание

    std::cout << "Server waiting...\n";

    }
    ~socketConnection(){ 
        closeConnection();
    }


    void reconnect();
    void getMsg();
    void closeConnection();

private:
    int server_sockfd, client_sockfd;
    socklen_t client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int msg;
    vector<int> clients;

};

void socketConnection::getMsg(){

    struct sigaction sa;
    sigaction(SIGHUP, NULL, &sa);
    sa.sa_handler = sigHupHandler;
    sa.sa_flags |= SA_RESTART;
    sigaction(SIGHUP, &sa, NULL);

    while(1){

        fd_set fds;
        FD_ZERO(&fds);

        FD_SET(server_sockfd, &fds);

        for(clientIt = clients.begin(); clientIt != clients.end(); clientIt++) 
            FD_SET(*clientIt, &fds);
        if (pselect(maxFd + 1, &fds, NULL, NULL, NULL, origSigMask) == -1)
            if (errno == EINTR){
                if(wasSigHup == 1) {
                    cout << "Recieved SIGHUP signal\n";
                    wasSigHup = 0;
                }
                continue;
            }
            else {
                perror("pselect error");
                break;
                }

        if (FD_ISSET(server_sockfd, &fds)){
            client_len = sizeof(client_address);
            client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
            clients.push_back(client_sockfd);
            client_sockfd = -1;

            continue;
        }

        for(auto clientIt = clients.begin(); clientIt != clients.end(); ++clientIt){

            if (FD_ISSET(*clientIt, &fds)){
                recv(*clientIt, &msg, sizeof(msg), 0)
                close(*clientIt);
                clintIt = cleints.erase(clientIt);

            }
        }
}
}

void socketConnection::closeConnection(){//закрытия сокетов

    close(client_sockfd);
    for(auto clientIt = clients.begin(); clientIt != clients.end(); ++clientIt){
        close(*clientIt);
    }
    std::cout << "Server closed connection\n";
    
}

int main(){
    socketConnection sockSer;
    return 0;
}