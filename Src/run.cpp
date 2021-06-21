#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <unordered_set>
#include <ctype.h>
using namespace std;

struct Transition {
    char symbol;
    int state;

    Transition(char symbol, int state) {
        this->symbol = symbol;
        this->state = state;
    }
};


class NFA {
    public:
        int n; // number of states
        int a; // number of accept states
        int t; // number of transitions

        vector< vector<Transition> > transitions;
        vector<int> accept_states;
        vector<bool> is_accept;

        NFA() {}

        NFA(int n, int a, int t, vector<int>& accept_states, vector< vector<Transition> >& transitions) {
            this->n = n;
            this->a = a;
            this->t = t;
            this->accept_states = accept_states;
            this->transitions = transitions;

            this->is_accept = vector<bool>(n, false);
            for (int i = 0; i < this->accept_states.size(); i++) {
                this->is_accept[this->accept_states[i]] = true;
            }
        }
};


/* Function Prototypes */

NFA* readNFA();
void simulateNFA(NFA*, string& input, string& result);

int main() {
    string input;
    cin >> input;

    NFA* nfa = readNFA();
    string result = "";
    simulateNFA(nfa, input, result);
    cout << result << endl;
}

// Reads NFA from the console
NFA* readNFA() {
    int n, a, t;
    cin >> n >> a >> t;

    NFA* nfa = new NFA();
    nfa->n = n;
    nfa->a = a;
    nfa->t = t;

    for (int i = 0; i < a; i++) {
        int accept_state;
        cin >> accept_state;
        nfa->accept_states.push_back(accept_state);
    }

    nfa->transitions = vector< vector<Transition> >(n);

    // 0 - 25 == a - z
    // 26 - 35 == 0 - 9

    for (int i = 0; i < n; i++) {
        int num_transitions;
        cin >> num_transitions;

        for (int j = 0; j < num_transitions; j++) {
            char symbol;
            int state;

            cin >> symbol >> state;
            nfa->transitions[i].push_back(Transition(symbol, state));
        }
    }

    nfa->is_accept = vector<bool>(n, false);
    for (int i = 0; i < nfa->accept_states.size(); i++) {
        nfa->is_accept[nfa->accept_states[i]] = true;
    }

    return nfa;
}


// Simulates nfa on given input
void simulateNFA(NFA* nfa, string& input, string& result) {
    unordered_set<int> cur_states;
    cur_states.insert(0);

    for (int i = 0; i < input.length(); i++) {
        unordered_set<int> next_states;
        bool was_accept = false;

        for (unordered_set<int>::iterator it = cur_states.begin(); it != cur_states.end(); it++) {
            int cur_state = *it;
            char symbol = input[i];

            for (int j = 0; j < nfa->transitions[cur_state].size(); j++) {
                Transition trans = nfa->transitions[cur_state][j];
                if (trans.symbol == input[i]) {
                    int state = trans.state;
                    next_states.insert(state);

                    if (nfa->is_accept[state]) {
                        was_accept = true;
                    }
                }
            }
        }

        if (was_accept) {
            result += 'Y';
        } else {
            result += 'N';
        }

        cur_states = next_states;
    }
}