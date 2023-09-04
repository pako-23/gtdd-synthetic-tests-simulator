#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <cstdint>
#include <ctime>
#include <map>
#include <sstream>
#include <string>

struct Parameter {
    friend class ArgParser;

    Parameter(const char*, const char*, const char* = nullptr, bool = false);
    virtual ~Parameter() {};

    virtual void parse(const char*) { parameter_set = true; };

private:
    std::string long_name;
    std::string short_name;
    std::string description;
    bool required;
    bool parameter_set;
};

template <typename T>
struct ValuedParameter : public Parameter {
    ValuedParameter(const char* long_name,
        const char* description,
        T& value,
        const char* short_name = nullptr,
        bool required = false)
        : Parameter { long_name, description, short_name, required }
        , value { value }
    {
    }
    void parse(const char* option) override
    {
        Parameter::parse(option);
        std::istringstream ss { option };
        ss >> value;
    };

private:
    T& value;
};

struct ArgParser {
    ArgParser(const std::string&, const std::string&);
    ~ArgParser();

    void print_help(std::ostream&);
    void add_parameter(Parameter*);
    void parse(int argc, const char* argv[]);

private:
    std::string prog_name;
    std::string desc;
    std::map<std::string, Parameter*> parameters;
    std::map<std::string, std::string> short_names;
};

bool file_exists(const char*);
void create_dir(const char*);
std::string
graphviz_filename(const std::string&, tm*);

template <typename T>
std::ostringstream&
join_helper(std::ostringstream& os, const std::string&, T v)
{
    os << v;
    return os;
}

template <typename T, typename... Types>
std::ostringstream&
join_helper(std::ostringstream& os,
    const std::string& sep,
    T v,
    Types... values)
{
    os << v << sep;
    return join_helper(os, sep, values...);
}

template <typename... Types>
std::string
join(const std::string& sep, Types... values)
{
    std::ostringstream oss {};
    return join_helper(oss, sep, values...).str();
}

#endif
