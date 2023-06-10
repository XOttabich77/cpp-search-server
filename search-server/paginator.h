#pragma once

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end, size_t size)
        :begin_(begin),
        end_(end),
        size_(size) {}

    auto begin() const {
        return begin_;
    }
    auto end() const {
        return end_;
    }
    auto size() const {
        return size_;
    }

private:
    Iterator begin_;
    Iterator  end_;
    size_t size_;
};

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator it_begin, Iterator it_end, size_t size) {
        for (auto it_current = it_begin; it_current < it_end; advance(it_current, size)) {
            auto it_next = it_current;
            size_t size_current = distance(it_current, it_end);
            if (size > size_current) {
                size = size_current;
            }
            advance(it_next, size);
            pages_.push_back(IteratorRange<Iterator> {it_current, it_next, size});
        }
    }
    auto begin() const {
        return pages_.begin();
    }
    auto end() const {
        return pages_.end();
    }
private:
    std::vector<IteratorRange<Iterator>> pages_;
};
