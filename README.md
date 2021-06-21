# NFA-REGEX
REGEX-NFA converter and NFA simulator

## Description
This project consists of two parts:
1) Converts **Regex**(Regular Expression) to **NFA**(Nondeterministic Finite Automaton)
2) Simulates given NFA on given word

## Run
#### Windows
```console
.\build.bat
.\run.bat
```
#### Linux
You might need chmod +x command to make build.sh and run.sh executables
```console
./build.sh
./run.sh
```

## Details

### First Part (Regex->NFA)
##### Input Format
You are given a regular expression, consisting of ()-braces, lowercase english letters and digits,
operators: "|" , "*", and concatenation (ab is concatenation of a and b)

```console
Example 1: (a|b)*(c|())
Example 2: (ab*c(0|1)*)*
```

##### Output Format
You should output on the first line numbers: n, a, t.
States are numbered from 0 to n - 1
n - Number of states in resulting NFA;
a - Number of accepting states in resulting NFA;
t - Number of transitions in resulting NFA;

On the second line you should output the accepting states (each is between 0 and n - 1 inclusive).
On the next n lines, for line i (0 <= i < n) you should output 
the number of transitions from state i, and transitions from state i
transition is SYMBOL and STATE to where the transition goes

Possible outputs for examples in input
```console
Example 1:
2 2 3
0 1
3 a 0 b 0 c 1
0

Example 2:
3 2 6
0 2
1 a 1
2 b 1 c 2
3 0 2 1 2 a 1
```

### Second Part (NFA simulation)
