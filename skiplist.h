#include <iostream>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <shared_mutex>
#include <fstream>

#define SKIPLIST_MAXLEVEL 20
#define SKIPLIST_P 0.5 /* Skiplist P = 1/4 */
#define STORE_FILE "data/dump_file"

std::string delimiter = ":";

template<typename K, typename V>
class Node {
public:
    Node() {}
    Node(K k, V v, int);
    ~Node();
    K get_key() const;
    V get_value() const;
    void set_value(V);

    // node tower
    Node<K, V> **forward;
    int node_level;

private:
    K key;
    V value;
};

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level) : key(k), value(v), 
                                                    node_level(level) {
    // tower index from 0 - level
    this->forward = new Node<K, V>*[level+1];
    // fill tower with 0(NULL)
    memset(this->forward, 0, sizeof(Node<K, V>*)*(level + 1));   
}

template<typename K, typename V>
Node<K, V>::~Node() {
    delete []forward;
}

template<typename K, typename V>
K Node<K, V>::get_key() const {
    return key;
}

template<typename K, typename V>
V Node<K, V>::get_value() const {
    return value;
}

template<typename K, typename V>
void Node<K, V>::set_value(V value) {
    this->value = value;
}


template<typename K, typename V>
class SkipList {
public:
    SkipList();
    ~SkipList();
    int get_random_level();
    Node<K, V>* create_node(K, V, int);
    bool insert(K, V);
    bool search(K);
    void remove(K);
    void display_list();
    void dump_file();
    void load_file();
    int size();

private:
    void get_key_value_from_line(const std::string& str, std::string* key, std::string* value);
    bool is_valid_line(const std::string& str);

private:
    // current level
    int _skip_list_level;
    // head node
    Node<K, V> *_head;
    // file iostream
    std::ofstream _file_writer;
    std::ifstream _file_reader;
    // element counts
    int _element_count;
    // mutex for skip list
    std::mutex mutex_;
};

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(K key, V value, int level) {
    std::lock_guard<std::mutex> l(mutex_);
    Node<K, V>* node = new Node<K, V>(key, value, level);
    return node;
}

template<typename K, typename V>
SkipList<K, V>::SkipList() {
    this->_skip_list_level = 0;
    this->_element_count = 0;
    // create head node
    K k;
    V v;
    this->_head = new Node<K, V>(k, v, SKIPLIST_MAXLEVEL);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList() {
    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete _head;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level() {
    int level = 1;
    while ((random() & 0xFFFF) < (SKIPLIST_P * 0xFFFF)) {
        level += 1;
    }
    level = (level < SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
    return level;
}

template<typename K, typename V>
bool SkipList<K, V>::insert(K key, V value) {
    std::lock_guard<std::mutex> l(mutex_);
    Node<K, V> *current = this->_head;

    // create array for node->forward[i] tower
    Node<K, V> *update[SKIPLIST_MAXLEVEL+1];
    memset(update, 0, sizeof(Node<K, V>*)*(SKIPLIST_MAXLEVEL+1));

    for (int i = _skip_list_level; i >= 0; --i) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // reached level 0 and forward
    current = current->forward[0];
    if (current != nullptr && current->get_key() == key) {
        // std::cout << "key: " << key << ", exists!" << std::endl;
        return false;
    }
    if (current == nullptr || current->get_key() != key) {
        int random_level = get_random_level();
        // If random level is greater thar skip list's current level, initialize update value with pointer to header
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level+1; i <= random_level; ++i) {
                update[i] = _head;
            }
            _skip_list_level = random_level;
        }

        Node<K, V>* insert_node = create_node(key, value, random_level);
        // insert node
        for (int i = random_level; i >= 0; --i) {
            insert_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = insert_node;
        }
        // std::cout << "Successful inserted key: " << key << ", value: " << value << std::endl;
        ++_element_count;
    }
    return true;
}

template<typename K, typename V>
bool SkipList<K, V>::search(K key) {
    Node<K, V> *current = this->_head;
    // from top to bottom
    for (int i = _skip_list_level; i >= 0; --i) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        if (current->forward[i] != nullptr && current->forward[i]->get_key() == key) {
            // std::cout << "Found key: " << key << ", value: " << current->forward[i]->get_value() << std::endl;
            return true;
        }
    }
    // std::cout << "Not Found key: " << key << std::endl;
    return false;
}

template<typename K, typename V>
void SkipList<K, V>::remove(K key) {
    std::lock_guard<std::mutex> l(mutex_);
    Node<K, V> *current = this->_head;
    Node<K, V> *update[SKIPLIST_MAXLEVEL+1];
    memset(update, 0, sizeof(Node<K, V>*)*(SKIPLIST_MAXLEVEL+1));

    for (int i = _skip_list_level; i >= 0; --i) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != nullptr && current->get_key() == key) {
        for (int i = current->node_level; i >= 0; --i) {
            update[i]->forward[i] = current->forward[i];
        }
        delete current;
        --_element_count;
        while (_skip_list_level > 0 && _head->forward[_skip_list_level] == nullptr) {
            _skip_list_level--;
        }
        std::cout << "Success delete key: " << key << std::endl;
    }
    std::cout << "Not exists key: " << key << std::endl;
    return;
}

template<typename K, typename V>
void SkipList<K, V>::display_list() {
    std::cout << "\n********** SkipList ********" << std::endl;
    for (int i = _skip_list_level; i >= 0; --i) {
        Node<K, V> *node = this->_head->forward[i];
        std::cout << "Level " << i << " : ";
        while (node != nullptr) {
            std::cout << "{ " << node->get_key() << ":" << node->get_value() << " }" << " --> ";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

template<typename K, typename V>
void SkipList<K, V>::dump_file() {
    std::cout << "\n********** dump file **********" << std::endl;
    _file_writer.open(STORE_FILE);

    Node<K, V> *node = this->_head->forward[0];
    while (node != nullptr) {
        _file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << std::endl;
        node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    return;
}

template<typename K, typename V>
void SkipList<K, V>::load_file() {
    _file_reader.open(STORE_FILE);
    std::cout << "\n********** load file **********" << std::endl;
    std::string line;
    std::string *key = new std::string();
    std::string *value = new std::string();
    while (getline(_file_reader, line)) {
        get_key_value_from_line(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert(*key, *value);
        std::cout << "key: " << *key << " value: " << *value << std::endl;
    }
    _file_reader.close();
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_line(const std::string& str, std::string* key, std::string* value) {
    if (!is_valid_line(str)) {
        std::cout << "load failed..." << std::endl;
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_line(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}

template<typename K, typename V>
int SkipList<K, V>::size() {
    return _element_count;
}