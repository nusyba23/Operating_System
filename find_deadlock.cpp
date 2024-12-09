// =========================================================================
/// Copyright (C) 2024 Jonathan Hudson (jwhudson@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
///Modified by Nusyba


/// =========================================================================
/// this is the ONLY file you should edit and submit to D2L

//required headers for data structures and IO
#include "find_deadlock.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <queue>

/// Word2Int is a class that can convert words to unique integers
/// get(word) returns the same number given the same word
///           the numbers will start at 0, then 1, 2, ...
///
/// example:
///   Word2Int w2i;
///   w2i.get("hello") = 0
///   w2i.get("world") = 1
///   w2i.get("hello") = 0
///   w2i.get(0) = "hello"
///   w2i.get(1) = "world"
///
/// Word2Int "may" help you get a bit more performance
/// in your cycle finding algorithm, as arrays are faster
/// than hash tables...


//Mapping words to integers and vice versa
class Word2Int
{
    int counter = 0;
    std::unordered_map<std::string, int> myset_si; // Maps string to int
    std::unordered_map<int, std::string> myset_is;  // maps int t back to string

public:
    //Retruieve or assigning Id for a given string
    int get(const std::string &w)
    {
        auto f = myset_si.find(w);
        if (f == myset_si.end())
        {
            myset_si[w] = counter;
            myset_is[counter] = w;
            return counter++;
        }
        return f->second;
    }
    std::string get(const int i)    //retirves a string given its ID
    {
        return myset_is[i];
    }
    int getCounter()        //returns teh current number of unique string
    {
        return counter;
    }
};

Result result;
Word2Int w2i;   //instance of word2int for string-int conversions


//Calss representing tghe graph, optimized for fast deadlock detection
class FastGraph
{
public:
    std::vector<std::vector<int>> adj_list;     //Adjacency list for graph representation
    std::vector<int> out_counts;        //outgoing edge counts for each node

    //constructor initializes graph with n  nodes
    FastGraph(int n = 0) : adj_list(n), out_counts(n, 0) {}


    //adds an edge to the graph , adjusting the adjacency loist and out_counts
    void addEdge(int from, int to)
    {
        //Ensure the adjacency list and out_counts can accomodate all nodes 
        adj_list.resize(w2i.getCounter());
        out_counts.resize(w2i.getCounter());

        adj_list[to].push_back(from); // may be the other way around
        out_counts[from]++;           // increment out gouinng edge count for source node 

        // std::cout << w2i.get(from) << "->" << w2i.get(to) << std::endl;
    }

    // topological sort to detect deadlocks
    void toposort()
    {
        std::vector<int> out = out_counts;

        // Find all nodes in the graph with outdegree == 0
        std::queue<int> zeros;
        for (int i = 0; i < (int)out_counts.size(); i++)
        {
            if (out_counts[i] == 0)
            {
                zeros.push(i);
            }
        }

        // Process nodes, removing edges and checing for deadlocks 
        while (!zeros.empty())
        {
            int n = zeros.front();
            zeros.pop();

            for (const int n2 : adj_list[n])
            {
                out[n2]--;

                if (out[n2] == 0)
                {
                    zeros.push(n2);
                }
            }
        }

        //Identify any nodes still with outgoing edge , indicates a deadlock
        for (int i = 0; i < (int)out.size(); i++)
        {

            if (out[i] != 0)
            {

                // identify if the vertex is a process
                std::string process = w2i.get(i);
                if (process[0] == '+')
                {
                    result.procs.push_back(process.substr(1));
                }
            }
        }

        if (!result.procs.empty())
        {
            return;
        }
    }
};


/// this is the function you need to (re)implement
/// -------------------------------------------------------------------------
/// parameter edges[] contains a list of request- and assignment- edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge. As soon as you detect a deadlock, your function
/// needs to stop processing edges and return an instance of Result structure
/// with 'index' set to the index that caused the deadlock, and 'procs' set
/// to contain names of processes that are in the deadlock.
///
/// To indicate no deadlock was detected after processing all edges, you must
/// return 

Result find_deadlock(const std::vector<std::string> &edges)
{
    FastGraph graph;
     result.index = -1;     //initialize reslutt to -1

    for (int index = 0; index < (int)edges.size(); ++index)
    {
        auto parts = split_line(edges[index]);
        if (parts.size() != 3)      //validate edge formart
        {
            std::cerr << "Error: Invalid edge format '" << edges[index] << "'." << std::endl;
            continue;
        }
        bool isRequest = parts[1] == "->";      //detremine edge direc tion
        int from = w2i.get("+" + parts[0]), to = w2i.get(parts[2]);     //get node IDs

        if (isRequest)
        {
            graph.addEdge(from, to);        //add edge according to direction
        }
        else
        {
            graph.addEdge(to, from);
        }

        graph.toposort();   //check foor deadlocks

        if (result.procs.size() > 0)
        {

            result.index = index;
            break;
        }
    }

    return result;
}
