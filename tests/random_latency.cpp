#include <iostream>
#include <random>
#include <cstdlib>
#include <ctime>
#include <chrono>

// Function to test std::mt19937 (Mersenne Twister)
void test_mt19937(int num_samples) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 312);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_samples; ++i) {
        dis(gen);  // Generate a random number
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "std::mt19937 Time: " << duration.count() * 1000 << " ms\n";  // Milliseconds
}

// Function to test rand()
void test_rand(int num_samples) {
    std::srand(static_cast<unsigned>(std::time(0)));

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_samples; ++i) {
        std::rand() % 100 + 1;  // Generate a random number
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "rand() Time: " << duration.count() * 1000 << " ms\n";  // Milliseconds
}

int main() {
    int num_samples = 100000000;  // Number of random numbers to generate

    // Test latency for std::mt19937
    test_mt19937(num_samples);

    // Test latency for rand()
    test_rand(num_samples);

    return 0;
}

/* notes
 * rand() is faster, using a lower quality LCG/PRNG, shorter period, likely between [2^15,2^32]
 * the Mersenne twister provides higher quality randomness at the cost of some performance overhead, period=2^19937−1
 * for 10^8 iterations: rand=563.54 ms, mt19937=1848.73 ms
 *
 * LCG formula - X_{n+1} = (a * X_n + c) % m
 * algorithm - multiply by a, add c, modulo m
 * fast, simple, easy to implement, but deterministic and can be predictable if poor quality parameters are chosen
 *
 * A high-quality RNG produces numbers that are statistically indistinguishable from true randomness
 *
 * TODO: watch video: https://www.youtube.com/watch?v=LDPMpc-ENqY
*/