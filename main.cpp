#include <iostream>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

struct Pack {
     int i{0};
    double d{0.0};
    friend std::ostream& operator<<(std::ostream& os, const Pack& pack) {
        return os << "("<< pack.i << "," << pack.d << ")" <<'\n';
    }
};

template <typename T>
void print(T& dict) {
    for (const auto& [id, pack] : dict) {
        std::cout << id << " -> " << *pack;
    }
}

struct Data {
    Data() {
        std::cout << "Default c'tor : " << (void*)(this) << '\n';
    }
    ~Data() {
        //std::cout << "Default destructor: " << (void*)(this) << '\n';
    }
    Data(const Data&) {
        std::cout << "Copy c'tor: " << (void*)(this) << '\n';
    }
    Data(Data&&) noexcept{
        std::cout << "Move c'tor: " << (void*)(this) << '\n';
    }
};

void taskLRef(Data&) {
    std::cout << "Task getting Data& is being executed!\n";
}

void taskCLRef(const Data&) {
    std::cout << "Task getting const Data& is being executed!\n";
}

void taskRRef(Data&&) {
    std::cout << "Task getting Data&& is being executed!\n";
}

struct PDU {
    int* _integer;
    std::vector<double> _doubles;
    friend std::ostream& operator<<(std::ostream& os, const PDU& thiz) {
        return os << "At "<< &thiz << ", int : " << *(thiz._integer) << ", size of dynamic container : " << thiz._doubles.size() << " ";
    }
    PDU() {
        //std::cout << *this << "will be default constructed!\n";
        std::cout << this << " will be default constructed!\n";
    }
    ~PDU() noexcept {
        //std::cout << *this << "will be destructing!\n";
        std::cout << this << " will be destructing!\n";
        delete _integer;
    }
    PDU(PDU&& rhs) {
        //std::cout << this << " will be move constructed!\n";
        _integer = std::move(rhs._integer);
        _doubles = std::move(rhs._doubles);
        rhs._integer = nullptr;
        //std::cout << *this << "is move constructed!\n";
    }
};

std::vector<int*> sharedPdu{};

void printlines(int id) {
    for (int i = 0; i < id; ++i) {
        std::cout << "-";
    }
    std::cout << "Thread id " << id << " => ";
}

void process_pdu(PDU&& pdu, int id) {
    // move construction of incoming PDU
    PDU moved = std::move(pdu);
    printlines(id); std::cout << moved << '\n';
    printlines(id); std::cout << "Move constructed at " << (void*)(&moved) << " having " << *moved ._integer << " will be pushed to queue!" << '\n';
    sharedPdu.push_back(moved._integer);
    printlines(id);
}

void produce_pdu(int id) {
    printlines(id);
    std::unique_ptr<PDU> newPdu = std::make_unique<PDU>();
    newPdu->_integer = new int(id);
    process_pdu(std::move(*newPdu), id);
    printlines(id);
}

int main() {
    /*
    Data d;
    taskLRef(d);
    {
        std::thread threadLRef2{taskLRef, std::ref(d)};
        threadLRef2.detach();
    }
    std::cout << "\n\n\n";

    taskCLRef(d);
    {
        std::thread threadCLRef1{taskCLRef, d};
        threadCLRef1.detach();
    }

    {
        std::thread threadCLRef2{taskCLRef, std::ref(d)};
        threadCLRef2.detach();
    }

    {
        std::thread threadCLRef3{taskCLRef, Data{}};
        threadCLRef3.detach();
    }


    std::cout << "\n\n\n";

    taskRRef(Data{});

    {
        std::thread thr{taskRRef, Data{}};
        thr.detach();
    }

    {
        std::thread thr{taskRRef, d};
        thr.detach();
    }
    */

    /*
    std::unordered_map<int, Pack*> dict {
            {1, new Pack{.i = 10, .d = 10.10}},
            {2, new Pack{.i = 20, .d = 20.20}},
    };

    std::mutex mtx;

    for (uint32_t i = 0; i < 1000 ; ++i) {
        std::thread t1([&dict, &mtx](){
            auto it = dict.find(1);

            std::unique_lock lock(mtx);

            //std::atomic<Pack*> packPtr = it->second;
            //std::this_thread::sleep_for(std::chrono::nanoseconds (5));
            //packPtr.load()->i += 1;
            //std::this_thread::sleep_for(std::chrono::nanoseconds (5));
            //packPtr.load()->d += 2.0;

            std::this_thread::sleep_for(std::chrono::nanoseconds(2));
            it->second->i += 1;
            std::this_thread::sleep_for(std::chrono::nanoseconds(2));
            std::cout << *it->second;
            std::this_thread::sleep_for(std::chrono::nanoseconds(2));
            it->second->d += 2.0;
        });

        std::thread t2([&dict, &mtx](){
            auto it = dict.find(1);

            std::unique_lock lock(mtx);


            //auto& [_, pack] = *it;
            //pack->i += 2;
            //std::this_thread::sleep_for(std::chrono::nanoseconds (10));
            //pack->d += 4.0;
            //std::cout << *pack;

            //std::atomic<Pack*> packPtr = it->second;
            //std::this_thread::sleep_for(std::chrono::nanoseconds (5));
            //packPtr.load()->i += 2;
            //std::this_thread::sleep_for(std::chrono::nanoseconds (5));
            //packPtr.load()->d += 4.0;

            std::this_thread::sleep_for(std::chrono::nanoseconds(2));
            it->second->i += 2;
            std::this_thread::sleep_for(std::chrono::nanoseconds(2));
            std::cout << *it->second;
            std::this_thread::sleep_for(std::chrono::nanoseconds(2));
            it->second->d += 4.0;
        });

        t1.join();
        t2.join();
    }

    print(dict);
    */

    std::cout << std::string(100, '_') << '\n';
    {
        for (int t = 1; t < 5 ; ++t) {
            std::jthread th {produce_pdu, t};
        }
    }

    for (const auto& s : sharedPdu) {
        std::cout << *s << '\n';
    }

    return 0;
}
