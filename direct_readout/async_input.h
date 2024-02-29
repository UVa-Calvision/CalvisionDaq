#pragma once

#include <thread>
#include <iostream>

struct listen_for_key {
    listen_for_key(char key)
        : hit_(false)
    {
        listener_ = std::thread([this, key] () {
                    while (!hit_) {
                        if (std::cin.get() == key) {
                            hit_ = true;
                        }
                    }
                });
    }

    ~listen_for_key() {
        listener_.join();
    }

    bool hit() const { return hit_; }

private:
    std::thread listener_;
    volatile bool hit_;
};
