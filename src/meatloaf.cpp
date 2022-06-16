#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <map>

#include "lexer.hpp"
#include "parser.hpp"
#include "git.hpp"
#include "memory.hpp"

#define ML_VERSION "0.0.0.b"

/**
 TODO: Interpretation Variable Table
       Lambda functions
       If statements
       Booleans
       Fix issue with circumfix internal parsing
*/

/**
 Hardcoded cooked beef logo.
*/
void print_logo()
{
    #if defined(_WIN32)
        system("chcp 65001");
    #endif
    system("");
    std::cout << "\x1B[0m                \x1B[0m\n"
                 "\x1B[0m         \x1B[38;2;78;39;25m▄\x1B[0m\x1B[38;2;78;39;25m▄\x1B[0m\x1B[38;2;78;39;25m▄\x1B[0m\x1B[38;2;78;39;25m▄\x1B[0m   \x1B[0m\n"
                 "\x1B[0m       \x1B[38;2;78;39;25m▄\x1B[48;2;78;39;25m\x1B[38;2;82;47;31m▄\x1B[48;2;113;63;45m\x1B[38;2;152;92;67m▄\x1B[48;2;124;72;53m\x1B[38;2;142;82;60m▄\x1B[48;2;113;63;45m\x1B[38;2;103;55;40m▄\x1B[48;2;142;82;60m\x1B[38;2;113;63;45m▄\x1B[48;2;78;39;25m\x1B[38;2;103;55;40m▄\x1B[0m\x1B[38;2;78;39;25m▄\x1B[0m \x1B[0m\n" \
                 "\x1B[0m    \x1B[38;2;78;39;25m▄\x1B[0m\x1B[38;2;78;39;25m▄\x1B[48;2;78;39;25m\x1B[38;2;103;55;40m▄\x1B[48;2;142;82;60m\x1B[38;2;82;47;31m▄\x1B[48;2;142;82;60m\x1B[38;2;152;92;67m▄\x1B[48;2;82;47;31m\x1B[38;2;142;82;60m▄\x1B[48;2;124;72;53m\x1B[38;2;82;47;31m▄\x1B[48;2;142;82;60m\x1B[38;2;124;72;53m▄\x1B[48;2;82;47;31m\x1B[38;2;113;63;45m▄\x1B[48;2;113;63;45m\x1B[38;2;82;47;31m▄\x1B[48;2;42;22;13m \x1B[0m \x1B[0m\n" \
                 "\x1B[0m   \x1B[48;2;78;39;25m \x1B[48;2;82;47;31m\x1B[38;2;113;63;45m▄\x1B[48;2;124;72;53m\x1B[38;2;82;47;31m▄\x1B[48;2;142;82;60m\x1B[38;2;152;92;67m▄\x1B[48;2;103;55;40m\x1B[38;2;142;82;60m▄\x1B[48;2;82;47;31m\x1B[38;2;142;82;60m▄\x1B[48;2;152;92;67m\x1B[38;2;82;47;31m▄\x1B[48;2;142;82;60m\x1B[38;2;152;92;67m▄\x1B[48;2;113;63;45m\x1B[38;2;103;55;40m▄\x1B[48;2;103;55;40m\x1B[38;2;63;33;22m▄\x1B[48;2;63;33;22m\x1B[38;2;42;22;13m▄\x1B[0m\x1B[38;2;42;22;13m▀\x1B[0m \x1B[0m\n" \
                 "\x1B[0m  \x1B[48;2;78;39;25m \x1B[48;2;113;63;45m\x1B[38;2;152;92;67m▄\x1B[48;2;124;72;53m\x1B[38;2;113;63;45m▄\x1B[48;2;152;92;67m\x1B[38;2;124;72;53m▄\x1B[48;2;82;47;31m\x1B[38;2;152;92;67m▄\x1B[48;2;152;92;67m\x1B[38;2;82;47;31m▄\x1B[48;2;142;82;60m\x1B[38;2;113;63;45m▄\x1B[48;2;113;63;45m\x1B[38;2;124;72;53m▄\x1B[48;2;82;47;31m\x1B[38;2;103;55;40m▄\x1B[48;2;63;33;22m \x1B[38;2;42;22;13m▄\x1B[0m\x1B[38;2;42;22;13m▀\x1B[0m  \x1B[0m\n" \
                 "\x1B[0m   \x1B[48;2;78;39;25m\x1B[38;2;42;22;13m▄\x1B[48;2;152;92;67m\x1B[38;2;42;22;13m▄\x1B[48;2;142;82;60m\x1B[38;2;82;47;31m▄\x1B[48;2;142;82;60m\x1B[38;2;63;33;22m▄\x1B[48;2;124;72;53m\x1B[38;2;63;33;22m▄\x1B[48;2;103;55;40m\x1B[38;2;63;33;22m▄\x1B[48;2;63;33;22m\x1B[38;2;42;22;13m▄\x1B[48;2;63;33;22m\x1B[38;2;42;22;13m▄\x1B[0m\x1B[38;2;42;22;13m▀\x1B[0m    \x1B[0m\n" \
                 "\x1B[0m     \x1B[38;2;42;22;13m▀▀▀▀\x1B[0m       \x1B[0m\n";

    std::cout << "Meatloaf version " ML_VERSION " developed by Ricky and Kecky.\n\n\n";
}

void print_help() 
{
    std::cout << "Meatloaf helppage\n\n";
    std::cout << "no arguments:\n\topens interactive grill.\n\n";
    std::cout << "-h|--help:\n\tShows this page!\n\n";
    std::cout << "-v|--version:\n\tShows the version!\n";
}

void print_version() 
{
    std::cout << "OS: " OS ", architechture: " ARCH "\n";
    std::cout << "Git branch: " GIT_BRANCH ", revision: " GIT_REV ", tag: " GIT_TAG "\n";
}

void interactive_grill() 
{
    std::cout << "Welcome to the Meatloaf interactive grill\n Get ready to make your meat and we'll bread em.\nVersion " ML_VERSION "\n"
              << std::endl;
    std::string cmdlet;
    Stack global;

    while (true) {
        std::cout << "->- ";
        std::getline(std::cin, cmdlet);
        Source::text = cmdlet;
        Source::file = "<stdin>";

        auto lres = tokenize();
        if (lres.failed == false) {
            for (Token const& tk : lres.value) {
                std::cout << tk;
            };
            // more tokens than EOF
            if (lres.value.size() > 1) {
                std::cout << "\n";
            }
        }
        else {
            std::cout << *(lres.error) << std::endl;
            continue;
        }

        auto result = ast(lres.value);
        if (result.failed == true) {
            std::cout << *(result.error);
        }

        for (auto& nd : result.value) {
            auto res = nd->eval(global);

            if (res.failed) {
                std::cout << *(res.error);
            }
            else if (res.value != nullptr) {
                std::cout << res.value->repr() << " " << get_mltype_repr(res.value->get_type());
            }
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) 
{
    // print_logo();

    if(argc==1) {
        interactive_grill();
    } else {
        char* arg = argv[1];
        if(arg == "--help" || arg == "-h") {
            print_help();
        } else if(arg == "--version" || arg == "-v") {
            print_version();
        } else std::cout << "Invalid argument " << arg << "! use " << argv[0] << " --help for help!\n";
    }
    
    std::cout << std::endl;
    return 0;
}
