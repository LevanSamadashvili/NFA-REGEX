# NFA-REGEX
- **REGEX-NFA converter** and **NFA simulator**.
- This is the demonstration of the fact that **REGEX** has equal computational power as **NFA**.

## Description
The project consists of two parts:
1) **build.cpp**: Converts **Regex**(Regular Expression) to equivalent **NFA**(Nondeterministic Finite Automaton)
2) **run.cpp**:   Simulates given NFA on given word

## Run
#### Windows
```console
.\build.bat
.\run.bat
```
#### Linux
You might need **chmod +x** command to make **build.sh** and **run.sh** executables
```console
./build.sh
./run.sh
```

If these don't run, just compile source files in Src with g++ and run.

## Details
### First Part (Regex->NFA)
##### Input Format
- You are given a regular expression, consisting of ()-braces, lowercase english letters and digits,
- operators: "|"(**Union**), "*"(**Star** operation), and **concatenation** (ab is concatenation of a and b)
- () in input means **EPSILON** symbol in Regex

Example 1:
```console
(a|b)*(c|())
```
Example 2:
```console
(ab*c(0|1)*)*
```

##### Output Format
You should output on the first line numbers: **n, a, t**.
States are numbered from 0 to n - 1
- **n** - Number of states in resulting NFA, n should be at most regex.length() + 1 
- **a** - Number of accepting states in resulting NFA;
- **t** - Number of transitions in resulting NFA;

- On the second line you should output the accepting states (each is between 0 and n - 1 inclusive).
- On the next n lines, for line i (0 <= i < n) you should output 
the number of transitions from state i, and transitions from state i
**transition is SYMBOL and STATE to where the transition goes**

Possible outputs for examples in input:

Example 1:
```console
2 2 3
0 1
3 a 0 b 0 c 1
0
```

Example 2:
```console
3 2 6
0 2
1 a 1
2 b 1 c 2
3 0 2 1 2 a 1
```

### Second Part (NFA simulation)
##### Input Format
On the first line you are given a **word** (basically a string of characters)
On the next lines you are given an **NFA** (same input format as First Part)

Example 1:
```console
aababacab
2 2 3
0 1
3 a 0 b 0 c 1
0
```

Example 2:
```console
abbc1acabbbbc001cabc
3 2 6
0 2
1 a 1
2 b 1 c 2
3 0 2 1 2 a 1
```

##### Output Format
You should output the string of 'Y'-s and 'N'-s,
- **'Y'** on the i-th index means that NFA was in an accept state after reading i-th symbol from the input
- **'N'** on the i-th index means that none of NFA-s current states were accept states after reading i-th symbol from the input

Outputs for examples in input:

Example 1:
```console
YYYYYYYNN
```

Example 2:
```console
NNNYYNYNNNNNYYYYNNNN
```
