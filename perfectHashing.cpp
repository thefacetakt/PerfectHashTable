#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include "perfectHashing.h"
#include "tests.h"
#include <iostream>

std::map<std::string, unsigned int> arguments;

void parseArguments(int argc, char **argv)
{
    unsigned int value;
    for (int i = 1; i < argc; ++i)
    {
        unsigned int indexOfEqualSymbol = 0;
        for (; argv[i][indexOfEqualSymbol] != '\0' && argv[i][indexOfEqualSymbol] != '='; ++indexOfEqualSymbol);
        if (argv[i][indexOfEqualSymbol])
        {
            argv[i][indexOfEqualSymbol] = '\0';
            sscanf(argv[i] + indexOfEqualSymbol + 1, "%u", &value);     
            arguments[argv[i] + 1] = value;
        }
        else
        {
            arguments[argv[i] + 1];
        }
    }
}


int main(int argc, char **argv)
{
    parseArguments(argc, argv);
    
    NPerfectHash::PerfectHashSet FKS;
    NPerfectHashTests::WorkingSet stdSet;
    NPerfectHashTests::ITest *testCase;

    switch (arguments["typeOfTest"])
    {
        case 0U:
            testCase = new NPerfectHashTests::RandomUniqueSeqAndCorectRandomQueriesTest(arguments["numberOfTests"], arguments["maxNumberOfElements"], arguments["maxNumberOfQueries"]);
        break; case 1U:  
            testCase = new NPerfectHashTests::PermutationTest(arguments["maxNumberOfElements"]);
        break;
    }
    if (arguments.find("timeMeasure") != arguments.end())
    {
        clock_t begin = clock();
        NPerfectHashTests::test(*testCase, &FKS, NULL);
        clock_t end = clock();
        printf("Execution time: %.10lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
    }
    else
    {
        NPerfectHashTests::test(*testCase, &FKS, &stdSet);
    }
    delete testCase;
    return 0;
}
    
    