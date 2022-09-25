#ifndef SRC_RING_HPP
#define SRC_RING_HPP

#include <memory>

template<class T, class Allocator = std::allocator<T>>
class ring
{
private:
    Allocator allocator;
    std::size_t capacity_;
    T* data_end_;
    T* lower_bond_;
    T* upper_bond_;

public:
    // Create a ring with zero capacity.
    ring() :
        capacity_{ 0 },
        data_end_{ nullptr },
        lower_bond_{ nullptr },
        upper_bond_{ nullptr }
    {
    }

    // Create an empty ring with given capacity.
    // Capacity shouldn't be zero.
    ring(const std::size_t capacity) :
        capacity_{ capacity },
        data_end_{ allocator.allocate(capacity_) + capacity_ },
        lower_bond_{ data_end_ - capacity_},
        upper_bond_{ data_end_ }
    {
    }

    ~ring()
    {
        if (upper_bond_ != data_end_)
        {
            if (lower_bond_ >= upper_bond_)
            {
                while (lower_bond_ != data_end_)
                {
                    lower_bond_++->~T();
                }
                lower_bond_ = data_end_ - capacity_;
            }
            while (lower_bond_ != upper_bond_)
            {
                lower_bond_++->~T();
            }
        }
        allocator.deallocate(data_end_ - capacity_, capacity_);
    }

    // Check if the ring is full
    bool full() const
    {
        return upper_bond_ == lower_bond_;
    }

    // Push an item into the ring.
    // This shouldn't be called when the ring is full.
    template<class TT>
    void push(TT&& item)
    requires std::is_same<T, typename std::decay<TT>::type>::value
    {
        unset_empty();
        new (upper_bond_++) T{ std::forward<TT>(item) };
        if (upper_bond_ == data_end_)
        {
            upper_bond_ = data_end_ - capacity_;
        }
    }

    // push a range of items into the ring.
    // The number of items shouldn't be greater then empty space in the ring.
    void push(const T* begin, const T* end)
    {
        unset_empty();
        while (begin != end)
        {
            new (upper_bond_++) T{ begin++ };
            if (upper_bond_ == data_end_)
            {
                upper_bond_ = data_end_ - capacity_;
            }
        }
    }

    // Check if the ring is empty.
    bool empty() const
    {
        return upper_bond_ == data_end_;
    }

    // Pop an item from the ring
    // This shouldn't be called when the ring is empty.
    T pop()
    {
        T item{ std::move(*lower_bond_) };
        lower_bond_++->~T();
        if (lower_bond_ == data_end_)
        {
            lower_bond_ = data_end_ - capacity_;
        }
        if (lower_bond_ == upper_bond_)
        {
            upper_bond_ = data_end_;
        }
        return item;
    }

    // Get chuncks of contaigous data.
    // This shouldn't be called when the ring is empty.
    std::array<std::pair<T*, T*>, 2> data() const
    {
        if (lower_bond_ > upper_bond_)
        {
            return
            {
                { lower_bond_, data_end_ },
                { data_end_ - capacity_, upper_bond_ },
            };
        }
        else
        {
            return
            {
                { lower_bond_ , upper_bond_ },
                { nullptr, nullptr },
            };
        }
    }

    // Change the capacity of the ring
    void reserve(const std::size_t size)
    {
        if (capacity_ != size)
        {
            T* data = size ? allocator.allocate(size) : nullptr;
            auto data_begin = data_end_ - capacity_;
            if (upper_bond_ != data_end_)
            {
                auto first_upper_bond =
                    lower_bond_ < upper_bond_ ? upper_bond_ : data_end_;
                if (first_upper_bond - lower_bond_ > size)
                {
                    if (first_upper_bond != upper_bond_)
                    {
                        while (upper_bond_ != data_begin)
                        {
                            (--upper_bond_)->~T();
                        }
                    }
                    upper_bond_ = lower_bond_ + size;
                    while (first_upper_bond != upper_bond_)
                    {
                        (--first_upper_bond)->~T();
                    }
                }
                else if (lower_bond_ >= upper_bond_ &&
                    capacity_ - (lower_bond_ - upper_bond_) > size)
                {
                    auto second_upper_bond = lower_bond_ - (capacity_ - size);
                    while (upper_bond_ != second_upper_bond)
                    {
                        (--upper_bond_)->~T();
                    }
                }
                auto it = data;
                while (lower_bond_ < first_upper_bond)
                {
                    new (it++) T{ std::move(*(lower_bond_)) };
                    (lower_bond_++)->~T();
                }
                if (lower_bond_ > upper_bond_)
                {
                    lower_bond_ = data_begin;
                }
                while (lower_bond_ < upper_bond_)
                {
                    new (it++) T{ std::move(*(lower_bond_)) };
                    (lower_bond_++)->~T();
                }
                upper_bond_ = it;
            }
            else
            {
                upper_bond_ = data + size;
            }
            if (data_begin)
            {
                allocator.deallocate(data_begin, capacity_);
            }
            capacity_ = size;
            data_end_ = data + capacity_;
            lower_bond_ = data;
        }
    }

    // Return the number of items currently in the ring
    std::size_t size() const
    {
        if (upper_bond_ != data_end_)
        {
            if (upper_bond_ > lower_bond_)
            {
                return upper_bond_ - lower_bond_;
            }
            else
            {
                return upper_bond_ + capacity_ - lower_bond_;
            }
        }
        return 0;
    }

    // Return the maximum number of items that can fit in the ring
    std::size_t capacity() const
    {
        return capacity_;
    }

private:
    void unset_empty()
    {
        if (upper_bond_ == data_end_)
        {
            upper_bond_ = data_end_ - capacity_;
            lower_bond_ = upper_bond_;
        }
    }
};

#endif // SRC_RING_HPP
