#include "Train.h"
#include "Terminal.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

Train trains[3] = {{'A', 0}, {'B', 0}, {'C', 0}};

std::mutex parking;

void Station(int t, Terminal* terminal) { 
    for (int i = 0; i < trains[t].timeSec && !trains[t].arrived; ++i) {
        
        if (!trains[t].cout) std::this_thread::sleep_for(std::chrono::seconds(0));
        else std::this_thread::sleep_for(std::chrono::seconds(1));
        
        if (i == trains[t].timeSec - 1 && terminal->freeParkingSpace) {
            
            terminal->freeParkingSpace = false;
            
            parking.lock();
            std::cout << "Train " << trains[t].name << " arrived at the station. Waiting for command input \"depart\"..\n";
            trains[t].arrived = true;
            parking.unlock();

            std::string depart;
            do {
                std::cin >> depart;
                if (depart != "depart") std::cout << "Invalid, enter the command \"depart\".\n";
            } while (depart != "depart");
            
            parking.lock();
            std::cout << "Train " << trains[t].name << " left the station.\n";
            terminal->freeParkingSpace = true;
            trains[t].cout = false;
            parking.unlock();

        } else if (i == trains[t].timeSec - 1 && trains[t].cout ) {
            
            parking.lock();
            std::cout << "Train " << trains[t].name << " waiting for free space.\n";
            trains[t].cout = false;
            parking.unlock();
        }
    } 
}

int main() { 
    std::cout << "Enter the travel time to the station (in seconds).\n";
    for (int i = 0; i < sizeof(trains) / sizeof(trains[0]); ++i) {
        std::cout << i + 1 << " Train " << trains[i].name << " : ";
        std::cin >> trains[i].timeSec;
    }                                                                
    Terminal terminal;
    std::cout << "\nStart!\n\n";

    int count = sizeof(trains) / sizeof(trains[0]);
    do {
        std::thread moveA(Station, 0, &terminal);
        std::thread moveB(Station, 1, &terminal);
        std::thread moveC(Station, 2, &terminal);


        moveA.join();
        moveB.join();
        moveC.join();

        count--;
    } while (count != 0);
}