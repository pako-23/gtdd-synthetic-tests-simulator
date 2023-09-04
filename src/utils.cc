#include "utils.h"
#include <iomanip>
#include <iostream>
#include <sys/stat.h>

Parameter::Parameter(const char* long_name,
    const char* description,
    const char* short_name,
    bool required)
    : long_name { long_name }
    , short_name { short_name == nullptr ? "" : short_name }
    , description { description }
    , required { required }
    , parameter_set { false }
{
}

ArgParser::ArgParser(const std::string& program_name,
    const std::string& description)
    : prog_name { program_name }
    , desc { description }
{
    add_parameter(new Parameter { "help", "Show the help page", "h" });
}

ArgParser::~ArgParser()
{
    for (const auto& it : parameters)
        delete (it.second);
}

void ArgParser::print_help(std::ostream& os)
{
    os << std::endl
       << "NAME" << std::endl
       << prog_name << " -- " << desc << std::endl
       << std::endl
       << "SYNOPSIS" << std::endl
       << "    " << prog_name;

    if (!short_names.empty()) {
        os << " [-";
        for (const auto& it : short_names)
            os << it.first;
        os << ']';
    }

    os << std::endl
       << std::endl
       << "DESCRIPTION" << std::endl
       << "      The program has the following options:" << std::endl
       << std::endl;
    size_t max_len = 0;
    for (const auto& param : parameters)
        max_len = std::max(max_len, param.second->long_name.length());

    for (const auto& param : parameters) {
        if (param.second->short_name.empty())
            os << "    ";
        else
            os << "  -" << param.second->short_name;

        os << "  [--" << param.second->long_name << "]";
        size_t tabbing = (max_len - param.second->long_name.length()) + 4;
        for (size_t i = 0; i < tabbing; ++i)
            os << ' ';
        os << param.second->description;
        if (param.second->required)
            os << " (Required)";
        os << std::endl;
    }
    os << std::endl;
}

void ArgParser::add_parameter(Parameter* param)
{
    if (parameters.find(param->long_name) != parameters.end())
        return;

    if (!param->short_name.empty()) {
        if (short_names.find(param->short_name) != short_names.end())
            return;
        short_names[param->short_name] = param->long_name;
    }

    parameters[param->long_name] = param;
}

void ArgParser::parse(int argc, const char* argv[])
{
    for (int i = 1; i < argc; i++) {
        const char* begin = argv[i];
        size_t size = 0;
        for (; *begin == '-'; ++begin)
            ;
        for (; *(begin + size) && *(begin + size) != '='; ++size)
            ;
        std::string option { begin, size };

        if (option == "help" || option == "h") {
            print_help(std::cout);
            exit(EXIT_SUCCESS);
        }

        auto long_name = short_names.find(option);
        auto param { parameters.find(
            long_name == short_names.end() ? option : long_name->second) };

        if (param == parameters.end()) {
            std::cerr << prog_name << ": " << argv[i]
                      << " is not a recognized parameter." << std::endl;
            print_help(std::cerr);
            exit(EXIT_FAILURE);
        }

        param->second->parse(begin + size + 1);
    }

    for (const auto& it : parameters)
        if (it.second->required && !it.second->parameter_set) {
            std::cerr << prog_name << ": --" << it.second->long_name
                      << " is a required parameter." << std::endl;

            print_help(std::cerr);
            exit(EXIT_FAILURE);
        }
}

bool file_exists(const char* path)
{
    struct stat st = { 0 };
    return stat(path, &st) == 0;
}

void create_dir(const char* path)
{
    if (!file_exists(path))
        mkdir(path, 0755);
}

std::string
graphviz_filename(const std::string& prefix, tm* tm)
{
    std::ostringstream oss {};
    oss << prefix << '-' << std::put_time(tm, "%d-%m-%Y-%H-%M-%S") << ".gv";
    return oss.str();
}
