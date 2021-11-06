#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "osversion.h"
#include "git.h"

#define ML_VERSION "0.0.0.b"

/**
 Hardcoded cooked beef logo.
*/
void print_logo() 
{
    #if defined(_WIN32)
        system("chcp 65001");
    #endif
    system("");
    std::cout << "\e[0m                \e[0m\n" \
                 "\e[0m         \e[38;2;78;39;25m▄\e[0m\e[38;2;78;39;25m▄\e[0m\e[38;2;78;39;25m▄\e[0m\e[38;2;78;39;25m▄\e[0m   \e[0m\n" \
                 "\e[0m       \e[38;2;78;39;25m▄\e[48;2;78;39;25m\e[38;2;82;47;31m▄\e[48;2;113;63;45m\e[38;2;152;92;67m▄\e[48;2;124;72;53m\e[38;2;142;82;60m▄\e[48;2;113;63;45m\e[38;2;103;55;40m▄\e[48;2;142;82;60m\e[38;2;113;63;45m▄\e[48;2;78;39;25m\e[38;2;103;55;40m▄\e[0m\e[38;2;78;39;25m▄\e[0m \e[0m\n" \
                 "\e[0m    \e[38;2;78;39;25m▄\e[0m\e[38;2;78;39;25m▄\e[48;2;78;39;25m\e[38;2;103;55;40m▄\e[48;2;142;82;60m\e[38;2;82;47;31m▄\e[48;2;142;82;60m\e[38;2;152;92;67m▄\e[48;2;82;47;31m\e[38;2;142;82;60m▄\e[48;2;124;72;53m\e[38;2;82;47;31m▄\e[48;2;142;82;60m\e[38;2;124;72;53m▄\e[48;2;82;47;31m\e[38;2;113;63;45m▄\e[48;2;113;63;45m\e[38;2;82;47;31m▄\e[48;2;42;22;13m \e[0m \e[0m\n" \
                 "\e[0m   \e[48;2;78;39;25m \e[48;2;82;47;31m\e[38;2;113;63;45m▄\e[48;2;124;72;53m\e[38;2;82;47;31m▄\e[48;2;142;82;60m\e[38;2;152;92;67m▄\e[48;2;103;55;40m\e[38;2;142;82;60m▄\e[48;2;82;47;31m\e[38;2;142;82;60m▄\e[48;2;152;92;67m\e[38;2;82;47;31m▄\e[48;2;142;82;60m\e[38;2;152;92;67m▄\e[48;2;113;63;45m\e[38;2;103;55;40m▄\e[48;2;103;55;40m\e[38;2;63;33;22m▄\e[48;2;63;33;22m\e[38;2;42;22;13m▄\e[0m\e[38;2;42;22;13m▀\e[0m \e[0m\n" \
                 "\e[0m  \e[48;2;78;39;25m \e[48;2;113;63;45m\e[38;2;152;92;67m▄\e[48;2;124;72;53m\e[38;2;113;63;45m▄\e[48;2;152;92;67m\e[38;2;124;72;53m▄\e[48;2;82;47;31m\e[38;2;152;92;67m▄\e[48;2;152;92;67m\e[38;2;82;47;31m▄\e[48;2;142;82;60m\e[38;2;113;63;45m▄\e[48;2;113;63;45m\e[38;2;124;72;53m▄\e[48;2;82;47;31m\e[38;2;103;55;40m▄\e[48;2;63;33;22m \e[38;2;42;22;13m▄\e[0m\e[38;2;42;22;13m▀\e[0m  \e[0m\n" \
                 "\e[0m   \e[48;2;78;39;25m\e[38;2;42;22;13m▄\e[48;2;152;92;67m\e[38;2;42;22;13m▄\e[48;2;142;82;60m\e[38;2;82;47;31m▄\e[48;2;142;82;60m\e[38;2;63;33;22m▄\e[48;2;124;72;53m\e[38;2;63;33;22m▄\e[48;2;103;55;40m\e[38;2;63;33;22m▄\e[48;2;63;33;22m\e[38;2;42;22;13m▄\e[48;2;63;33;22m\e[38;2;42;22;13m▄\e[0m\e[38;2;42;22;13m▀\e[0m    \e[0m\n" \
                 "\e[0m     \e[38;2;42;22;13m▀▀▀▀\e[0m       \e[0m\n";

    std::cout << "Meatloaf version " ML_VERSION " developed by Ricky and Kecky.\n\n\n";
}

void print_help() 
{
    std::cout << "Meatloaf helppage\n\n";
    std::cout << "no arguments:\n\topens interactive grill.\n\n";
    std::cout << "-h|--help:\n\tShows this page!\n\n";
    std::cout << "-v|--version:\n\tShows the version!\n";
}

void print_version() {
    std::cout << "OS: " OS ", architechture: " ARCH "\n";
    std::cout << "Git branch: " GIT_BRANCH ", revision: " GIT_REV ", tag: " GIT_TAG "\n";
}

void interactive_grill() {
    std::cout << "Welcome to the Meatloaf interactive grill\n Get ready to make your meat and we'll bread em.\nVersion " ML_VERSION "\n"
              << std::endl;
    std::string cmdlet;
    while (true) {
        std::cout << "->- ";
        std::getline(std::cin, cmdlet);
        Source::text = cmdlet;
        LexxedResults lres = tokenize();
        std::cout << "... ";
        if (lres.failed() == false)
        {
            for (Token tk : lres.tokens)
            {
                std::cout << tk << ' ';
            };
            std::cout << "\n";
        }
        else
        {
            std::cout << lres.error;
        }

        Parser p(lres.tokens);
        std::vector<Operable*> nodes = p.ast();
        std::cout << nodes[0];
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    print_logo();

    if(argc==1) {
        interactive_grill();
    } else {
        char* arg = argv[1];
        if(strcmp(arg, "--help")==0 || strcmp(arg, "-h")==0) {
            print_help();
        } else if(strcmp(arg, "--version")==0 || strcmp(arg, "-v")==0) {
            print_version();
        } else std::cout << "Invalid argument " << arg << "! use " << argv[0] << " --help for help!\n";
    }
    
    std::cout << std::endl;
    return 0;
}
