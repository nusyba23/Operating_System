/// =========================================================================
/// Copyright (C) 2024 Jonathan Hudson (jwhudson@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the only file you should modify and submit for grading



//Modified by Nusyba Shifa(30162709)


#include "pagesim.h"

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits>

//helper function to findd   the next use of a page
int findNextUse(int page, const std::vector<int>& ref, std::vector<int>::size_type startPos) {
    for (std::vector<int>::size_type i = startPos; i < ref.size(); ++i) {
        if (ref[i] == page) {
            return i;
        }
    }
    return std::numeric_limits<int>::max();                         //if not found, return max value
}

int optimal(int frame_count, std::vector<int>& ref, std::vector<int>& frames) {
    int page_faults = 0;
    std::vector<bool> inFrame(100, false);                          //keep track of whether a page is in a frame

    for (std::vector<int>::size_type i = 0; i < ref.size(); ++i) {
        int page = ref[i];
        //check if the page is already loaded
        if (!inFrame[page]) {
            page_faults++;
            if (std::find(frames.begin(), frames.end(), -1) != frames.end()) { //if there's an empty frame
                auto it = std::find(frames.begin(), frames.end(), -1);
                *it = page;
                inFrame[page] = true;
            } else {
                //find the page that will not be used for the longest time
                int maxDistance = -1, pageToReplace = -1;
                for (int j = 0; j < frame_count; ++j) {
                    int nextPageUse = findNextUse(frames[j], ref, i + 1);
                    if (nextPageUse > maxDistance) {
                        maxDistance = nextPageUse;
                        pageToReplace = j;
                    }
                }
                inFrame[frames[pageToReplace]] = false;                         //remove the old page
                frames[pageToReplace] = page;                                   //insert the new page
                inFrame[page] = true;
            }
        }
    }
    return page_faults;
}

int lru(int frame_count, std::vector<int>& ref, std::vector<int>& frames) {
    int page_faults = 0;
    std::unordered_map<int, int> lastUsed;
    std::vector<int> loaded(frame_count, -1);

    for (std::vector<int>::size_type i = 0; i < ref.size(); ++i) {
        if (std::find(loaded.begin(), loaded.end(), ref[i]) == loaded.end()) {  //page faultt
            page_faults++;
            int lru = i, replaceIndex = 0;
            for (int j = 0; j < frame_count; ++j) {
                if (loaded[j] == -1) {                                          //empty slot found
                    replaceIndex = j;
                    break;
                }
                if (lastUsed[loaded[j]] < lru) {
                    lru = lastUsed[loaded[j]];
                    replaceIndex = j;
                }
            }
            loaded[replaceIndex] = ref[i];
        }
        lastUsed[ref[i]] = i;
    }
    frames = loaded;
    return page_faults;
}

int clock(int frame_count, std::vector<int>& ref, std::vector<int>& frames) {
    int page_faults = 0, clockHand = 0;
    std::vector<int> refBit(frame_count, 0);
    std::unordered_map<int, int> pageToFrame;

    for (int request : ref) {
        if (pageToFrame.find(request) == pageToFrame.end()) {       //Page fault
            page_faults++;
            while (refBit[clockHand] == 1) {
                refBit[clockHand] = 0;          //clear the reference bit
                clockHand = (clockHand + 1) % frame_count;          //Move the clock hand
            }
            // Replace the page at clockHand
            if (frames[clockHand] != -1) {
                pageToFrame.erase(frames[clockHand]);
            }
            frames[clockHand] = request;
            pageToFrame[request] = clockHand;
            refBit[clockHand] = 1;      //set the reference bit for the new pageg
            clockHand = (clockHand + 1) % frame_count;
        } else {

            //if page is already in a frame, set its reference bit
            refBit[pageToFrame[request]] = 1;
        }
    }
    return page_faults;
}
