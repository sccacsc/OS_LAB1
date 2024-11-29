#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>

std::mutex mt;
std::condition_variable cv;
std::queue<int> v;

void producer(){

    int prodVal = 0;

    while(true){
        //с задержкой в одну секунду 
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ++prodVal;
        std::cout << "Produced: " << prodVal << std::endl;

        std::unique_lock<std::mutex> lck(mt);
        v.push(prodVal);
        cv.notify_all();

    }
}

void consumer(){
    while(true){

        int consVal;

        std::unique_lock<std::mutex> lck(mt);
        cv.wait(lck, []{return !v.empty(); });
        consVal = v.front(); v.pop();

        std::cout << "Consumed: " << consVal << std::endl;

        cv.notify_all();
    }

}

int main(){
    std::thread t1(consumer);
    std::thread t2(producer);

    t1.join(); t2.join();

    return 0;
}