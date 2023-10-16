#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>   //can be removed

#include "utils.h"

#define DEFAULT_K 4
#define DEFAULT_L 5
#define DEFAULT_N 1
#define DEFAULT_R 10000

using namespace std;

int main(int argc, char* argv[])
{
    /* Non-initialized args */
    int k = 0;  /* no. of hash functions per g function */
    int L = 0;  /* no. of hash tables */
    int N = 0;  /* no. of neighbors to be found */
    int R = 0;  /* range */

    string inputFile = "";
    string outputFile = "";
    string queryFile = "";

    ifstream input;
    ifstream query;
    ofstream output;

    get_parameters(argc, argv, inputFile, outputFile, queryFile, k, L, N, R);

    if(k == 0)
        k = DEFAULT_K;
    
    if(L == 0)
        L = DEFAULT_L;
    
    if(N == 0)
        N = DEFAULT_N;

    if(R == 0)
        R = DEFAULT_R;
    
    int flag = -1;

    /* While user decides to exit */
    while(flag != 8){

        /* Get input file */
        while(1){

            /* Check if input file was provided by parameters */
            /* Also in case of re-execution, check if user wants another file to be used */
            if(inputFile.empty() || (flag != -1 && (!input.is_open()) ) ){
                cout << "Please provide path to input file(dataset), or 'exit' to abort: ";
                fflush(stdout);
                getline(cin, inputFile);
                fflush(stdin);
            }

            /* Abort */
            if(!inputFile.compare("exit")){ 
                cout << "No file was given. Abort." << endl;
                return 0;
            }

            /* Check if file exists */
            struct stat buffer;
            if(stat(inputFile.c_str(), &buffer) != 0){
                cout << "File " << inputFile << " does not exist. Try again." << endl;
                inputFile = "";
                continue;
            }

            if(input.is_open()) // close if file was opened in previous execution
                input.close();

            input.open(inputFile); // open file provided 
            if(input.is_open()) // file was succesfully opened
                break;
        }

        /* Get query file */
        while(1){
            
            /* Check if query file was provided by parameters */
            if(queryFile.empty() || (flag != -1 && (!query.is_open()))){
                cout << "Please provide path to query file, or 'exit' to abort: ";
                fflush(stdout);
                getline(cin, queryFile);
                fflush(stdin);
            }

            /* Abort */
            if(!queryFile.compare("exit")){ 
                cout << "No file was given. Abort." << endl;
                return 0;
            }

            /* Check if file exists */
            struct stat buffer;
            if(stat(queryFile.c_str(), &buffer) != 0){
                cout << "File " << queryFile << " does not exist. Try again." << endl;
                queryFile = "";
                continue;
            }

            if(query.is_open()) // close if file was opened in previous execution
                query.close();
            
            query.open(queryFile); // open file provided 
            if(query.is_open()) // file was succesfully opened
                break;
        }

        /* Get output file */
        while(1){ // until correct file is given

            /* Check if output file was provided by parameters */
            if(outputFile.empty() || (flag != -1 && (!output.is_open()))){
                cout << "Please provide path to output file, or 'exit' to abort: ";
                fflush(stdout);
                getline(cin, outputFile);
                fflush(stdin);
            }

             /* Abort */
            if(!outputFile.compare("exit")){
                cout << "No file was given. Abort." << endl;
                return 0;
            }

            if(output.is_open()) // close if file was opened in previous execution
                output.close();

            output.open(outputFile); // open file provided 
            if(output.is_open()) // file was succesfully opened
                break;
        }

        /* Print Program Parameteres */
        print_parameters(inputFile, outputFile, queryFile, k, L, N, R);

    }
    cout << "Exiting..." << endl;
}