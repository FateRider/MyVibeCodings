#include <iostream>
#include <cstdlib>
#include <ctime>

// Generates a random number string
std::string randNum(int length) {
    std::string result = "";
    for (int i = 0; i < length; ++i) {
        char digit = '0' + std::rand() % 10;
        result += digit;
    }
    return result;
}

// Generates a random letter string (uppercase and lowercase)
std::string randChar(int length) {
    std::string result = "";
    for (int i = 0; i < length; ++i) {
        int choice = std::rand() % 2; // 0 = lowercase, 1 = uppercase
        char letter = (choice == 0) ? ('a' + std::rand() % 26) : ('A' + std::rand() % 26);
        result += letter;
    }
    return result;
}

// Generates a mixed string with numbers and letters
std::string randMix(int length) {
    std::string result = "";
    for (int i = 0; i < length; ++i) {
        int choice = std::rand() % 3; // 0 = digit, 1 = lowercase, 2 = uppercase
        char ch;
        if (choice == 0)
            ch = '0' + std::rand() % 10;
        else if (choice == 1)
            ch = 'a' + std::rand() % 26;
        else
            ch = 'A' + std::rand() % 26;
        result += ch;
    }
    return result;
}

int main() {
    std::srand(std::time(0)); // Seed for randomness

    int len;
    std::cout << "Enter length: ";
    std::cin >> len;

    std::cout << "Random numbers: " << randNum(len) << std::endl;
    std::cout << "Random letters: " << randChar(len) << std::endl;
    std::cout << "Random mixed:   " << randMix(len) << std::endl;

    return 0;
}
