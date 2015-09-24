#pragma once

#include <vector>
#include <string>
#include <sstream>

#define DEFINE_int(argument, shortArg, description, defaultValue) int FLAG_##argument = defaultValue; Acidrain::CommandLineOptionRegistrarInteger FLAG_REGISTRAR_##argument(#argument "," #shortArg, description, defaultValue, FLAG_##argument);
#define DEFINE_bool(argument, shortArg, description, defaultValue)  bool FLAG_##argument = defaultValue; Acidrain::CommandLineOptionRegistrarBool FLAG_REGISTRAR_##argument(#argument "," #shortArg, description, defaultValue, FLAG_##argument);
#define DEFINE_string(argument, shortArg, description, defaultValue)  ::std::string FLAG_##argument = defaultValue; Acidrain::CommandLineOptionRegistrarString FLAG_REGISTRAR_##argument(#argument "," #shortArg, description, defaultValue, FLAG_##argument);

#define DECLARE_int(argument) extern int FLAG_##argument;
#define DECLARE_bool(argument) extern bool FLAG_##argument;
#define DECLARE_string(argument) extern string FLAG_##argument;

#define IS_SPECIFIED(argument) Acidrain::CommandLineParser::instance().isArgumentSpecified(#argument)

namespace Acidrain {

    using namespace std;

    enum CommandLineOptionType {
        STRING,
        INTEGER,
        BOOLEAN
    };

    struct CommandLineOptionValue {
        CommandLineOptionValue() = default;

        CommandLineOptionValue(string& value) : stringValue(value) {
        }

        CommandLineOptionValue(int value) : intValue(value) {
        }

        CommandLineOptionValue(bool value) : boolValue(value) {
        }

        int intValue;
        string stringValue;
        bool boolValue;
    };

    struct CommandLineOption {
        string shortOption;
        string longOption;
        string description;
        CommandLineOptionValue defaultValue;
        bool isSpecified = false;
        CommandLineOptionValue value;
        CommandLineOptionType type;
        void* valueHolder;
    };

    class CommandLineParser {
    public:

        static CommandLineParser& instance();

        CommandLineParser& addString(string argumentNames, string description, string defaultValue, void* valueHolder);

        CommandLineParser& addBool(string argumentNames, string description, bool defaultValue, void* valueHolder);

        CommandLineParser& addInteger(string argumentNames, string description, int defaultValue, void* valueHolder);

        CommandLineParser& parse(int argc, char** argv);

        void usage();

        CommandLineParser& dump();

        string paramAsString(string name);

        int paramAsInt(string name);

        bool paramAsBool(string name);

        bool isArgumentSpecified(string argument);

    private:
        vector<CommandLineOption> options;

        CommandLineOption* optionByArgument(char const* name);

        CommandLineOption* optionByLongName(char const* name);

        CommandLineOption* optionByShortName(char const* name);

        CommandLineOption* optionByName(string name);
    };

    void ParseCommandLineFlags(int argc, char** argv);

    class CommandLineOptionRegistrarInteger {
    public:
        CommandLineOptionRegistrarInteger(string argumentNames, string description, int defaultValue, int& valueHolder) {
            CommandLineParser::instance().addInteger(argumentNames, description, defaultValue, &valueHolder);
        }
    };

    class CommandLineOptionRegistrarString {
    public:
        CommandLineOptionRegistrarString(string argumentNames, string description, string defaultValue, string& valueHolder) {
            CommandLineParser::instance().addString(argumentNames, description, defaultValue, &valueHolder);
        }
    };

    class CommandLineOptionRegistrarBool {
    public:
        CommandLineOptionRegistrarBool(string argumentNames, string description, bool defaultValue, bool& valueHolder) {
            CommandLineParser::instance().addBool(argumentNames, description, defaultValue, &valueHolder);
        }
    };
}