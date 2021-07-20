#include <iostream>
#include "skiplist.h"
#define FILE_PATH "./data/dump_file"

int main() {

    SkipList<std::string, std::string> skipList;
	skipList.insert("3", "b"); 
	skipList.insert("7", "c"); 
	skipList.insert("8", "sun"); 
	skipList.insert("9", "xiu"); 
	skipList.insert("19", "yang"); 
	skipList.insert("19", "yang"); 
	skipList.insert("19", "yanuuuuuug"); 
	skipList.insert("1", "a"); 

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.dump_file();

    // skipList.load_file();

    skipList.search("9");
    skipList.search("18");


    skipList.display_list();

    skipList.remove("3");
    skipList.remove("7");

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.display_list();

    SkipList<int, std::string> skipList2;
    for (int i=0; i<10; i++) {
		skipList2.insert(rand() % 10, "a"); 
        std::cout << "count: " << i << std::endl;
	}
    skipList2.display_list();
    std::cout << "skipList size:" << skipList2.size() << std::endl;
}