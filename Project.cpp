#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm> // Include for std::find
#include <stdexcept>
#include <unistd.h>
#include <iterator> // Include for std::istream_iterator

using namespace std;

class FileHandler {
public:
    vector<string> readFile(const string& filePath) {
        vector<string> lines;
        ifstream file(filePath);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                lines.push_back(line);
            }
            file.close();
        } else {
            throw invalid_argument("File could not be opened.");
        }
        return lines;
    }

    map<string, vector<string>> parseFile(const vector<string>& lines) {
        map<string, vector<string>> parsedLines;
        parsedLines["states"] = splitString(lines[0]);
        parsedLines["input_symbols"] = splitString(lines[1]);
        parsedLines["stack_symbols"] = splitString(lines[2]);
        parsedLines["initial_state"] = {lines[3]};
        parsedLines["initial_stack"] = {lines[4]};
        parsedLines["final_states"] = splitString(lines[5]);
        vector<string> productions;
        for (size_t i = 6; i < lines.size(); ++i) {
            productions.push_back(lines[i]);
        }
        parsedLines["productions"] = productions;
        return parsedLines;
    }

private:
    vector<string> splitString(const string& str) {
        istringstream iss(str);
        vector<string> tokens;
        string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }
};

class PDA {
public:
    void compute(const string& inputString, const map<string, vector<string>>& parsedLines) {
        string modifiedInputString = inputString + "e";
        string initStackSymbol = parsedLines.at("initial_stack")[0];
        stack.push_back(initStackSymbol);
        const vector<string>& finalStates = parsedLines.at("final_states");
        const string& initialState = parsedLines.at("initial_state")[0];
        const vector<string>& stackSymbols = parsedLines.at("stack_symbols");
        const vector<string>& productions = parsedLines.at("productions");

        string currentStackSymbol = initStackSymbol;
        string currentState = initialState;

        cout << "State\tInput\tStack\tMove\n";
        cout << currentState << "\t_\tZ\t(" << currentStackSymbol << ", " << stackToString(stack) << ")\n";

        for (char c : modifiedInputString) {
            for (const string& production : productions) {
                istringstream iss(production);
                vector<string> tokens(istream_iterator<string>{iss}, istream_iterator<string>());
                if (tokens[0] == currentState && tokens[1] == string(1, c) && tokens[2] == currentStackSymbol) {
                    currentState = tokens[3];
                    if (tokens[4].size() == 2) {
                        stack.push_back(string(1, c));
                    } else if (tokens[4].size() == 3) {
                        stack.push_back(string(1, c));
                        stack.push_back(string(1, c));
                    } else if (tokens[4] == "e" && stack.size() != 1) {
                        stack.pop_back();
                        break;
                    }
                }
            }
            string previousStackSymbol = currentStackSymbol;
            currentStackSymbol = stack.back();
            cout << currentState << "\t" << c << "\t" << previousStackSymbol << "\t(" << currentStackSymbol << ", " << stackToString(stack) << ")\n";
            usleep(1000000); // Sleep for 1 second
        }

        if (find(finalStates.begin(), finalStates.end(), currentState) != finalStates.end()) {
            cout << "String accepted by PDA." << endl;
        } else {
            cout << "String rejected by PDA." << endl;
        }
    }

private:
    vector<string> stack;

    string stackToString(const vector<string>& stack) {
        string result;
        for (const string& symbol : stack) {
            result += symbol;
        }
        return result;
    }
};

int main() {
    FileHandler fh;
    PDA pda;
    string automataFilePath;
    cout << "Enter the automata file path: ";
    cin >> automataFilePath; // Context Free Language
    vector<string> lines;
    try {
        lines = fh.readFile(automataFilePath); 
    } catch (const invalid_argument& e) {
        cerr << e.what() << endl;
        return 1;
    }
    cout << "Reading Automata File" << endl;
    sleep(2);
    cout << "Automata File Successfully Read" << endl;
    sleep(2);
    string inputString;
    cout << "Enter input String: ";
    cin >> inputString;
    inputString.erase(inputString.find_last_not_of(" \n\r\t") + 1);
    cout << "Loading Details from Automata File: " << endl;
    sleep(3);
    map<string, vector<string>> parsedLines = fh.parseFile(lines);
    cout << "States: ";
    for (const string& state : parsedLines["states"]) {
        cout << state << " ";
    }
    cout << endl;
    cout << "Input Symbols: ";
    for (const string& symbol : parsedLines["input_symbols"]) {
        cout << symbol << " ";
    }
    cout << endl;
    cout << "Stack Symbols: ";
    for (const string& symbol : parsedLines["stack_symbols"]) {
        cout << symbol << " ";
    }
    cout << endl;
    cout << "Initial State: " << parsedLines["initial_state"][0] << endl;
    cout << "Initial Stack Symbol: " << parsedLines["initial_stack"][0] << endl;
    cout << "Final States: ";
    for (const string& state : parsedLines["final_states"]) {
        cout << state << " ";
    }
    cout << endl;
    cout << "Productions List:" << endl;
    for (const string& production : parsedLines["productions"]) {
        cout << "\t" << production << endl;
    }
    sleep(2);
    cout << "Details loaded" << endl;
    cout << "Computing the Transition Table:" << endl;
    pda.compute(inputString, parsedLines);
    return 0;
}