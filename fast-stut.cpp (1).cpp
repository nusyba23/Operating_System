//Name:Nusyba Shifa
//Source of code used from https://csgit.ucalgary.ca/jwhudson/cpsc457w24


#include <unistd.h> 
#include <stdio.h> 
#include <ctype.h>  
#include <string>   
#include <vector>   

// Function that spit a string intowords bsed on whitespace
std::vector<std::string> split(const std::string &p_line) {
    auto line = p_line + " "; 
    bool in_str = false; 
    std::string curr_word = ""; // String holding the current word
    std::vector<std::string> res; 
    for (auto c : line) { 
        if (isspace(c)) { 
            if (in_str) res.push_back(curr_word); 
            in_str = false; // Resetting flg as we are now outside a word
            curr_word = ""; 
        } else {
            curr_word.push_back(c); 
            in_str = true; 
        }
    }
    return res; 
}

//setting bufr_size to 1MB
const int BUFFER_SIZE = 1024 * 1024;
unsigned char buffer[BUFFER_SIZE]; 
int buff_size = 0; 
int buff_pos = 0;  

//Function to read one character from stdin using bufferd reading
int buffered_read_char() {
    if (buff_pos >= buff_size) {
        buff_size = read(STDIN_FILENO, buffer, sizeof(buffer)); 
        if (buff_size <= 0) return -1; 
        buff_pos = 0; 
    }
    return buffer[buff_pos++]; //Return next character and increment the position
}

//Function to read a line from stdin using the buffer
std::string stdin_readline() {
    std::string result; 
    int char_read; 
    while ((char_read = buffered_read_char()) != -1) { 
        result.push_back(char_read); 
        if (char_read == '\n') break; 
    }
    return result; 
}

// Function to check if a word is a stutter
bool is_stutter(const std::string &s) {
    if (s.size() % 2 != 0) return false; 
    for (size_t i = 0; i < s.size() / 2; i++) 
        if (tolower(s[i]) != tolower(s[i + s.size() / 2])) 
            return false;
    return true; 
}

//Function finding the longest stutter in input
std::string get_longest_stutter() {
    std::string max_stut; 
    while (1) {
        std::string line = stdin_readline(); 
        if (line.empty()) break;  
        auto words = split(line); 
        for (auto &word : words) {                                        
            if (word.size() > max_stut.size() && is_stutter(word)) {       
                max_stut = word; 
            }
        }
    }
    return max_stut;
}

int main() {
    std::string max_stut = get_longest_stutter();           
    printf("Longest stutter: %s\n", max_stut.c_str());     
    return 0; 
}
