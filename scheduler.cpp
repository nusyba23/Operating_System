/// =========================================================================
/// Copyright (C) 2024 Jonathan Hudson (jwhudson@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the only file you should modify and submit for grading


///Modified By Nusyba Shifa (30162709)

#include "scheduler.h"
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <climits>


// Helper function to add process to sequence with compression and ensuring not to exceed max_seq_len
void addToSequence(std::vector<int>& seq, int processId, int64_t max_seq_len) {
    if (seq.size() >= max_seq_len) return; // Do not exceed max_seq_len

    if (seq.empty() || seq.back() != processId) {
        seq.push_back(processId); // Add processId to seq if it's not a duplicate of the last element
    }
}

// this is the function you should implement
//
// simulate_rr() implements a Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrivals, and bursts
// output:
//   seq[] - will contain the compressed execution sequence
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//         - sequence will be trimmed to contain only first max_seq_len entries
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields


void simulate_rr(int64_t quantum, int64_t max_seq_len, std::vector<Process> &processes, std::vector<int> &seq)
{
    seq.clear();

    std::queue<int> readyQueue;
    int64_t currentTime = 0;
    std::vector<int64_t> remainingBurstTime(processes.size());
    std::vector<bool> addedToQueue(processes.size(), false);
    int next_proc = 0;
    int prev_process = -1;

    std::vector<int64_t> wait_times(processes.size(), 0);

    // Main simulation loop
    while (true)
    {

        // Add processes that have arrived to the ready queue
        for (size_t i = next_proc; i < processes.size(); ++i)
        {
            std::cout << "In the loop, the next_process is " << i << std::endl;
            // case where the next process starts while the previous burst is taking place
            if (processes[i].arrival < currentTime)
            {
                readyQueue.push(i);
                remainingBurstTime[i] = processes[i].burst;
                std::cout << "line 489 added process: " << i << " at time: " << currentTime << std::endl;
                next_proc++;
            }

            // case where the next process starts as soon as the previous burst ends
            if (processes[i].arrival == currentTime)
            {
                // we need to requeue the old process if it has not finished yet BEFORE the next case

                if (prev_process != -1)
                {
                    std::cout << "line 497 added process: " << prev_process << " at time: " << currentTime << std::endl;

                    readyQueue.push(prev_process);
                    prev_process = -1;
                }

                readyQueue.push(i);
                remainingBurstTime[i] = processes[i].burst;
                std::cout << "line 508 added process: " << i << " at time: " << currentTime << std::endl;

                next_proc++;
            }
        }

        // case where only one process has to be requeued (for loop is skipped)
        if (prev_process != -1)
        {
            readyQueue.push(prev_process);
            std::cout << "line 518 added process: " << prev_process << " at time: " << currentTime << std::endl;

            prev_process = -1;
        }

        if (!readyQueue.empty())
        {
            int processId = readyQueue.front();
            readyQueue.pop();

            if (seq.back() != processId || seq.empty())
            {
                std::cout << "current process that has cpu: " << processId << std::endl;
                seq.push_back(processId);
            }
            if (processes[processId].start_time == -1)
            {
                processes[processId].start_time = currentTime;
            }

            // Execute process for a quantum or until completion
            int64_t executionTime = std::min(quantum, remainingBurstTime[processId]);
            remainingBurstTime[processId] -= executionTime;
            currentTime += executionTime;

            for (int i = 0; i < next_proc; i++)
            {
                if (i != processId && processes[i].finish_time == -1 && processes[i].arrival <= currentTime)
                {
                    // increase wait time here
                    wait_times[i] += executionTime;
                }
            }

            // Check for process completion
            if (remainingBurstTime[processId] > 0)
            {
                prev_process = processId;
            }
            else
            {
                // Re-add to ready queue if not completed
                processes[processId].finish_time = currentTime;
                std::cout << "burst ended for process: " << processId << " at time: " << currentTime << std::endl;
                prev_process = -1;
            }
        }
        else
        {
            // If there are no processes ready to execute, advance time
            currentTime++;
            prev_process = -1;
            if (seq.back() != -1 || seq.empty())
            {
                std::cout << "current process that has cpu: " << prev_process << std::endl;
                seq.push_back(-1);
            }

            if (readyQueue.empty() && int(processes.size()) == next_proc)
            {
                break; // End simulation if max sequence length reached
            }
        }
    }
    if (!seq.empty())
    {
        seq.pop_back();
    }
    // resize seq to max_seq_len if required
    seq.resize(std::min(max_seq_len, int64_t(seq.size())));

    int i = 0;
    for (const auto num : wait_times)
    {
        std::cout << "P" << i << " has waited " << num << std::endl;
        i++;
    }
}


// this is the function you should implement
//
// simulate_sjf() implements a Shortest Process First scheduling simulator
// input:
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrivals, and bursts
// output:
//   seq[] - will contain the compressed execution sequence
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//         - sequence will be trimmed to contain only first max_seq_len entries
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields



void simulate_sjf(int64_t max_seq_len, std::vector<Process> &processes, std::vector<int> &seq)
{
    seq.clear();

    if (processes.empty())
        return;

    int64_t currentTime = 0;
    std::vector<bool> completed(processes.size(), false);
    std::vector<int64_t> waitTimes(processes.size(), 0);
    int completedProcesses = 0;
    bool isFirstProcess = true;

    while (completedProcesses < processes.size() && seq.size() < max_seq_len)
    {
        int shortestJobIndex = -1;
        int64_t shortestBurst = INT64_MAX;

        // Prepare visualization for this time slice

        for (size_t i = 0; i < processes.size(); ++i)
        {
            if (!completed[i] && processes[i].arrival <= currentTime)
            {
                if (processes[i].burst < shortestBurst)
                {
                    shortestJobIndex = i;
                    shortestBurst = processes[i].burst;
                }
            }
        }

        // Integration of original simulation code for process selection and time management
        if (isFirstProcess && (shortestJobIndex == -1 || processes[shortestJobIndex].arrival > currentTime))
        {
            seq.push_back(-1); // Add idle time at the start if the first process does not start immediately
            std::cout << currentTime << "  " << std::endl;
            currentTime++;
            isFirstProcess = false;
            continue;
        }

        if (shortestJobIndex != -1)
        {
            // If this is not the first process and there's a gap, fill it with idle time
            if (!isFirstProcess && processes[shortestJobIndex].arrival > currentTime)
            {
                seq.push_back(-1);
                currentTime = processes[shortestJobIndex].arrival;
            }
            isFirstProcess = false;

            if (processes[shortestJobIndex].start_time == -1)
            {
                processes[shortestJobIndex].start_time = currentTime;
            }
        

            // print here for when a process is being executed
            for (int i = currentTime; i < currentTime + processes[shortestJobIndex].burst; i++)
            {
                std::cout << i << " ";
                for (int j = 0; j < shortestJobIndex; j++)
                {
                    if (processes[j].arrival <= i && processes[j].finish_time == -1)
                    {
                        std::cout << ".  ";
                        waitTimes[j]++;
                    }
                    else
                    {
                        std::cout << "   ";
                    }
                }

                std::cout << "#  ";

                for (int j = shortestJobIndex + 1; j < processes.size(); j++)
                {
                    if (processes[j].arrival <= i && processes[j].finish_time == -1)
                    {
                        std::cout << ".  ";
                        waitTimes[j]++;
                    }
                    else
                    {
                        std::cout << "   ";
                    }
                }

                std::cout << std::endl;
            }
            currentTime += processes[shortestJobIndex].burst;
            completed[shortestJobIndex] = true;
            processes[shortestJobIndex].finish_time = currentTime;
            completedProcesses++;

            seq.push_back(shortestJobIndex);
        }
        else
        {

            int64_t nextArrivalTime = INT64_MAX;
            for (size_t i = 0; i < processes.size(); ++i)
            {
                if (!completed[i] && processes[i].arrival > currentTime)
                {
                    nextArrivalTime = std::min(nextArrivalTime, processes[i].arrival);
                }
            }
            if (nextArrivalTime != INT64_MAX)
            {
               
                seq.push_back(-1); // Add idle time if we're jumping forward in time
                // }

                // ^ print here for CPU being idle
                for (int i = currentTime; i < nextArrivalTime; i++)
                {
                    std::cout << i << " ";
                    std::cout << " " << std::endl;
                }
                currentTime = nextArrivalTime;
                isFirstProcess = false;
            }
        }
    }

    std::cout << currentTime << std::endl;

    double totalWaitTime = 0;
    for (auto wait : waitTimes)
    {
        totalWaitTime += wait;
    }
    double averageWaitTime = totalWaitTime / processes.size();

    // Printing wait times for each process
    for (size_t i = 0; i < processes.size(); ++i)
    {
        std::cout << "P" << i << " waited " << waitTimes[i] << " sec.\n";
    }

    // Printing average wait time
    std::cout << "Average waiting time = " << averageWaitTime << " sec.\n";
}