#include <thread>
#include <chrono>
#include <iostream>
#include <future>
#include <cassert>

void WaitFor(const std::chrono::seconds t)
{
    std::this_thread::sleep_for(t);
    std::cout << "Done waiting!\n";
}

void WaitingTest(const unsigned int occupiedThreads)
{
    // Define optimal number of threads such as: nbr of hardware threads - threads used by program already.
    const unsigned int optimalNbrOfThreads = std::thread::hardware_concurrency() - occupiedThreads;
    assert(optimalNbrOfThreads > 0);
    std::cout << "Optimal number of threads is: " << optimalNbrOfThreads << '\n';

    // Initialize threads.
    std::thread threads[optimalNbrOfThreads];
    for (int i = 0; i < optimalNbrOfThreads; ++i)
    {
        threads[i] = std::thread(WaitFor, std::chrono::seconds(1));
    }
    std::cout << "Launched threads!\n";

    // Simulate main thread activity.
    std::cout << "Starting own wait...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Main thread woken up!\n";

    // Finally join the threads once main thread is done.
    for (int i = 0; i < optimalNbrOfThreads; ++i)
    {
        threads[i].join();
    }
}

void PromisedSum(std::promise<int> &&promise, const int a, const int b)
{
    std::cout << "New thread started sleeping\n";
    std::this_thread::sleep_for(std::chrono::seconds(rand() % 5)); // Sleep for a random amount of seconds up to 5.
    std::cout << "New thread done sleeping\n";
    promise.set_value(a + b);
}

bool inline IsReady(const std::future<int> &future)
{
    auto status = future.wait_for(std::chrono::milliseconds(0));
    return status == std::future_status::ready;
}

void PromisedSumTest(const unsigned int occupiedThreads)
{
    const unsigned int optimalNbrOfThreads = std::thread::hardware_concurrency() - occupiedThreads;
    assert(optimalNbrOfThreads > 0);
    std::cout << "Optimal number of threads is: " << optimalNbrOfThreads << '\n';
    std::thread threads[optimalNbrOfThreads];

    std::promise<int> promises[optimalNbrOfThreads];
    std::future<int> futures[optimalNbrOfThreads];
    for (int i = 0; i < optimalNbrOfThreads; ++i)
    {
        futures[i] = promises[i].get_future();
    }

    int a[optimalNbrOfThreads];
    int b[optimalNbrOfThreads];
    for (int i = 0; i < optimalNbrOfThreads; ++i)
    {
        a[i] = rand() % 100;
        b[i] = rand() % 100;
    }

    for (int i = 0; i < optimalNbrOfThreads; ++i)
    {
        threads[i] = std::thread(PromisedSum, std::move(promises[i]), a[i], b[i]);
    }

    unsigned int iterations = 0;
    bool isReady[3]{false, false, false};
    while (!isReady[0] || !isReady[1] || !isReady[2])
    {
        for (int i = 0; i < optimalNbrOfThreads; ++i)
        {
            iterations++;
            if (IsReady(futures[i]) == true)
            {
                if (!isReady[i])
                {
                    isReady[i] = true;
                    std::cout << "A thread is ready!\n";
                }
            }
        }
    }

    std::cout << "All threads are ready! Main thread waited through: " << iterations << " iterations.\n";

    for (int i = 0; i < optimalNbrOfThreads; ++i)
    {
        std::cout << "Sum of " << a[i] << " and " << b[i] << " is: " << futures[i].get() << '\n';
    }

    for (int i = 0; i < optimalNbrOfThreads; ++i)
    {
        threads[i].join();
    }
    std::cout << "Joined all threads!\n";
}

int main()
{
    const unsigned int NBR_OCCUPIED_THREADS = 1; // One main thread running (this one).

    WaitingTest(NBR_OCCUPIED_THREADS); // Simple multithreaded waiting test.
    std::cout << "\n\n\n";
    PromisedSumTest(NBR_OCCUPIED_THREADS); // Simple multithreading with promises.

    return 0;
}
