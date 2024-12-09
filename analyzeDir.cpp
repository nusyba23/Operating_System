
//// =========================================================================
/// Written by pfederl@ucalgary.ca in 2023, for CPSC457.
/// =========================================================================
///edited by Nusyba Shifa
///


#include "analyzeDir.h"

#include <cassert>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

struct FileData {
    std::string path;
    long size;
};

// Helper function declarations
bool is_dir(const std::string &path);
bool is_file(const std::string &path);
void analyze_file(const std::string &path, Results &res);
void analyze_directory(const std::string &path, Results &res, int &dirCount, int &fileCount, long &totalSize);




// New function to process directories and track file counts
void analyze_directory(const std::string &path, Results &res, std::map<std::string, int>& fileCounts, int &dirCount, int &fileCount, long &totalSize, std::string currentPath = ".") {
    DIR* dir = opendir(path.c_str());
    assert(dir != nullptr);
    dirent* entry;

    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        std::string fullPath = path + "/" + name;
        struct stat statBuffer;
        if (stat(fullPath.c_str(), &statBuffer) == 0) {
            if (S_ISDIR(statBuffer.st_mode)) {
                dirCount++;
                analyze_directory(fullPath, res, fileCounts, dirCount, fileCount, totalSize, currentPath + "/" + name);
            } else if (S_ISREG(statBuffer.st_mode)) {
                fileCount++;
                totalSize += statBuffer.st_size;
                fileCounts[currentPath]++;

                // Updating the largest file 
                if (statBuffer.st_size > res.largest_file_size) {
                    res.largest_file_size = statBuffer.st_size;
                    res.largest_file_path = fullPath;
                }
            }
        }
    }
    closedir(dir);
}

Results analyzeDir(int n) {
    Results res;
    res.largest_file_size = -1;
    int dirCount = 1, fileCount = 0;
    long totalSize = 0;
    std::map<std::string, int> fileCounts; // Track

    // Initializing
    fileCounts["."] = 0;

    // Startingss the analsie frm curent dircry
    analyze_directory(".", res, fileCounts, dirCount, fileCount, totalSize);

    // Identify the largest directory by file count
    auto largestDir = std::max_element(fileCounts.begin(), fileCounts.end(), 
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second < b.second;
        });

    if (largestDir != fileCounts.end()) {
        // Adjusing the path string 
        std::string largestDirPath = largestDir->first == "." ? largestDir->first : largestDir->first.substr(2);
        res.largest_dirs.push_back({largestDirPath, largestDir->second});
    }

    // popelate remaining fiels
    res.n_files = fileCount;
    res.n_dirs = dirCount;
    res.all_files_size = totalSize;

    return res;
}

void analyze_directory(const std::string &path, Results &res, int &dirCount, int &fileCount, long &totalSize) {
    auto dir = opendir(path.c_str());
    assert(dir != nullptr);
    struct dirent *de;
    int localFileCount = 0; // lsocal file count 

    while ((de = readdir(dir)) != nullptr) {
        std::string name = de->d_name;
        // Skip . and ..
        if (name == "." || name == "..") continue;

        std::string fullPath = path + "/" + name;
        if (is_dir(fullPath)) {
            dirCount++;
            analyze_directory(fullPath, res, dirCount, fileCount, totalSize); // Recurse  in subdirectoyr
        } else if (is_file(fullPath)) {
            fileCount++;
            localFileCount++; 
            analyze_file(fullPath, res); 
            struct stat buff;
            if (0 == stat(fullPath.c_str(), &buff)) {
                totalSize += buff.st_size;
            }
        }
    }
    closedir(dir);

    // After processing the directory, check if it has the most files
    if (localFileCount > 0) { 
        res.largest_dirs.push_back({path, localFileCount});
        
        std::sort(res.largest_dirs.begin(), res.largest_dirs.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second;
        });
       
        if (res.largest_dirs.size() > 1) {
            res.largest_dirs.resize(1); // keep only the lagest directory
        }
    }
}

void analyze_file(const std::string &path, Results &res) {
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) == 0) {
        if (S_ISREG(fileInfo.st_mode)) { 
            long fileSize = fileInfo.st_size;
            
            // Updating total sizee
            res.all_files_size += fileSize;
            
            // Check and update if this is the largest file so far
            if (fileSize > res.largest_file_size) {
                res.largest_file_size = fileSize;
                res.largest_file_path = path;
            }

            
            res.n_files++;
        }
    } else {
        std::cerr << "Failed to get file info for: " << path << std::endl;
    }
}


bool is_dir(const std::string & path) {
    struct stat buff;
    if (0 != stat(path.c_str(), &buff)) return false;
    return S_ISDIR(buff.st_mode);
}

bool is_file(const std::string & path) {
    struct stat buff;
    if (0 != stat(path.c_str(), &buff)) return false;
    return S_ISREG(buff.st_mode);
}

bool ends_with(const std::string& value, const std::string& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}