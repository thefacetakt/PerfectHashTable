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
            unsigned int n = rnd.next(maxNumberOfElements / 2U, maxNumberOfElements);
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
    
    unsigned int processQuery(NPerfectHash::ISet *set, EActionType type, unsigned int element)
    {
        switch (type)
        {
            case INSERT:
                return (set->insert(element), element);
            case ERASE:
                return (set->erase(element), element);
            case FIND:
                return set->find(element);
            case IS_POSSIBLE:
                return set->isPossible(element);
            case SIZE:
                return set->size();
        }
        return 0U;
    }
    
    void processQueryAndCatchException(NPerfectHash::ISet *set, EActionType type, unsigned int element, unsigned int &result, bool &exceptionHandled)
    {
        exceptionHandled = false;
        try
        {
            result = processQuery(set, type, element);
        }
        catch(std::exception &)
        {
            exceptionHandled = true;
        }
    }
    
    void initiationWithExceptionHandling(NPerfectHash::ISet *set, std::vector<unsigned int> const &elements, bool &exceptionHandled)
    {
        exceptionHandled = false;
        try
        {
            set->init(elements);
        }
        catch (std::exception &)
        {
            exceptionHandled = true;
        }
    }
    
    void test(ITest &testCase, NPerfectHash::ISet *firstSet, NPerfectHash::ISet *secondSet = NULL)
    {
        unsigned int testNumber = 0U;
        do 
        {
            ++testNumber;
            bool firstExceptionHandled, secondExceptionHandled;
            initiationWithExceptionHandling(firstSet, testCase.getAvalibleElements(), firstExceptionHandled);
            if (secondSet)
            {
                initiationWithExceptionHandling(secondSet, testCase.getAvalibleElements(), secondExceptionHandled);
                if (firstExceptionHandled != secondExceptionHandled)
                {
                    printf("\nDifferent Exception result - test %u\n", testNumber);
                    continue;
                }
            }
            
            Event currentQuery = testCase.nextQuery();
            unsigned int firstSetResult, secondSetResult;
            
            while (currentQuery.type != EXIT)
            {
                processQueryAndCatchException(firstSet, currentQuery.type, currentQuery.element, firstSetResult, firstExceptionHandled);
                if (secondSet)
                {
                    processQueryAndCatchException(secondSet, currentQuery.type, currentQuery.element, secondSetResult, secondExceptionHandled);
                    
                    if (firstExceptionHandled != secondExceptionHandled)
                    {
                        printf("\nDifferent Exception result - test %u\n", testNumber);
                        break;
                    }
                    if (firstSetResult != secondSetResult)
                    {
                        printf("\nDifferent Answers - test %u\n", testNumber);
                        break;
                    }
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