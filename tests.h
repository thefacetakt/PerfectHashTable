#ifndef _PERFECT_HASH_TESTS
#define _PERFECT_HASH_TESTS

#include <cstdio>
#include <set>
#include <vector>
#include <algorithm>
#include <climits>
#include "testlib.h"
#include "perfectHashing.h"

namespace NPerfectHashTests
{
    enum EActionType
    {
        INSERT,
        ERASE,
        FIND,
        IS_POSSIBLE,
        SIZE,
        EXIT
    };
    unsigned int const EActionTypeSize = 6;
    
    class WorkingSet: public NPerfectHash::ISet
    {
        
        std::vector <unsigned int> possibleElements;
        std::set<unsigned int> currentQuery;
    public:
        void init(std::vector<unsigned int> const &elements)
        {
            possibleElements = elements;
            std::sort(possibleElements.begin(), possibleElements.end());
            for (unsigned int i = 1; i < possibleElements.size(); ++i)
            {
                if (possibleElements[i] == possibleElements[i - 1])
                {
                    throw NPerfectHash::EqualElementsException(possibleElements[i]);
                }
            }
            currentQuery.clear();
        }
        
        bool isPossible(unsigned int element) const
        {
            return std::binary_search(possibleElements.begin(), possibleElements.end(), element);
        }
        
        void checkPossibility(unsigned int element) const
        {
            if (!isPossible(element))
            {
                throw NPerfectHash::ImpossibleElementException(element);
            }
        }
        
        void insert(unsigned int element)
        {
            checkPossibility(element);
            currentQuery.insert(element);
        }
        
        void erase(unsigned int element)
        {
            checkPossibility(element);
            currentQuery.erase(element);
        }
        
        bool find(unsigned int element) const
        {
            checkPossibility(element);
            return (currentQuery.find(element) != currentQuery.end());
        }
        
        unsigned int size() const
        {
            return currentQuery.size();
        }
    };

    class ITest
    {
    public:
        virtual std::vector<unsigned int> const &getAvalibleElements() const = 0;
        virtual std::pair<EActionType, unsigned int> nextQuery() = 0;
        virtual unsigned int getNumberOfQueries() const = 0;
        virtual unsigned int getNumberOfElements() const = 0;
        
    };
    
    class RandomUniqueSeqAndCorectRandomQueriesTest: public ITest
    {
        std::vector <unsigned int> avalibleElements;
        unsigned int numberOfQueries;
        unsigned int calls;
    public:
        RandomUniqueSeqAndCorectRandomQueriesTest(unsigned int maxNumberOfElements, unsigned int maxNumberOfQueries)
        {
            calls = 0U;
            unsigned int n = rnd.next(0U, maxNumberOfElements);
            avalibleElements.resize(n);
            for (unsigned int i = 0U; i < n; ++i)
                avalibleElements[i] = rnd.next(0U, UINT_MAX);
            std::sort(avalibleElements.begin(), avalibleElements.end());
            avalibleElements.resize(std::distance(avalibleElements.begin(), std::unique(avalibleElements.begin(), avalibleElements.end())));
            numberOfQueries = rnd.next(0U, maxNumberOfQueries);
        }
        
        std::vector<unsigned int> const &getAvalibleElements() const
        {
            return avalibleElements;
        }
        
        std::pair<EActionType, unsigned int> nextQuery()
        {
            if (++calls == numberOfQueries)
            {
                return std::make_pair(EXIT, 0U);
            }
            return std::make_pair(EActionType(rnd.next(0U, EActionTypeSize - 1U)), avalibleElements[rnd.next(0U, static_cast<unsigned int>(avalibleElements.size()) - 1U)]);
        }

        unsigned int getNumberOfElements() const 
        {
            return avalibleElements.size();
        }
        
        unsigned int getNumberOfQueries() const
        {
            return numberOfQueries;
        }
    };
    
    void test(ITest &testCase, NPerfectHash::ISet &firstSet, NPerfectHash::ISet &secondSet, unsigned int testNumber)
    {
        firstSet.init(testCase.getAvalibleElements());
        secondSet.init(testCase.getAvalibleElements());
        
        do
        {
            std::pair<EActionType, unsigned int> currentQuery = testCase.nextQuery();
            switch (currentQuery.first)
            {
                case INSERT:
                    firstSet.insert(currentQuery.second);
                    secondSet.insert(currentQuery.second);
                break; case ERASE:
                    firstSet.erase(currentQuery.second);
                    secondSet.erase(currentQuery.second);
                break; case FIND:
                    if (firstSet.find(currentQuery.second) != secondSet.find(currentQuery.second))
                    {
                        return void(printf("WRONG ANSWER TEST %u: %u SEARCH MISMATCH\n", testNumber, currentQuery.second));
                    }
                break; case IS_POSSIBLE:
                    if (firstSet.isPossible(currentQuery.second) != secondSet.isPossible(currentQuery.second))
                    {
                        return void(printf("WRONG ANSWER TEST %u: %u POSSIBILITY MISMATCH\n", testNumber, currentQuery.second));
                    }
                break; case SIZE:
                    if (firstSet.size() != secondSet.size())
                    {
                        return void(printf("WRONG ANSWER TEST %u: SIZE MISMATCH\n", testNumber));
                    }
                break; case EXIT:
                    printf("OK TEST %u: %u elements %u queries\n", testNumber, testCase.getNumberOfElements(), testCase.getNumberOfQueries());
                    return;
            }
        }
        while (true);
    }
};
#endif