#include <iostream>
#include <cmath>
#include <string>

#include "utils.h"

using namespace std;

int isNumber(char*);

int assign_int_value(int&, string, char*);

void get_parameters(int argc, char** argv, string& inputFile, string& outputFile, string& queryFile, int& k, int& L, int& N, int& R) 
{
    for(int i = 1; i < argc; i+= 2) {
        char currPar = argv[i][1];

        switch(currPar) {
            case 'd': {
                if(!inputFile.empty()){
                    printf("Parameter error! Parameter [-d] specified twice!\n");
                    return;
                }

                inputFile = argv[i + 1];
                break;
            }
            case 'q': {
                if(!queryFile.empty()){
                    printf("Parameter error! Parameter [-q] specified twice!\n");
                    return;
                }

                queryFile = argv[i + 1];
                break;
            }
            case 'o': {
                if(!outputFile.empty()){
                    printf("Parameter error! Parameter [-o] specified twice!\n");
                    return;
                }

                outputFile = argv[i + 1];
                break;
            }
            case 'k': {
                if(assign_int_value(k, "k", argv[i+1]) == -1) {
                    return;
                }
                break;
            }
            case 'L': {
                if(assign_int_value(L, "L", argv[i+1]) == -1) {
                    return;
                }
                break;
            }
            case 'N': {
                if(assign_int_value(N, "N", argv[i+1]) == -1) {
                    return;
                }
                break;
            }
            case 'R': {
                if(assign_int_value(R, "R", argv[i+1]) == -1) {
                    return;
                }
                break;
            }
            default: {
                printf("Parameter error! Unknown parameter [%s]!\n", argv[i]);
                return;
            }
        }
    }

    return;
}

int assign_int_value(int& par, string parName, char* val) 
{
    if(par != 0) {
        cout << "Parameter error! Parameter [-"  << parName << "] specified twice!\n" << endl;
        return -1;
    }

    if(!isNumber(val)) {
        cout << "Parameter error! Value for parameter [-"  << parName << "] is not a number!\n" << endl;
        return -1;
    }

    if(atoi(val) <= 0) {
        cout << "Parameter error! Value for parameter [-"  << parName << "] is negative!\n" << endl;
        return -1;
    }

    par = atoi(val);

    return 0;
}

int isNumber(char* str) 
{
    char* temp = str;
    char ch = *temp;
    if(ch == '-') // negative number is provided
        ch = *(++temp);
    while(ch != '\0'){
        if(ch >= 48 && ch <= 57){
            ch = *(++temp);
        }
        else{
            return 0;
        }
    }
    return 1;
}

/* Debug */

void print_parameters(string& inputFile, string& outputFile, string& queryFile, int& k, int& L, int& N, int& R) 
{
    cout << "\n\nParameters Given:" << endl;
    cout << "................." << endl;
    cout << ">Input file: " << inputFile << endl;
    cout << ">Query File: " << queryFile << endl;
    cout << ">Output File: " << outputFile << endl;
    cout << ">Number Of Hash Functions(k): " << k << endl;
    cout << ">Number Of Hash Tables(L): " << L << endl;
    cout << ">Number Of Nearest Neighbors To Be Found(N): " << N << endl;
    cout << ">Range(R): " << R << endl;
    cout << "\n" << endl;
}