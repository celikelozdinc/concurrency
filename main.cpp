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

int main() {
    Data d;
    taskLRef(d);
    //std::thread threadLRef1{taskLRef, d}; //std::thread arguments must be invocable after conversion to rvalues
        //in instantiation of function template specialization 'std::thread::thread<void (&)(Data &), Data &, void>' requested here
    {
        std::thread threadLRef2{taskLRef, std::ref(d)};
        threadLRef2.detach();
    }
    //std::thread threadLRef3{taskLRef, Data{}};
        //in instantiation of function template specialization 'std::thread::thread<void (&)(Data &), Data, void>' requested here

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

    //std::thread thr{taskRRef, std::ref(d)};//std::thread arguments must be invocable after conversion to rvalues
        //in instantiation of function template specialization 'std::thread::thread<void (&)(Data &&), std::reference_wrapper<Data>, void>' requested here

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


    return 0;
}
