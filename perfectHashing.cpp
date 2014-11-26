#include <cstdio>
#include <vector>
#include <cstdlib>
#include "perfectHashing.h"
#include "tests.h"

int main(int argc, char **argv)
{
    unsigned int testCount = atoi(argv[1]);
    unsigned int maxNumberOfElements = atoi(argv[2]);
    unsigned int maxNumberOfQueries = atoi(argv[3]);
    
    for (unsigned int testNumber = 0; testNumber != testCount; ++testNumber)
    {
        NPerfectHash::PerfectHashSet FKS;
        NPerfectHashTests::WorkingSet stdSet;
        NPerfectHashTests::RandomUniqueSeqAndCorectRandomQueriesTest testCase(maxNumberOfElements + 1, maxNumberOfQueries + 1);
        NPerfectHashTests::test(testCase, FKS, stdSet, testNumber);
    }
    return 0;
}
    
    