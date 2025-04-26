#pragma once

#include <memory>

class Stack {
public:

private:
    size_t _max_size;
    size_t _size;
    std::shared_ptr<Frame> _top;
    std::shared_ptr<Frame> _frames;
};

