#pragma once

#include <vector>
#include <string>
#include <sstream>

#define FLAG_ADD_INTEGER(argument,description,defaultValue) Acidrain::CommandLineParser::instance().addInteger(argument, description, defaultValue)
#define FLAG_ADD_BOOLEAN(argument,description,defaultValue) Acidrain::CommandLineParser::instance().addBool(argument, description, defaultValue)
#define FLAG_ADD_STRING(argument,description,defaultValue) Acidrain::CommandLineParser::instance().addString(argument, description, defaultValue)
#define FLAGS_PARSE(argc,argv) Acidrain::CommandLineParser::instance().parse(argc, argv)

#define FLAG_AS_STRING(name) Acidrain::CommandLineParser::instance().paramAsString(name)
#define FLAG_AS_BOOLEAN(name) Acidrain::CommandLineParser::instance().paramAsBool(name)
#define FLAG_AS_INTEGER(name) Acidrain::CommandLineParser::instance().paramAsInt(name)

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
    };

    class CommandLineParser {
    public:

        static CommandLineParser& instance();

        CommandLineParser& addString(string argumentNames, string description, string defaultValue = "");

        CommandLineParser& addBool(string argumentNames, string description, bool defaultValue = false);

        CommandLineParser& addInteger(string argumentNames, string description, int defaultValue = 0);

//    bool hasDefined(string argumentName);
//    int get(string argumentName);
//    string get(string argumentName);
//    bool get(string argumentName);

        CommandLineParser& parse(int argc, char** argv);

        CommandLineParser& dump();

        string paramAsString(string name);

        int paramAsInt(string name);

        bool paramAsBool(string name);

    private:
        vector<CommandLineOption> options;

        CommandLineOption* optionByArgument(char const* name);

        CommandLineOption* optionByLongName(char const * name);

        CommandLineOption* optionByShortName(char const * name);

        CommandLineOption* optionByName(string name);
    };
}