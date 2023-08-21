#pragma once

#include <mutex>
#include <atomic>
#include <future>

using namespace std::string_literals;

template <typename Key, typename Value>
class ConcurrentMap {
private:
    
    struct Bucket_Map {
        std::mutex m;
        std::map<Key, Value> map;
    };

    std::vector < Bucket_Map > buckets_;

public:
    static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys"s);

    struct Access {
        std::lock_guard<std::mutex> guard;
        Value& ref_to_value;
        
        Access(const Key& key, Bucket_Map& bucket) : guard(bucket.m), ref_to_value(bucket.map[key])
        {
        }
    };

    explicit ConcurrentMap(size_t bucket_count) : buckets_(bucket_count)
    {
    }

    Access operator[](const Key& key) {
        auto& bucket = buckets_[static_cast<uint64_t>(key) % buckets_.size()];
        return { key, bucket };
    }

    size_t erase(const Key& key) {
        auto& bucket = buckets_[static_cast<uint64_t>(key) % buckets_.size()];
        return bucket.map.erase(key);
        }


    std::map<Key, Value> BuildOrdinaryMap() {
        std::map<Key, Value> result;
        for (auto& [m, map] : buckets_) {
            std::lock_guard g(m);
            result.insert(map.begin(), map.end());
        }
        return result;
    }


};

