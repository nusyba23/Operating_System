/// =========================================================================
/// Copyright (C) 2024 Jonathan Hudson (jwhudson@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// Modified By Nusyba Shifa (30162709)

#include "fat.h"
#include <vector>
#include <map>
#include <algorithm>
#include <cstdio>
#include <iostream>

//This code defines a function named checkConsistency that is designed to check the consistency of a file 
//allocation table (FAT) against a set of file entries. It's part of a file system checking tool, where the 
//FAT is a data structure used to keep track of the blocks of storage used by files on a disk.
// Return value:
//   the function should return the number of free blocks
//   also, for ever entry in the files[] array, you need to set the appropriate flags:
//      i.e. tooManyBlocks, tooFewBlocks, hasCycle and sharesBlocks


//keep track of visiited bllsocks, initaialized to false indiceating no blocks have been visited yt.
int checkConsistency(int blockSize, std::vector<DEntry>& files, std::vector<int>& fat) {
    std::vector<bool> visited(fat.size(), false);
    std::map<int, std::vector<int>> blockToFileIndexes; //map to associate blocks 
    int freeBlocks = 0;

    // iterate through each file entrry to check and set conssiistency flags
    for (size_t i = 0; i < files.size(); ++i) {
        DEntry& file = files[i];
        int currentBlock = file.ind;    //starteing block wiith curent flag
        std::vector<int> fileBlocks;
        std::map<int, bool> visitedBlocks;

        // reset error flag for current fiel
        file.hasCycle = false;
        file.tooFewBlocks = false;
        file.tooManyBlocks = false;
        file.sharesBlocks = false;

    //traverse the FAT chaain for the current foile
        while (currentBlock != -1) {
            if (visitedBlocks[currentBlock]) { // cycle detection
                file.hasCycle = true;
                break;
            }
            if (visited[currentBlock]) { // shared block  detection
                file.sharesBlocks = true;
            }
            visitedBlocks[currentBlock] = true;
            visited[currentBlock] = true;
            fileBlocks.push_back(currentBlock);
            currentBlock = fat[currentBlock];
        }

        //   updating the blockToFileIndexes map
        for (int block : fileBlocks) {
            blockToFileIndexes[block].push_back(i);
        }

  // calculating required and  actual blocks forr the file
        int requiredBlocks = (file.size + blockSize - 1) / blockSize;
        int actualBlocks = fileBlocks.size();

        // setting tooFewBlocks and tooManyBlocks flags allocated to the file
        if (actualBlocks < requiredBlocks) file.tooFewBlocks = true;
        if (actualBlocks > requiredBlocks) file.tooManyBlocks = true;
    }

    // check for shared blocks after full file processing
    for (const auto& entry : blockToFileIndexes) {
        if (entry.second.size() > 1) {
            for (int index : entry.second) {
                files[index].sharesBlocks = true;
            }
        }
    }

    // Count the number of  blockss that were never visited 
    freeBlocks = std::count(visited.begin(), visited.end(), false);

    return freeBlocks;
}
