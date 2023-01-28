#include <iostream>
#include <cassert>
#include <memory>

#include "arena_allocator.h"

using namespace std;

#define SMALL_ARENA_SIZE 1024
#define MEDIUM_ARENA_SIZE 2048
#define LARGE_ARENA_SIZE 4096

auto user_arena = Arena<SMALL_ARENA_SIZE>{};

struct User {

    User() = default;
    User(uint64_t id) : id_(id) {}
    void* operator new(size_t size) {
        return user_arena.allocate(size);
    }

    void operator delete(void* p) {
        return user_arena.deallocate(static_cast<byte*>(p), sizeof(User));
    }

    void* operator new[](size_t size) {
        return user_arena.allocate(size);
    }
    
    void operator delete[](void* p, size_t size) {
        user_arena.deallocate(static_cast<byte*>(p), size);
    }

private:
    uint64_t id_{};
};

int main() {
    auto user1 = new User{1};
    delete user1;

    auto users = new User[10];
    delete [] users;

    auto user2 = make_unique<User>(2);

    return 0;
}