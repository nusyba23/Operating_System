// -------------------------------------------------------------------------------------
// this is the only file you need to edit
// -------------------------------------------------------------------------------------
//
// (c) 2023, Pavol Federl, pfederl@ucalgary.ca
// Do not distribute this file.

#include "fatsim.h"
#include <iostream>
#include <algorithm>


int maxPathLength = 0;


void DFSPathFinder(long node, std::vector<long> & path, std::vector<std::vector<long>> & paths, std::vector<std::vector<long>> & adj_list) {

  //psuedo recursive call for DFS

  //DFS(node, & path, & paths, & adj_list)
    //for i in adj_list[node]
      //if adj_list[i] len == 0 -- if the adj list for this node is empty, ie no branches off of it and the path is finished
        //path.push_back(i)
        //paths.push_back(path)
        //check for maxlength
        //path.pop_back()
      //else -- if the path is not yet finished and there are 1 or more paths off of this node
        //path.push_back(i)
        //DFS(i, path, paths, adj_list)
        //path.pop_back()

  for (long i : adj_list[node]) {
    if (adj_list[i].size() == 0) {
      path.push_back(i);
      paths.push_back(path);
      //check for maxlength
      if (maxPathLength < (int)path.size())
        maxPathLength = (int)path.size();
      path.pop_back();
    }
    else {
      path.push_back(i);
      DFSPathFinder(i, path, paths, adj_list);
      path.pop_back();
    }
  }

}

// reimplement this function
std::vector<long> fat_check(const std::vector<long> & fat)
{

  /*
  std::vector<long> results;
  int maxLength = 0;

  int fileLength;
  long nextBlock;

  for (long i = 0; i < (long)fat.size(); i++) {

    //trace the file starting at i
    fileLength = 1;
    nextBlock = i;
    while(true) {
      if (fat[nextBlock] == -1)
        break;
      if (fileLength >= (int)fat.size())
        break;
      nextBlock = fat[nextBlock];
      fileLength++;
    }

    //if it is a loop (length >= length of fat), then is corrupt, discard
    if (fileLength >= (int)fat.size())
      continue;

    //if it is of length > max, clear all noted starts, and mark down current file i
    if (fileLength > maxLength) {
      maxLength = fileLength;
      results.clear();
      results.push_back(i);
    }

    //if it is of length == max, just mark down current file i
    else if (fileLength == maxLength)
      results.push_back(i);

  }

  return results; //pretty sure it is already sorted in ascending order, as all files of equal length will be iterated through and appended in ascending order
  */

  std::vector<std::vector<long>> adj_list((int)fat.size()); //init adjacency list with a vector for each node
  std::vector<long> adj_list_null; //specific adj list for node -1, since it can't be indexed directly

  //make the adjacency list

  for (long i = 0; i < (long)fat.size(); i++) { //for each element in the FAT
    if (fat[i] == -1)
      adj_list_null.push_back(i);
    else
      adj_list[fat[i]].push_back(i); //push back the relevant edge between i (from) and fat[i] (to)
  }

  //declare paths variable to store all found paths from DFS
  std::vector<std::vector<long>> paths;
  std::vector<long> path;

  //start DFS for all in adj_list_null
  for (long i : adj_list_null) {
    if (adj_list[i].size() == 0) {
      path.push_back(i);
      paths.push_back(path);
      //check for maxlength
      if (maxPathLength < (int)path.size())
        maxPathLength = (int)path.size();
      path.pop_back();
    }
    else {
      path.push_back(i);
      DFSPathFinder(i, path, paths, adj_list);
      path.pop_back();
    }
  }

  std::vector<long> results;

  //scan through all paths, get last value of all length == maxPathLength
  for (std::vector<long> i : paths) {
    if ((int)i.size() == maxPathLength)
      results.push_back(i.back());
  }

  //for (long i : adj_list_null)
  //  printf("%lu\n", i);
  std::sort(results.begin(), results.end());
  return results;
  
}