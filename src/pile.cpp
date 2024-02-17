#include <iostream>
#include <vector>
#include <stdexcept> // Include for std::runtime_error
#include "decouverte.h" // Assuming MazeSquare is defined here

// Define the Stack class
class Stack {
private:
    std::vector<MazeSquare> data; // Store MazeSquare objects directly

public:
    // Push a copy of MazeSquare onto the stack
    void push(const MazeSquare& square) {
        data.push_back(square);
    }

    // Pop a MazeSquare from the stack
    MazeSquare pop() {
        if (isEmpty()) {
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
