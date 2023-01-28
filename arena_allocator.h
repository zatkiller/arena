#pragma once

using namespace std;

/*
* Arena allocator implementation. Buffer allocated at compile time and freed when Arena destructor
* is called.
*/
template <size_t N>
class Arena {
public:
    static constexpr size_t alignment = alignof(max_align_t);

    Arena() : ptr_(buf_) {}

    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    void reset () {
        ptr_ = buf_;
    }

    static constexpr size_t size() { 
        return N;
    }

    size_t used() const {
        return static_cast<size_t>(ptr_ - buf_);
    }

    byte* allocate(size_t n) {
        auto aligned_n = round_alignment(n);
        auto rem_bytes = static_cast<const size_t>(buf_ + N - ptr_);
        if (rem_bytes >= aligned_n) {
            auto r = ptr_;
            ptr_ += aligned_n;
            return r;
        }

        return static_cast<byte*>(::operator new(n));
    }

    void deallocate(std::byte* p, size_t n) {
        if (within_bounds(p)) { 
            n = round_alignment(n);
            if (p + n == ptr_)
                ptr_ = p;
        } else {
            ::operator delete(p);
        }
    }

private:
    static size_t round_alignment(size_t n) { 
        return (n + (alignment-1)) & ~(alignment-1); 
    }

    bool within_bounds(const byte* p) const {
        return uintptr_t(p) >= uintptr_t(buf_) && uintptr_t(p) <= uintptr_t(buf_) + N; 
    }

    alignas(alignment) byte buf_[N];
    byte* ptr_{};
};
