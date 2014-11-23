#ifndef _PERFECT_HASH_TABLE
#define _PERFECT_HASH_TABLE

#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <string>
#include <algorithm>

namespace NPerfectHash
{
    class Hash
    {
        static const unsigned long long PRIME = 4294967311LLU;
        unsigned long long firstHashCoefficient;
        unsigned long long secondHashCoefficient;
        unsigned int sizeOfTable;
        
    public:
        void generateNewCoefficients()
        {
            firstHashCoefficient = rand() % (PRIME - 1U) + 1U;
            secondHashCoefficient = rand() % PRIME;
        }
        
        void setSize(unsigned int size)
        {
            sizeOfTable = size;
        }
        
        unsigned int operator()(unsigned int key) const
        {
            return ((((firstHashCoefficient >> 32LLU) * key) % PRIME + ((firstHashCoefficient & UINT_MAX) * key) % PRIME + secondHashCoefficient) % PRIME) % sizeOfTable;
        }
    };

        template<class TableType>
    void chooseSortFunction(std::vector<unsigned int> const &elements, TableType &hashTable, unsigned int (*sizeConversion) (unsigned int))
    {
        hashTable.sizeOfTable = sizeConversion(elements.size());
        hashTable.hash.setSize(hashTable.sizeOfTable);
        
        do
        {
            hashTable.hash.generateNewCoefficients();
        }
        while (hashTable.isBadHashFunction(elements));
    }

    class PerfectHashTable
    {
        class IException
        {
            static const std::string messageTemplate;
        public:
            unsigned int whichElement;
            virtual void what() = 0;
        };
        
        class EqualElementsException : public IException
        {
            std::string messageTemplate;
        public:
            unsigned int whichElement;
            EqualElementsException(unsigned int element) : whichElement(element) 
            {
                messageTemplate = "There are two or more instances if %u element\n";
            }
            void what()
            {
                printf(messageTemplate.c_str(), whichElement);
            }
        };
        
        class ImpossibleElementException : public IException
        {
            std::string messageTemplate;
        public:
            ImpossibleElementException(unsigned int element) : whichElement(element)
            {
                messageTemplate = "No support of %u element\n";
            }
            unsigned int whichElement;
            void what()
            {
                printf(messageTemplate.c_str(), whichElement);
            }
        };
        
        static unsigned long long square(unsigned int x)
        {
            return 1llu * x * x;
        }
        
        struct InnerHashTable
        {
            std::vector<bool> presence;
            std::vector<int> hashElement;
            std::vector<bool> hashAvalible;
            
            Hash hash;
            unsigned int sizeOfTable;
            
            InnerHashTable(std::vector<unsigned int> const &elements)
            {
                init(elements);
            }
            
            void equalityCheck(std::vector<unsigned int> const &elements)
            {
                for (unsigned int firstElementIndex = 0; firstElementIndex != elements.size(); ++firstElementIndex)
                {
                    for (unsigned int secondElementIndex = 0; secondElementIndex != firstElementIndex; ++secondElementIndex)
                    {
                        if (elements[firstElementIndex] == elements[secondElementIndex])
                        {
                            throw EqualElementsException(elements[firstElementIndex]);
                        }
                    }
                }
            }
            
            bool isBadHashFunction(std::vector<unsigned int> const &elements)
            {
                presence.assign(sizeOfTable, false);
                
                for (auto const &element: elements)
                {
                    unsigned int currentHash = hash(element);
                    
                    if (presence[currentHash])
                    {
                        return true;
                    }
                    
                    presence[currentHash] = true;
                }
                
                return false;
            }
            
            friend void chooseSortFunction<InnerHashTable>(std::vector<unsigned int> const &, InnerHashTable &, unsigned int (unsigned int));
            
            void init(std::vector<unsigned int> const &elements)
            {
                equalityCheck(elements);
                chooseSortFunction(elements, *this, [] (unsigned int x) -> unsigned int 
                                                    {
                                                        return x * x;
                                                    }
                );
                
                presence.assign(sizeOfTable, false);
                hashAvalible.assign(sizeOfTable, false);
                hashElement.resize(sizeOfTable);
                for (auto const &element: elements)
                {
                    hashAvalible[hash(element)] = true;
                    hashElement[hash(element)] = element;
                }
            }
            
            bool insert(unsigned int element)
            {
                if (!isPossible(element))
                {
                    throw ImpossibleElementException(element);
                }
                bool added = !presence[hash(element)];
                presence[hash(element)] = 1;
                return added;
            }
            
            bool erase(unsigned int element)
            {
                if (!isPossible(element))
                {
                    throw ImpossibleElementException(element);
                }
                bool removed = presence[hash(element)];
                presence[hash(element)] = 0;
                return removed;
            }
            
            bool find(unsigned int element)
            {
                if (!isPossible(element))
                {
                    throw ImpossibleElementException(element);
                }
                return presence[hash(element)];
            }
            
            bool isPossible(unsigned int element)
            {
                return (hashAvalible[hash(element)] && hashElement[hash(element)] == element);
            }
        };
        
        
        std::vector<InnerHashTable> innerHashTables;
        std::vector<std::vector<unsigned int> > innerTablesElements;
        Hash hash;
        unsigned int sizeOfTable;
        unsigned int numberOfElements;
        
        bool isBadHashFunction(std::vector<unsigned int> const &elements)
        {
            innerTablesElements.assign(sizeOfTable, std::vector<unsigned int> ());
            
            for (auto const &element: elements)
            {
                innerTablesElements[hash(element)].push_back(element);
            }
            
            unsigned long long sumOfSquaresOfInnerTableSizes = 0;
            
            for (auto const &tableElements: innerTablesElements)
            {
                for (unsigned int i = 1; i < tableElements.size(); ++i)
                {
                    if (tableElements[i] == tableElements[i - 1])
                    {
                        throw EqualElementsException(tableElements[i]);
                    }
                }
                if (tableElements.size() == 3u)
                {
                    if (tableElements.front() == tableElements.back())
                    {
                        throw EqualElementsException(tableElements.front());
                    }
                }
                sumOfSquaresOfInnerTableSizes += square(tableElements.size());
            }
            return (sumOfSquaresOfInnerTableSizes > 3llu * sizeOfTable);
        }
        
        
        void fillInnerHashTables()
        {
            innerHashTables.clear();
            innerHashTables.reserve(sizeOfTable);
            
            for (auto const &elements: innerTablesElements)
            {
                innerHashTables.push_back(InnerHashTable(elements));
            }
        }
        
        friend void chooseSortFunction<PerfectHashTable>(std::vector<unsigned int> const &, PerfectHashTable &, unsigned int (unsigned int));

    public:
        void init(std::vector<unsigned int> const &elements)
        {   
            numberOfElements = 0U;
            chooseSortFunction(elements, *this, [](unsigned int x) -> unsigned int
                                                {
                                                    return x;
                                                }
            );
            
            fillInnerHashTables();
        }
        
        void insert(unsigned int element) 
        {
            numberOfElements += innerHashTables[hash(element)].insert(element);
        }
        
        void erase(unsigned int element)
        {
            numberOfElements -= innerHashTables[hash(element)].erase(element);
        }
        
        bool find(unsigned int element)
        {
            return innerHashTables[hash(element)].find(element);
        }
        
        bool isPossible(unsigned int element)
        {
            return innerHashTables[hash(element)].find(element);
        }
        
        size_t size()
        {
            return numberOfElements;
        }
    };
};

#endif