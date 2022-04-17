#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

class Path
{
private:
#ifdef _WIN32
    static inline const char SEPARATOR = '\\';
    static inline const char ALT_SEPARATOR = '/';
#else
    static inline const char SEPARATOR = '/';
    static inline const char ALT_SEPARATOR = '/';
#endif

    std::string value;

public:
    Path(const std::string_view value_ = {}) : value { value_ }
    {} 

    bool empty() const
    {
        return value.empty();
    }

    Path operator/(const Path& other) const
    {
        return Path { value + SEPARATOR + other.value };
    }

    const std::string& string() const { return value; }

    void replace_filename(const std::string_view filename)
    {
        auto pos = value.find_last_of(SEPARATOR);
        if (pos == std::string::npos)
        {
            pos = value.find_last_of(ALT_SEPARATOR);
        }

        value.replace(pos + 1, std::string::npos, filename);
    }
};

class MediaPath
{
public:
    static inline void initialize(const std::string_view executable_path)
    {
        media_path = executable_path;
        media_path.replace_filename("media");
    }

    MediaPath(const std::string_view path_) : path { path_ } {}

    Path get_full_path() const
    {
        if (media_path.empty())
        {
            throw std::runtime_error { "MediaPath::initialize() should have been called before "
                                       "trying to retrieve a path from MediaPath." };
        }

        return media_path / path;
    }

private:
    static inline Path media_path;

    const Path path;
};
