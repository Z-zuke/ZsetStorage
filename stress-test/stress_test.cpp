#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include "../skiplist.h"

#define TEST_COUNT 1000000
SkipList<int, std::string> skipList;

void insertElement() { 
    int tmp = TEST_COUNT; 
	for (int i=0; i<tmp; i++) {
		skipList.insert(rand() % TEST_COUNT, "a"); 
        // std::cout << "count: " << count << "  skiplist size: " << skipList.size() << std::endl; 
	}
    std::cout << "skiplist size: " << skipList.size() << std::endl; 
}

void getElement() {
    int tmp = TEST_COUNT; 
	for (int i=0; i<tmp; i++) {
		skipList.search(rand() % TEST_COUNT); 
	}
}

int main() {
    srand (time(NULL));  
    {
        auto start = std::chrono::high_resolution_clock::now();
        insertElement();
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    }
    // skipList.display_list();

    {
        auto start = std::chrono::high_resolution_clock::now();
        getElement();
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "search elapsed:" << elapsed.count() << std::endl;
    }

    return 0;

}
