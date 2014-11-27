#include <cstdio>
#include <vector>
#include <cstdlib>
#include "perfectHashing.h"
#include "tests.h"


int main(int argc, char **argv)
{
    unsigned int testType = atoi(argv[1]);
    unsigned int testCount = atoi(argv[2]);
    unsigned int maxNumberOfElements = atoi(argv[3]);
    unsigned int maxNumberOfQueries;
    
    if (argc > 4)
    {
        maxNumberOfQueries = atoi(argv[4]);
    }
    
    
    NPerfectHash::PerfectHashSet FKS;
    NPerfectHashTests::WorkingSet stdSet;
    NPerfectHashTests::ITest *testCase;
    
    switch (testType)
    {
        case 0U:
            testCase = new NPerfectHashTests::RandomUniqueSeqAndCorectRandomQueriesTest(testCount, maxNumberOfElements + 1U, maxNumberOfQueries + 1U);
        break; case 1U:  
            testCase = new NPerfectHashTests::PermutationTest(maxNumberOfElements);
        break;
    }
    NPerfectHashTests::test(*testCase, FKS, stdSet);
    delete testCase;
    return 0;
}
    
    