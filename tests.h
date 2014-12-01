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
    
    template<class RandomAccessIterator>
    void randomShuffle(RandomAccessIterator const &first, RandomAccessIterator const &last)
    {
        for (RandomAccessIterator it = first; it != last; ++it)
        {
            std::swap(*it, *(first + rnd.next(it - first)));
        }
    }
    
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
    
    class AllEqualInitTest: public ITest
    {
        std::vector <unsigned int> avalibleElements;
        unsigned int maxNumberOfElements;
        unsigned int numberOfInitializations;
        unsigned int initialisations;
        
        void init()
        {
            avalibleElements.assign(rnd.next(maxNumberOfElements / 2, maxNumberOfElements), rnd.next(UINT_MAX));
        }
        
    public:
        
        AllEqualInitTest(unsigned int maxNumberOfElements, unsigned int numberOfTests) : maxNumberOfElements(maxNumberOfElements)
        {
            initialisations = 0;
            numberOfInitializations = numberOfTests;
            init();
        }
        
        std::vector<unsigned int> const &getAvalibleElements() const
        {
            return avalibleElements;
        }
        Event nextQuery()
        {
            return Event(EXIT, 0U);
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
            if (++initialisations == numberOfInitializations)
            {
                return false;
            }
            init();
            return true;
        }
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
    
    class TwoElementInitTestRandomOrder;
    class PairInitTest;
    class PairInitRandomTest;
    class TwoElementInitTestStrongOrder: public ITest
    {
        friend TwoElementInitTestRandomOrder;
        friend PairInitTest;
        friend PairInitRandomTest;
        
        std::vector <unsigned int> avalibleElements;
        unsigned int maxNumberOfElements;
        unsigned int numberOfInitializations;
        unsigned int initialisations;
        
        void init()
        {
            unsigned int firstElement, secondElement;
            firstElement = rnd.next(UINT_MAX);
            do
            {
                secondElement = rnd.next(UINT_MAX);
            }
            while (firstElement == secondElement);
            unsigned int n = rnd.next(maxNumberOfElements / 4, maxNumberOfElements / 2);
            avalibleElements.clear();
            avalibleElements.reserve(2 * n);
            for (unsigned int i = 0; i < n; ++i)
            {
                avalibleElements.push_back(firstElement);
                avalibleElements.push_back(secondElement);
            }
        }
        
    public:
        
        TwoElementInitTestStrongOrder(unsigned int maxNumberOfElements, unsigned int numberOfTests) : maxNumberOfElements(maxNumberOfElements)
        {
            initialisations = 0;
            numberOfInitializations = numberOfTests;
            init();
        }
        
        std::vector<unsigned int> const &getAvalibleElements() const
        {
            return avalibleElements;
        }
        
        Event nextQuery()
        {
            return Event(EXIT, 0U);
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
            if (++initialisations == numberOfInitializations)
            {
                return false;
            }
            init();
            return true;
        }
    };
    
    class TwoElementInitTestRandomOrder: public TwoElementInitTestStrongOrder
    {
        void init()
        {
            unsigned int firstElement, secondElement;
            firstElement = rnd.next(UINT_MAX);
            do
            {
                secondElement = rnd.next(UINT_MAX);
            }
            while (firstElement == secondElement);
            unsigned int n = rnd.next(maxNumberOfElements / 4, maxNumberOfElements / 2);
            avalibleElements.clear();
            avalibleElements.reserve(2 * n);
            for (unsigned int i = 0; i < n; ++i)
            {
                avalibleElements.push_back(firstElement);
                avalibleElements.push_back(secondElement);
            }
            randomShuffle(avalibleElements.begin(), avalibleElements.end());
        }
    public:
        TwoElementInitTestRandomOrder(unsigned int maxNumberOfElements, unsigned int numberOfTests) : TwoElementInitTestStrongOrder(maxNumberOfElements, numberOfTests)
        {
        }
    };
    
    class TwoElementInitTestEveryOrder : public ITest
    {
        std::vector<bool> currentPositions;
        std::vector<unsigned int> avalibleElements;
        unsigned int firstElement;
        unsigned int secondElement;
        unsigned int numberOfElements;
        
        
    public:
        
        TwoElementInitTestEveryOrder(unsigned int numberOfElements) : numberOfElements(numberOfElements)
        {
            firstElement = rnd.next(UINT_MAX);
            do
            {
                secondElement = rnd.next(UINT_MAX);
            }
            while (firstElement == secondElement);
            avalibleElements.resize(2 * numberOfElements);
            for (unsigned int i = 0; i < numberOfElements; ++i)
            {
                avalibleElements[i] = firstElement;
                avalibleElements[numberOfElements + i] = secondElement;
            }
        }
        
        std::vector<unsigned int> const &getAvalibleElements() const
        {
            return avalibleElements;
        }
        Event nextQuery()
        {
            return Event(EXIT, 0U);
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
            for (unsigned int i = numberOfElements - 1; i != UINT_MAX; --i)
            {
                if (avalibleElements[i] == 0 && (i + 1 != numberOfElements && avalibleElements[i + 1] == 1))
                {
                    std::swap(avalibleElements[i], avalibleElements[i + 1]);
                    return true;
                }
            }
            return false;
        }
    };
    
    class BigKeyValuesAndRandomQueries;
    class IsPossibleTrueTest;
    class IsPossibleRandomTest;
    class RandomKeyInsertTest;
    
    class RandomUniqueSeqAndCorectRandomQueriesTest: public ITest
    {
        friend BigKeyValuesAndRandomQueries;
        friend IsPossibleTrueTest;
        friend IsPossibleRandomTest;
        friend RandomKeyInsertTest;
        
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
            unsigned int n = rnd.next(maxNumberOfElements / 2, maxNumberOfElements);
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
    
    
    class PairInitTest : public TwoElementInitTestStrongOrder
    {   
        void init()
        {
            unsigned int n = rnd.next(maxNumberOfElements / 4, maxNumberOfElements / 2);
            std::set<unsigned int> used;
            avalibleElements.resize(2 * n);
            unsigned int element;
            for (unsigned int i = 0; i != n; ++i)
            {
                do
                {
                    element = rnd.next(UINT_MAX);
                }
                while (used.find(element) != used.end());
                avalibleElements[2U * i] = avalibleElements[2U * i + 1] = element;
                used.insert(element);
            }
        }
    public:
        PairInitTest(unsigned int maxNumberOfElements, unsigned int numberOfTests) : TwoElementInitTestStrongOrder(maxNumberOfElements, numberOfTests)
        {
        }
    };
    
    class PairInitRandomTest : public TwoElementInitTestStrongOrder
    {
        void init()
        {
            unsigned int n = rnd.next(maxNumberOfElements / 4, maxNumberOfElements / 2);
            std::set<unsigned int> used;
            avalibleElements.resize(2 * n);
            unsigned int element;
            for (unsigned int i = 0; i != n; ++i)
            {
                do
                {
                    element = rnd.next(UINT_MAX);
                }
                while (used.find(element) != used.end());
                avalibleElements[2U * i] = avalibleElements[2U * i + 1] = element;
                used.insert(element);
            }
            randomShuffle(avalibleElements.begin(), avalibleElements.end());
        }
    public:
        PairInitRandomTest(unsigned int maxNumberOfElements, unsigned int numberOfTests) : TwoElementInitTestStrongOrder(maxNumberOfElements, numberOfTests)
        {
        }
    };
    
    class BigKeyValuesAndRandomQueries : public RandomUniqueSeqAndCorectRandomQueriesTest
    {
        void init()
        {
            avalibleElements.clear();
            calls = 0U;
            unsigned int n = rnd.next(maxNumberOfElements / 2, maxNumberOfElements);
            avalibleElements.resize(n);
            for (unsigned int i = 0U; i < n; ++i)
                avalibleElements[i] = UINT_MAX - n + i;
            randomShuffle(avalibleElements.begin(), avalibleElements.end());
            numberOfQueries = rnd.next(1U, maxNumberOfQueries);
        }
    public:
        BigKeyValuesAndRandomQueries(unsigned int numberOfTests, unsigned int maxNumberOfElements, unsigned int maxNumberOfQueries) :
            RandomUniqueSeqAndCorectRandomQueriesTest(numberOfTests, maxNumberOfElements, maxNumberOfQueries)
        {
        }
    };
    
    class IsPossibleTrueTest: public RandomUniqueSeqAndCorectRandomQueriesTest
    {
    public:
        Event nextQuery()
        {
            if (++calls == numberOfQueries)
            {
                return Event(EXIT, 0U);
            }
            return Event(IS_POSSIBLE, avalibleElements[rnd.next(0U, static_cast<unsigned int>(avalibleElements.size()) - 1U)]);
        }
    public:
        IsPossibleTrueTest(unsigned int numberOfTests, unsigned int maxNumberOfElements, unsigned int maxNumberOfQueries) :
            RandomUniqueSeqAndCorectRandomQueriesTest(numberOfTests, maxNumberOfElements, maxNumberOfQueries)
        {
        }
    };
    
    class IsPossibleRandomTest: public RandomUniqueSeqAndCorectRandomQueriesTest
    {
    public:
        Event nextQuery()
        {
            if (++calls == numberOfQueries)
            {
                return Event(EXIT, 0U);
            }
            return Event(IS_POSSIBLE, rnd.next(0U, UINT_MAX));
        }
    public:
        IsPossibleRandomTest(unsigned int numberOfTests, unsigned int maxNumberOfElements, unsigned int maxNumberOfQueries) :
            RandomUniqueSeqAndCorectRandomQueriesTest(numberOfTests, maxNumberOfElements, maxNumberOfQueries)
        {
        }
    };
    
    class MultipleInsertTest;
    
    class CleverSequenceOfQueriesTest: public ITest
    {
        friend MultipleInsertTest;
        std::vector <unsigned int> avalibleElements;
        std::vector <Event> events;
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
            unsigned int n = rnd.next(maxNumberOfElements / 2, maxNumberOfElements);
            avalibleElements.resize(n);
            for (unsigned int i = 0U; i < n; ++i)
                avalibleElements[i] = rnd.next(0U, UINT_MAX);
            std::sort(avalibleElements.begin(), avalibleElements.end());
            avalibleElements.resize(std::distance(avalibleElements.begin(), std::unique(avalibleElements.begin(), avalibleElements.end())));
            numberOfQueries = rnd.next(0U, maxNumberOfQueries / 8);
            for (unsigned int i = 0; i < numberOfQueries; ++i)
            {
                unsigned int element = avalibleElements[rnd.next(0U, static_cast<unsigned int>(avalibleElements.size()) - 1U)];
                events.push_back(Event(IS_POSSIBLE, element));
                events.push_back(Event(SIZE, element));
                events.push_back(Event(INSERT, element));
                events.push_back(Event(IS_POSSIBLE, element));
                events.push_back(Event(SIZE, element));
                events.push_back(Event(ERASE, element));
                events.push_back(Event(IS_POSSIBLE, element));
                events.push_back(Event(SIZE, element));                
            }
            events.push_back(Event(EXIT, 0U));
            ++numberOfQueries; // EXIT
        }
        
    public:
        CleverSequenceOfQueriesTest(unsigned int numberOfTests, unsigned int maxNumberOfElements, unsigned int maxNumberOfQueries) :
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
            return events[calls++];
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
    
    class MultipleInsertTest: public CleverSequenceOfQueriesTest
    {
        void init()
        {
            avalibleElements.clear();
            calls = 0U;
            unsigned int n = rnd.next(maxNumberOfElements / 2, maxNumberOfElements);
            avalibleElements.resize(n);
            for (unsigned int i = 0U; i < n; ++i)
                avalibleElements[i] = rnd.next(0U, UINT_MAX);
            std::sort(avalibleElements.begin(), avalibleElements.end());
            avalibleElements.resize(std::distance(avalibleElements.begin(), std::unique(avalibleElements.begin(), avalibleElements.end())));
            numberOfQueries = rnd.next(0U, maxNumberOfQueries / 9);
            for (unsigned int i = 0; i < numberOfQueries; ++i)
            {
                unsigned int element = avalibleElements[rnd.next(0U, static_cast<unsigned int>(avalibleElements.size()) - 1U)];
                events.push_back(Event(IS_POSSIBLE, element));
                events.push_back(Event(SIZE, element));
                events.push_back(Event(INSERT, element));
                events.push_back(Event(INSERT, element));
                events.push_back(Event(IS_POSSIBLE, element));
                events.push_back(Event(SIZE, element));
                events.push_back(Event(ERASE, element));
                events.push_back(Event(IS_POSSIBLE, element));
                events.push_back(Event(SIZE, element));                
            }
            events.push_back(Event(EXIT, 0U));
            ++numberOfQueries; // EXIT
        }
    public:
        MultipleInsertTest(unsigned int numberOfTests, unsigned int maxNumberOfElements, unsigned int maxNumberOfQueries) :
            CleverSequenceOfQueriesTest(numberOfTests, maxNumberOfElements, maxNumberOfQueries)
        {
        }
    };
    
    class RandomKeyInsertTest: public RandomUniqueSeqAndCorectRandomQueriesTest
    {
        Event nextQuery()
        {
            if (++calls == numberOfQueries)
            {
                return Event(EXIT, 0U);
            }
            return Event(EActionType(rnd.next(0U, EActionTypeSize - 1U)), rnd.next(0U, UINT_MAX));
        }
    public:
        RandomKeyInsertTest(unsigned int numberOfTests, unsigned int maxNumberOfElements, unsigned int maxNumberOfQueries) :
            RandomUniqueSeqAndCorectRandomQueriesTest(numberOfTests, maxNumberOfElements, maxNumberOfQueries)
        {
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
#ifndef DISABLE_POSITIVE_OUTPUT
            printf("+");
#endif
        }
        while (testCase.nextInitialization());
        printf("\n");
    }
    
};
#endif