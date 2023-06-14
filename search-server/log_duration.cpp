#include "log_duration.h"

LogDuration::LogDuration(const std::string& id)
    : LogDuration(id, std::cerr)
{
}
LogDuration::LogDuration(const std::string& id, std::ostream& os)
    : id_(id),
    out_(&os)
{
}

LogDuration::~LogDuration()
{
    using namespace std::chrono;
    using namespace std::literals;

    const auto end_time = Clock::now();
    const auto dur = end_time - start_time_;
    *out_ << id_ << ": "s << duration_cast<milliseconds>(dur).count() << " ms"s << std::endl;
}