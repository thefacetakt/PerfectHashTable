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
    
    class Event
    {
    public:
        EActionType type;
        unsigned int element;
        Event(EActionType type, unsigned int element) : type(type), element(element)
        {
        }
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
        virtual Event nextQuery() = 0;
        virtual unsigned int getNumberOfQueries() const = 0;
        virtual unsigned int getNumberOfElements() const = 0;
        virtual bool nextInitialization() = 0;
        virtual ~ITest(){}
    };
    
    class PermutationTest: public ITest
    {
        std::vector <unsigned int> avalibleElements;
    public:
        PermutationTest(unsigned int n)
        {
            avalibleElements.resize(n);
            for (unsigned int i = 0; i < n; ++i)
            {
                avalibleElements[i] = i + 1;
            }
        }
        
        std::vector <unsigned int> const &getAvalibleElements() const
        {
            return avalibleElements;
        }
        
        Event nextQuery()
        {
            return Event(EXIT, 0);
        }
        
        unsigned int getNumberOfQueries() const
        {
            return 1U;
        }
        unsigned int getNumberOfElements() const
        {
            return avalibleElements.size();
        }
        
        bool nextInitialization() 
        {            
            return std::next_permutation(avalibleElements.begin(), avalibleElements.end());
        }
    };
    
    class RandomUniqueSeqAndCorectRandomQueriesTest: public ITest
    {
        std::vector <unsigned int> avalibleElements;
        unsigned int numberOfQueries;
        unsigned int calls;
        unsigned int initialisations;
        unsigned int maxNumberOfElements;
        unsigned int maxNumberOfQueries;
        unsigned int numberOfInitializations;
        
        void init()
        {
            avalibleElements.clear();
            calls = 0U;
            unsigned int n = rnd.next(1U, maxNumberOfElements);
            avalibleElements.resize(n);
            for (unsigned int i = 0U; i < n; ++i)
                avalibleElements[i] = rnd.next(0U, UINT_MAX);
            std::sort(avalibleElements.begin(), avalibleElements.end());
            avalibleElements.resize(std::distance(avalibleElements.begin(), std::unique(avalibleElements.begin(), avalibleElements.end())));
            numberOfQueries = rnd.next(1U, maxNumberOfQueries);
        }
        
    public:
        RandomUniqueSeqAndCorectRandomQueriesTest(unsigned int numberOfTests, unsigned int maxNumberOfElements, unsigned int maxNumberOfQueries) :
            maxNumberOfElements(maxNumberOfElements), maxNumberOfQueries(maxNumberOfQueries), numberOfInitializations(numberOfTests)
        {
            initialisations = 0;
            init();
        }
        
        std::vector<unsigned int> const &getAvalibleElements() const
        {
            return avalibleElements;
        }
        
        Event nextQuery()
        {
            if (++calls == numberOfQueries)
            {
                return Event(EXIT, 0U);
            }
            return Event(EActionType(rnd.next(0U, EActionTypeSize - 1U)), avalibleElements[rnd.next(0U, static_cast<unsigned int>(avalibleElements.size()) - 1U)]);
        }

        unsigned int getNumberOfElements() const 
        {
            return avalibleElements.size();
        }
        
        unsigned int getNumberOfQueries() const
        {
            return numberOfQueries;
        }
        
        bool nextInitialization()
        {
            if (++initialisations == numberOfInitializations)
            {
                return false;
            }
            init();
            return true;
        }
    };
    
    void test(ITest &testCase, NPerfectHash::ISet &firstSet, NPerfectHash::ISet &secondSet)
    {
        unsigned int testNumber = 0;
        do 
        {
            ++testNumber;
            firstSet.init(testCase.getAvalibleElements());
            secondSet.init(testCase.getAvalibleElements());
            Event currentQuery = testCase.nextQuery();
            while (currentQuery.type != EXIT)
            {
                switch (currentQuery.type)
                {
                    case INSERT:
                        firstSet.insert(currentQuery.element);
                        secondSet.insert(currentQuery.element);
                    break; case ERASE:
                        firstSet.erase(currentQuery.element);
                        secondSet.erase(currentQuery.element);
                    break; case FIND:
                        if (firstSet.find(currentQuery.element) != secondSet.find(currentQuery.element))
                        {
                            return void(printf("-\nWRONG ANSWER TEST %u: %u SEARCH MISMATCH\n", testNumber, currentQuery.element));
                        }
                    break; case IS_POSSIBLE:
                        if (firstSet.isPossible(currentQuery.element) != secondSet.isPossible(currentQuery.element))
                        {
                            return void(printf("-\nWRONG ANSWER TEST %u: %u POSSIBILITY MISMATCH\n", testNumber, currentQuery.element));
                        }
                    break; case SIZE:
                        if (firstSet.size() != secondSet.size())
                        {
                            return void(printf("-\nWRONG ANSWER TEST %u: SIZE MISMATCH\n", testNumber));
                        }
                    break;
                }
                currentQuery = testCase.nextQuery();
            }
            printf("+");
        }
        while (testCase.nextInitialization());
        printf("\n");
    }
    
};
#endif