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
        break; case 2U:
            testCase = new NPerfectHashTests::AllEqualInitTest(arguments["maxNumberOfElements"], arguments["numberOfTests"]);
        break; case 3U:
            testCase = new NPerfectHashTests::TwoElementInitTestStrongOrder(arguments["maxNumberOfElements"], arguments["numberOfTests"]);
        break; case 4U:
            testCase = new NPerfectHashTests::TwoElementInitTestRandomOrder(arguments["maxNumberOfElements"], arguments["numberOfTests"]);
        break; case 5U:
            testCase = new NPerfectHashTests::TwoElementInitTestEveryOrder(arguments["maxNumberOfElements"]);
        break; case 6U:
            testCase = new NPerfectHashTests::PairInitTest(arguments["maxNumberOfElements"], arguments["numberOfTests"]);
        break; case 7U:
            testCase = new NPerfectHashTests::PairInitRandomTest(arguments["maxNumberOfElements"], arguments["numberOfTests"]);
        break; case 8U:
            testCase = new NPerfectHashTests::BigKeyValuesAndRandomQueries(arguments["numberOfTests"], arguments["maxNumberOfElements"], arguments["maxNumberOfQueries"]);
        break; case 9U:
            testCase = new NPerfectHashTests::IsPossibleTrueTest(arguments["numberOfTests"], arguments["maxNumberOfElements"], arguments["maxNumberOfQueries"]);
        break; case 10U:
            testCase = new NPerfectHashTests::IsPossibleRandomTest(arguments["numberOfTests"], arguments["maxNumberOfElements"], arguments["maxNumberOfQueries"]);
        break; case 11U:
            testCase = new NPerfectHashTests::CleverSequenceOfQueriesTest(arguments["numberOfTests"], arguments["maxNumberOfElements"], arguments["maxNumberOfQueries"]);
        break; case 12U:
            testCase = new NPerfectHashTests::MultipleInsertTest(arguments["numberOfTests"], arguments["maxNumberOfElements"], arguments["maxNumberOfQueries"]);
        break; case 13U:
            testCase = new NPerfectHashTests::RandomKeyInsertTest(arguments["numberOfTests"], arguments["maxNumberOfElements"], arguments["maxNumberOfQueries"]);
        break;
    }
    if (arguments.find("timeMeasure") != arguments.end())
    {
        clock_t begin = clock();
        NPerfectHashTests::test(*testCase, &FKS, NULL);
        clock_t end = clock();
        printf("Execution time:          %.10lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
        printf("Average execution time:  %.10lf\n", (double)(end - begin) / CLOCKS_PER_SEC / arguments["numberOfTests"]);
    }
    else
    {
        NPerfectHashTests::test(*testCase, &FKS, &stdSet);
    }
    delete testCase;
    return 0;
}
    
    