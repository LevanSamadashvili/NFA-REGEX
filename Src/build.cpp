#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <ctype.h>
using namespace std;

/* CONSTANTS */

const int NO_OP = 0;
const int UNION = 1;
const int STAR = 2;
const int CONCAT = 3;
const int CHARACTER = 4;
const int OPEN_BRACE = 5;
const int CLOSE_BRACE = 6;
const char EPSILON = '%';

const string NODE_NAME = "Node";
const string STAR_NAME = "Star";
const string UNION_NAME = "Union";
const string CONCAT_NAME = "Concat";
const string SYMBOL_NAME = "Symbol";

/* CLASSES */

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

class Node {
    public:
        string name;

        Node() {
            name = NODE_NAME;
        }
};

class Star:public Node {
    public:
        Node* child;

        Star() {
            name = STAR_NAME;
        }

        Star(Node* child) {
            name = STAR_NAME;
            this->child = child;
        }
};

class Union:public Node {
    public:
        Node* left;
        Node* right;

        Union() {
            name = UNION_NAME;
        }

        Union(Node* left, Node* right) {
            name = UNION_NAME;
            this->left = left;
            this->right = right;
        }
};

class Concat:public Node {
    public:
        Node* left;
        Node* right;

        Concat() {
            name = CONCAT_NAME;
        }

        Concat(Node* left, Node* right) {
            name = CONCAT_NAME;
            this->left = left;
            this->right = right;
        }
};

class Symbol:public Node {
    public:
        char symbol;

        Symbol() {
            name = SYMBOL_NAME;
        }

        Symbol(char symbol) {
            name = SYMBOL_NAME;
            this->symbol = symbol;
        }
};

/* FUNCTION PROTOTYPES */

int getType(char ch);
Node* parseStar(string& regex, Node* node, int& idx);
Node* parseCharacter(string& regex, int& idx);
Node* parseConcat(string& regex, Node* node, int& idx);
Node* parseUnion(string& regex, Node* node, int& idx);
Node* buildTree(string& regex, int& idx);
void printTree(Node* root, int num_tabs = 0);
NFA* buildNFA(string regex);
void printNFA(NFA* nfa);
NFA* buildNodeNFA(Node* root);
NFA* buildSymbolNFA(Symbol* node);
NFA* buildConcatNFA(Concat* node);
NFA* buildUnionNFA(Union* node);
NFA* buildStarNFA(Star* node);
void getReachableStates(NFA* nfa, int start, set<int>& result, bool* visited);
bool hasTransition(vector<Transition>& transitions, Transition& trans);
NFA* removeExcessStates(NFA* nfa); 

int main() {
    string regex;
    cin >> regex;

    NFA* nfa;
    nfa = buildNFA(regex);
    printNFA(nfa);

    return 0;
}

// Builds NFA for given regex with epsilons
NFA* buildNFA(string regex) {
    int start = 0;
    Node* root = buildTree(regex, start);

    // printf("\n\nFIIIINAAAAL RESULTTT \n\n");
    // printTree(root);

    NFA* nfa = buildNodeNFA(root);
    nfa = removeExcessStates(nfa);
    return nfa;
}

// Builds regex tree from given regex pattern
Node* buildTree(string& regex, int& idx) {
    Node* result = NULL;
    Node* new_node = NULL;

    while (idx < regex.length()) {
        int cur_ch = regex[idx];
        int cur_type = getType(cur_ch);

        switch (cur_type) {
            case OPEN_BRACE:
                new_node = buildTree(regex, ++idx);
                new_node = parseStar(regex, new_node, idx);
                new_node = parseConcat(regex, new_node, idx);       

                if (result == NULL) {
                    result = new_node;
                } else {
                    result = new Concat(result, new_node);
                }
                break;
            case CLOSE_BRACE:
                if (getType(regex[idx - 1]) == OPEN_BRACE) {
                    new_node = new Symbol(EPSILON);
                }

                if (result == NULL) {
                    result = new_node;
                }

                return result;
            case CHARACTER:
                new_node = parseCharacter(regex, idx);
                new_node = parseConcat(regex, new_node, idx);

                if (result == NULL) {
                    result = new_node;
                } else {
                    result = new Concat(result, new_node);
                }
                break;    
            case UNION:
                idx--;
                break;        
            default:
                break;
        }

        result = parseUnion(regex, result, idx);
        idx++;
    }

    result = parseStar(regex, result, idx);
    result = parseConcat(regex, result, idx);
    return result;
}

// Gets the type of the current character
int getType(char ch) {
    if (ch == '*') {
        return STAR;
    }

    if (ch == '|') {
        return UNION;
    }

    if (ch == '(') {
        return OPEN_BRACE;
    }

    if (ch == ')') {
        return CLOSE_BRACE;
    }

    return CHARACTER;
}   

// Checks if next is star and applies star operation to this node
// leaves idx where it stopped parsing
// ab*mm
// starts at b, ends at *
Node* parseStar(string& regex, Node* node, int& idx) {
    int tmp = idx + 1;
    bool was_star = false;

    while (tmp < regex.length() && getType(regex[tmp]) == STAR) {
        was_star = true;
        tmp++;
    }
    
    if (was_star && node->name != STAR_NAME) {
        node = new Star(node);
        tmp--;
        idx = tmp;
    }

    return node;
}

// Parses character 
Node* parseCharacter(string& regex, int& idx) {
    Node* result = new Symbol(regex[idx]);
    result = parseStar(regex, result, idx);
    return result;
}

// Parses concat
Node* parseConcat(string& regex, Node* node, int& idx) {
    Node* result = node;
    Node* new_node = NULL;

    while (idx < regex.length() - 1) {
        idx++;
        int type = getType(regex[idx]);
        bool do_break = false;

        switch (type) {
            case OPEN_BRACE:
                new_node = buildTree(regex, ++idx);
                new_node = parseStar(regex, new_node, idx);
                new_node = parseConcat(regex, new_node, idx);
                result = new Concat(result, new_node);
                break;
            case CHARACTER:
                new_node = parseCharacter(regex, idx);
                result = new Concat(result, new_node);
                break;
            default:
                do_break = true;
                break;
        }

        if (do_break) {
            idx--;
            break;
        }
    }

    return result;
}


// Parses union
Node* parseUnion(string& regex, Node* node, int& idx) {
    Node* new_node = NULL;
    if (idx < (int)regex.length() - 2) {
        if (getType(regex[idx + 1]) == UNION) {
            idx += 2;
            int type = getType(regex[idx]);

            if (type == CHARACTER) {
                new_node = new Symbol(regex[idx]);
            } else if (type == OPEN_BRACE) {
                new_node = buildTree(regex, ++idx);
            }

            new_node = parseStar(regex, new_node, idx);
            new_node = parseConcat(regex, new_node, idx);
            node = new Union(node, new_node);
        }
    }

    return node;
}


// Prints regex tree
void printTree(Node* root, int num_tabs) {
    string name = root->name;
    
    for (int i = 0; i < num_tabs; i++) {
        printf("\t");
    }

    printf("%s", name.c_str());

    if (name != SYMBOL_NAME) {
        printf("\n");

        if (name == UNION_NAME) {
            printTree(((Union*)root)->left, num_tabs + 1);
            printTree(((Union*)root)->right, num_tabs + 1);
        } else if (name == STAR_NAME) {
            printTree(((Star*)root)->child, num_tabs + 1);
        } else if (name == CONCAT_NAME) {
            printTree(((Concat*)root)->left, num_tabs + 1);
            printTree(((Concat*)root)->right, num_tabs + 1);
        }
    } else {
        printf(" %c\n", ((Symbol*)root)->symbol);
    }
}


// Builds appropriate NFA from regex tree
NFA* buildNodeNFA(Node* root) {
    string root_name = root->name;
    NFA* result = NULL;

    if (root_name == SYMBOL_NAME) {
        result = buildSymbolNFA((Symbol*)root);
    } else if (root_name == CONCAT_NAME) {
        result = buildConcatNFA((Concat*)root);
    } else if (root_name == UNION_NAME) {
        result = buildUnionNFA((Union*)root);
    } else if (root_name == STAR_NAME) {
        result = buildStarNFA((Star*)root);
    }

    return result;
}

// Builds new NFA which recognizes a symbol
NFA* buildSymbolNFA(Symbol* node) {
    int a, n, t;
    n = 2;
    a = 1;
    t = 1;

    vector<int> accept_states;
    accept_states.push_back(1);

    vector< vector<Transition> > transitions(n);
    Transition transition(node->symbol, 1);
    transitions[0].push_back(transition);

    NFA* result = NULL;

    if (transition.symbol == EPSILON) {
        vector<int> accept_states_new(1, 0);
        n = 1;
        a = 1;
        t = 0;
        vector< vector<Transition> > transitions_new(n);
        result = new NFA(n, a, t, accept_states_new, transitions_new);
    } else {
        result = new NFA(n, a, t, accept_states, transitions);
    }

    return result;
}

// Builds new NFA which recognizes concatenation
NFA* buildConcatNFA(Concat* node) {
    NFA* leftNFA = buildNodeNFA(node->left);
    NFA* rightNFA = buildNodeNFA(node->right);
    
    int n, a, t;
    n = leftNFA->n + rightNFA->n;
    a = rightNFA->a;
    t = leftNFA->t + rightNFA->t;

    vector<int> accept_states;
    vector< vector<Transition> > transitions(n);

    for (int i = 0; i < rightNFA->a; i++) {
        int state = rightNFA->accept_states[i] + leftNFA->n;
        accept_states.push_back(state);
    } 

    for (int i = 0; i < leftNFA->n; i++) {
        transitions[i] = leftNFA->transitions[i];
    }

    for (int i = 0; i < rightNFA->n; i++) {
        vector<Transition> new_transitions = rightNFA->transitions[i];
        for (int j = 0; j < new_transitions.size(); j++) {
            new_transitions[j].state += leftNFA->n;
        }

        transitions[i + leftNFA->n] = new_transitions;
    }

    for (int i = 0; i < leftNFA->a; i++) {
        int accept = leftNFA->accept_states[i];
        
        for (int j = 0; j < transitions[leftNFA->n].size(); j++) {
            Transition trans = transitions[leftNFA->n][j];
            transitions[accept].push_back(trans);
            t++;
        }

        if (rightNFA->is_accept[0]) {
            accept_states.push_back(accept);
            a++;
        }
    }

    NFA* result = new NFA(n, a, t, accept_states, transitions);

    delete leftNFA;
    delete rightNFA;
    return result;
}

// Builds new NFA which recognizes union
NFA* buildUnionNFA(Union* node) {
    NFA* leftNFA = buildNodeNFA(node->left);
    NFA* rightNFA = buildNodeNFA(node->right);
    
    int n, a, t;
    n = leftNFA->n + rightNFA->n + 1;
    a = leftNFA->a + rightNFA->a;
    t = leftNFA->t + rightNFA->t;

    vector<int> accept_states;
    vector< vector<Transition> > transitions(n);

    for (int i = 0; i < leftNFA->a; i++) {
        int state = leftNFA->accept_states[i];
        accept_states.push_back(state + 1);
    }

    for (int i = 0; i < rightNFA->a; i++) {
        int state = rightNFA->accept_states[i];
        accept_states.push_back(state + leftNFA->n + 1);
    }

    for (int i = 0; i < leftNFA->transitions[0].size(); i++) {
        Transition trans = leftNFA->transitions[0][i];
        trans.state++;

        transitions[0].push_back(trans);
        t++;
    }

    for (int i = 0; i < rightNFA->transitions[0].size(); i++) {
        Transition trans = rightNFA->transitions[0][i];
        trans.state += (1 + leftNFA->n);

        transitions[0].push_back(trans);
        t++;
    }

    if (leftNFA->is_accept[0] || rightNFA->is_accept[0]) {
        accept_states.push_back(0);
        a++;
    }

    for (int i = 1; i <= leftNFA->n; i++) {
        vector<Transition> new_transitions = leftNFA->transitions[i - 1];
        for (int j = 0; j < new_transitions.size(); j++) {
            new_transitions[j].state++;
        }

        transitions[i] = new_transitions;
    }

    for (int i = leftNFA->n + 1; i <= leftNFA->n + rightNFA->n; i++) {
        vector<Transition> new_transitions = rightNFA->transitions[i - leftNFA->n - 1];
        for (int j = 0; j < new_transitions.size(); j++) {
            new_transitions[j].state += (leftNFA->n + 1);
        }

        transitions[i] = new_transitions;
    }

    NFA* result = new NFA(n, a, t, accept_states, transitions);
    delete leftNFA;
    delete rightNFA;
    return result;
}

// Builds new NFA which recognizes star operation
NFA* buildStarNFA(Star* node) {
    NFA* child = buildNodeNFA(node->child);

    int n, a, t;
    n = child->n;
    t = child->t;
    
    vector<int> accept_states;
    if (!child->is_accept[0]) {
        accept_states.push_back(0);
        a = child->a + 1;
    } else {
        a = child->a;
    }

    for (int i = 0; i < child->accept_states.size(); i++) {
        accept_states.push_back(child->accept_states[i]);
    }

    vector< vector<Transition> > transitions(n);
    for (int i = 0; i < child->transitions.size(); i++) {
        transitions[i] = child->transitions[i];
    }

    for (int i = 0; i < child->accept_states.size(); i++) {
        int accept = child->accept_states[i];

        for (int j = 0; j < child->transitions[0].size(); j++) {
            Transition trans = child->transitions[0][j];
            if (!hasTransition(transitions[accept], trans)) {
                transitions[accept].push_back(trans);
                t++;
            }
        }
    }

    NFA* result = new NFA(n, a, t, accept_states, transitions);

    delete child;
    return result;
}

NFA* removeExcessStates(NFA* nfa) {
    int n, a, t;
    n = nfa->n;
    a = nfa->a;
    t = nfa->t;

    vector<int> accept_states = nfa->accept_states;
    vector <vector<Transition> > transitions = nfa->transitions;
    
    set<int> reachable_states;
    bool visited[nfa->n] = { false };
    getReachableStates(nfa, 0, reachable_states, visited);

    for (int i = n - 1; i >= 0; i--) {
        if (reachable_states.find(i) == reachable_states.end()) {
            t -= transitions[i].size();
            transitions.erase(transitions.begin() + i);
            n--;

            if (nfa->is_accept[i]) {
                int idx = -1;
                for (int j = accept_states.size() - 1; j >= 0; j--) {
                    if (accept_states[j] == i) {
                        idx = j;
                        break;
                    }
                }

                if (idx != -1) {
                    accept_states.erase(accept_states.begin() + idx);
                    a--;
                }
            }

            for (int j = 0; j < n; j++) {
                for (int k = 0; k < transitions[j].size(); k++) {
                    if (transitions[j][k].state > i) {
                        transitions[j][k].state--;
                    }
                }
            }

            for (int j = 0; j < accept_states.size(); j++) {
                if (accept_states[j] > i) {
                    accept_states[j]--;
                }
            }
        }
    }

    NFA* result = new NFA(n, a, t, accept_states, transitions);
    delete nfa;
    return result;
}

// Returns true if transitions contains trans
bool hasTransition(vector<Transition>& transitions, Transition& trans) {
    for (int i = 0; i < transitions.size(); i++) {
        if (transitions[i].symbol == trans.symbol && transitions[i].state == trans.state) {
            return true;
        }
    }

    return false;
}

// Gets every reachable state from start
void getReachableStates(NFA* nfa, int start, set<int>& result, bool* visited) {
    visited[start] = true;
    result.insert(start);

    for (int i = 0; i < nfa->transitions[start].size(); i++) {
        Transition trans = nfa->transitions[start][i];

        if (!visited[trans.state]) {
            set<int> new_result;
            getReachableStates(nfa, trans.state, new_result, visited);

            for (set<int>::iterator it = new_result.begin(); it != new_result.end(); it++) {
                result.insert(*it);
            }
        }
    }
}

// Prints given NFA
void printNFA(NFA* nfa) {
    printf("%d %d %d\n", nfa->n, nfa->a, nfa->t);

    for (int i = 0; i < nfa->a; i++) {
        int state = nfa->accept_states[i];
        printf("%d ", state);
    }
    printf("\n");

    for (int i = 0; i < nfa->n; i++) {
        printf("%d ", nfa->transitions[i].size());

        for (int j = 0; j < nfa->transitions[i].size(); j++) {
            Transition transition = nfa->transitions[i][j];
            printf("%c %d ", transition.symbol, transition.state);
        }

        printf("\n");
    }
}