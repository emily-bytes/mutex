#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

#define MAX_TURNS 2
#define RESET_VAL 4

/* Resources shared by threads */
mutex mut;
condition_variable cv;
int allowed_ID = 1;      // shared counter
bool isComplete{false};  // flag

/* Modifying thread; safe-incrementing thread */
void increment(int ID) {
    unique_lock<std::mutex> lk(mut);     // Acquire lock
    cout << "Thread " << ID << "'s turn!\n";
    allowed_ID++;   // Modify the data
    if (allowed_ID == RESET_VAL) {
        allowed_ID = 1;
    }
    isComplete = true;
    cv.notify_one();    // Notify condition variable
}

/* Waiting thread */
void read(int ID) {
    if (allowed_ID != ID) {
        std::this_thread::sleep_for(1s);
        unique_lock<std::mutex> lk(mut);     // Acquire lock
        cv.wait(lk, [] { return isComplete; });    // Lambda predicate
        cout << "Not thread " << ID << "'s turn!\n";
    }
}

/* Thread takes turn */
void takeTurn(int ID) {
    for (int turns = 0; turns < MAX_TURNS; turns++) {
        read(ID);
        increment(ID);
    }
    cout << "Thread " << ID << " complete!\n";
}

int main() {
    /* Assign IDs */
    int ID1 = 1, ID2 = 2, ID3 = 3;

    /* Create three threads and pass ID as parameters */
    thread thread1{takeTurn, ref(ID1)};
    thread thread2{takeTurn, ref(ID2)};
    thread thread3{takeTurn, ref(ID3)};

    /* Wait for threads to finish */
    thread1.join();
    thread2.join();
    thread3.join();

    return 0;
}

