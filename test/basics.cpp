#include <iostream>

#include <ring.hpp>

std::size_t data_counter_ = 0;
std::size_t ctor_counter_ = 0;
std::size_t dtor_counter_ = 0;

class obj
{
private:
    std::size_t data_;

public:
    obj(std::size_t data) :
        data_{ data }
    {
        ++ctor_counter_;
    }
    obj(obj&& o) :
        data_{ std::move(o.data_) }
    {
        ++ctor_counter_;
    }
    ~obj()
    {
        ++dtor_counter_;
        if (dtor_counter_ > ctor_counter_)
        {
            throw std::runtime_error("More destructor than constructor");
        }
    }
    std::size_t data() const
    {
        return data_;
    }
};

int main()
{
    {
        ring<obj> r;
        if (!r.empty())
        {
            std::cerr << "Default initialized should be empty." << std::endl;
            return -1;
        }
        if (!r.full())
        {
            std::cerr << "Default initialized should be full." << std::endl;
            return -1;
        }
        if (r.capacity() != 0)
        {
            std::cerr << "Default initialized should have zero capacity." <<
                std::endl;
            return -1;
        }
        if (r.size() != 0)
        {
            std::cerr << "Default initialized should have zero size." <<
                std::endl;
            return -1;
        }
        r.reserve(10);
        if (!r.empty())
        {
            std::cerr << "Without push ring should be empty." << std::endl;
            return -1;
        }
        if (r.full())
        {
            std::cerr << "Without push ring shouldn't be full." << std::endl;
            return -1;
        }
        if (r.capacity() != 10)
        {
            std::cerr << "After reserve capacity should be 10." << std::endl;
            return -1;
        }
        if (r.size() != 0)
        {
            std::cerr << "Without push ring should have zero size." <<
                std::endl;
            return -1;
        }
        for (std::size_t i = 0; i < 8; ++i)
        {
            r.push(obj{ data_counter_++ });
        }
        if (r.size() != 8)
        {
            std::cerr << "After 8 push size should be 8." << std::endl;
            return -1;
        }
        r.reserve(16);
        for (std::size_t i = 0; i < 8; ++i)
        {
            r.push(obj{ data_counter_++ });
            std::size_t d;
            d = r.pop().data();
            if (d != i * 2)
            {
                std::cerr << "Data not expected." << std::endl;
                return -1;
            }
            d = r.pop().data();
            if (d != i * 2 + 1)
            {
                std::cerr << "Data not expected." << std::endl;
                return -1;
            }
        }
        if (r.size() != 0)
        {
            std::cerr << "After popping all size should be zero." << std::endl;
            return -1;
        }
    }
    if (ctor_counter_ != dtor_counter_)
    {
        std::cerr << "Counters don't match." << std::endl;
        return -1;
    }
    return 0;
}
