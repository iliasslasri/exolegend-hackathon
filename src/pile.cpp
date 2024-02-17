#include <iostream>
#include <vector>
#include "decouverte.h"



// Define the Stack class
class Stack {
private:
    std::vector<MazeSquare> data;

public:
    void push(const MazeSquare& square) {
        data.push_back(square);
    }

    MazeSquare pop() {
        if (data.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        MazeSquare square = data.back();
        data.pop_back();
        return square;
    }

    bool isEmpty() const {
        return data.empty();
    }
};


