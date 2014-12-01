#ifndef _PERFECT_HASH_TABLE
#define _PERFECT_HASH_TABLE

#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <string>
#include <algorithm>
#include "testlib.h"

namespace NPerfectHash
{
    class ISet
    {
    public:
        virtual void insert(unsigned int) = 0;
        virtual void erase(unsigned int) = 0;
        virtual bool find(unsigned int) const = 0;
        virtual bool isPossible(unsigned int) const = 0;
        virtual void init(std::vector<unsigned int> const &) = 0;
        virtual unsigned int size() const = 0;
    };
    
    class Hash
    {
        static const unsigned long long PRIME = 4294967311LLU;
        unsigned long long firstHashCoefficient;
        unsigned long long secondHashCoefficient;
        unsigned int sizeOfSet;
        
    public:
        inline void generateNewCoefficients()
        {
            firstHashCoefficient = rnd.next(1LLU, PRIME - 1LLU); 
            secondHashCoefficient = rnd.next(0LLU, PRIME - 1LLU);
        }
        
        inline void setSize(unsigned int size)
        {
            sizeOfSet = size;
        }
        
        inline unsigned int operator()(unsigned int key) const
        {
            return (((((firstHashCoefficient >> 32LLU) << 32LLU) * key) % PRIME + ((firstHashCoefficient & UINT_MAX) * key) % PRIME + secondHashCoefficient) % PRIME) % sizeOfSet;
        }
    };
    
    template<class SetType>
    inline void chooseHashFunction(std::vector<unsigned int> const &elements, SetType &hashSet, unsigned int (*sizeConversion) (unsigned int))
    {
        hashSet.sizeOfSet = sizeConversion(elements.size());
        hashSet.hash.setSize(hashSet.sizeOfSet);
        
        do
        {
            hashSet.hash.generateNewCoefficients();
        }
        while (hashSet.isBadHashFunction(elements));
    }
        
    class EqualElementsException: public std::exception
    {
        std::string messageTemplate;
    public:
        unsigned int whichElement;
        explicit EqualElementsException(unsigned int element) : whichElement(element) 
        {
            messageTemplate = "There are two or more instances of " + std::to_string(element) + " element\n";
        }
        virtual const char* what() const throw()
        {
            return messageTemplate.c_str();
        }
    };
    
    class ImpossibleElementException: public std::exception
    {
        std::string messageTemplate;
    public:
        unsigned int whichElement;
        explicit ImpossibleElementException(unsigned int element) : whichElement(element)
        {
            messageTemplate = "No support of " + std::to_string(element) + " element\n";
        }
        virtual const char* what() const throw()
        {
            return messageTemplate.c_str();
        }
    };
    
    class PerfectHashSet: public ISet
    { 
        static unsigned long long square(unsigned int x)
        {
            return 1llu * x * x;
        }
        
        struct InnerHashSet
        {
            std::vector<bool> presence;
            std::vector<unsigned int> hashElement;
            
            Hash hash;
            unsigned int sizeOfSet;
            
            InnerHashSet(std::vector<unsigned int> const &elements)
            {
                init(elements);
            }
            
            inline bool isBadHashFunction(std::vector<unsigned int> const &elements)
            {
                presence.assign(sizeOfSet, false);
                hashElement.assign(sizeOfSet, 0U);
                for (auto const &element: elements)
                {
                    unsigned int currentHash = hash(element);
                    
                    if (presence[currentHash])
                    {
                        if (hashElement[currentHash] == element)
                        {
                            throw EqualElementsException(element);
                        }
                        return true;
                    }
                    
                    hashElement[currentHash] = element;
                    presence[currentHash] = true;
                }
                
                return false;
            }
            
            friend void chooseHashFunction<InnerHashSet>(std::vector<unsigned int> const &, InnerHashSet &, unsigned int (unsigned int));
            
            inline void init(std::vector<unsigned int> const &elements)
            {
                chooseHashFunction(elements, *this, [] (unsigned int x) -> unsigned int 
                                                    {
                                                        return x * x;
                                                    }
                );
                
                presence.assign(sizeOfSet, false);
                if (elements.size() && std::count(elements.begin(), elements.end(), 0U) == 0U)
                {
                    hashElement[hash(0U)] = 1U;
                }
            }
            
            inline void checkPossibility(unsigned int element) const
            {
                if (!isPossible(element))
                {
                    throw ImpossibleElementException(element);
                }
            }
            
            inline bool operate(unsigned int element, bool operationType) // 1 - insert, 0 - remove;
            {
                checkPossibility(element);
                bool result = operationType ^ presence[hash(element)];
                presence[hash(element)] = operationType;
                return result;
            }
            
            bool insert(unsigned int element)
            {
                return operate(element, 1);
            }
            
            bool erase(unsigned int element)
            {
                return operate(element, 0);
            }
            
            bool find(unsigned int element) const
            {           
                checkPossibility(element);
                return presence[hash(element)];
            }
            
            bool isPossible(unsigned int element) const
            {
                return (presence.size() && hashElement[hash(element)] == element);
            }
        };
        
        
        std::vector<InnerHashSet> innerHashSets;
        std::vector<std::vector<unsigned int> > innerSetsElements;
        Hash hash;
        unsigned int sizeOfSet;
        unsigned int numberOfElements;
        
        inline void checkEqualityAndThrowExceptionIfEqual(unsigned int firstElement, unsigned int secondElement) const
        {
            if (firstElement == secondElement)
            {
                throw EqualElementsException(firstElement);
            }
        }
        
        inline bool isBadHashFunction(std::vector<unsigned int> const &elements)
        {
            innerSetsElements.assign(sizeOfSet, std::vector<unsigned int> ());
            
            for (auto const &element: elements)
            {
                innerSetsElements[hash(element)].push_back(element);
            }
            
            unsigned long long sumOfSquaresOfInnerSetSizes = 0;
            
            for (auto const &setElements: innerSetsElements)
            {
                for (unsigned int i = 1; i < setElements.size(); ++i)
                {
                    checkEqualityAndThrowExceptionIfEqual(setElements[i - 1], setElements[i]);
                }
                if (setElements.size() == 3U)
                {
                    checkEqualityAndThrowExceptionIfEqual(setElements.front(), setElements.back());
                }
                sumOfSquaresOfInnerSetSizes += square(setElements.size());
            }
            return (sumOfSquaresOfInnerSetSizes > 3LLU * sizeOfSet);
        }
        
        
        inline void fillInnerHashSets()
        {
            innerHashSets.clear();
            innerHashSets.reserve(sizeOfSet);
            
            for (auto const &elements: innerSetsElements)
            {
                innerHashSets.push_back(InnerHashSet(elements));
            }
        }
        
        friend void chooseHashFunction<PerfectHashSet>(std::vector<unsigned int> const &, PerfectHashSet &, unsigned int (unsigned int));

    public:
        inline void init(std::vector<unsigned int> const &elements)
        {   
            numberOfElements = 0U;
            chooseHashFunction(elements, *this, [](unsigned int x) -> unsigned int
                                                {
                                                    return x;
                                                }
            );
            
            fillInnerHashSets();
        }
        
        void insert(unsigned int element) 
        {
            numberOfElements += innerHashSets[hash(element)].insert(element);
        }
        
        void erase(unsigned int element)
        {
            numberOfElements -= innerHashSets[hash(element)].erase(element);
        }
        
        bool find(unsigned int element) const
        {
            return innerHashSets[hash(element)].find(element);
        }
        
        bool isPossible(unsigned int element) const
        {
            return innerHashSets[hash(element)].isPossible(element);
        }
        
        unsigned int size() const
        {
            return numberOfElements;
        }
    };
};

#endif