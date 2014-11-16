#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <climits>

class Hash
{
    static const unsigned long long PRIME = 4294967311LLU;
    __int128 firstHashCoefficient;
    __int128 secondHashCoefficient;
    unsigned int sizeOfTable;
    
public:
    void generateNewCoefficients()
    {
        firstHashCoefficient = rand() % (PRIME - 1) + 1;
        secondHashCoefficient = rand() % PRIME;
    }
    
    void setSize(unsigned int size)
    {
        sizeOfTable = size;
    }
    
    unsigned int operator()(unsigned int key) const
    {
        return ((firstHashCoefficient * key + secondHashCoefficient) % PRIME) % sizeOfTable;
    }
};

template<class TableType>
void chooseSortFunction(std::vector <unsigned int> const &elements, TableType &hashTable, unsigned int (*sizeConversion) (unsigned int))
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
    static unsigned long long square(unsigned int x)
    {
        return 1llu * x * x;
    }
    
    struct InnerHashTable
    {
        std::vector <bool> presence;
        Hash hash;
        unsigned int sizeOfTable;
        
        InnerHashTable(std::vector <unsigned int> const &elements)
        {
            init(elements);
        }
        
        bool isBadHashFunction(std::vector <unsigned int> const &elements)
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
        
        void init(std::vector <unsigned int> const &elements)
        {   
            chooseSortFunction(elements, *this, [] (unsigned int x) -> unsigned int 
                                                {
                                                    return x * x;
                                                }
            );
            
            presence.assign(sizeOfTable, false);
        }
        
        void insert(unsigned int element)
        {
            presence[hash(element)] = 1;
        }
        
        void erase(unsigned int element)
        {
            presence[hash(element)] = 0;
        }
        
        bool find(unsigned int element)
        {
            return presence[hash(element)];
        }
    };
    
    
    std::vector <InnerHashTable> innerHashTables;
    std::vector <std::vector<unsigned int> > innerTablesElements;
    Hash hash;
    unsigned int sizeOfTable;
    
    void processCountSortIteration(std::vector <unsigned int> &sourceArray,
                                   std::vector <unsigned int> &outputArray,
                                   std::vector <unsigned int> &count,
                                   unsigned int (*countFunction)(unsigned int)
         ) const
    {
        for (auto const &element: sourceArray)
        {
            ++count[countFunction(element)];
        }
        
        for (unsigned int i = 1; i < count.size(); ++i)
        {
            count[i] += count[i - 1];
        }
        
        for (unsigned int i = sourceArray.size() - 1; i != UINT_MAX; --i)
        {
            outputArray[--count[countFunction(sourceArray[i])]] = sourceArray[i];
        }

    }
    
    bool hasEqualElements(std::vector <unsigned int> elements) const
    {
        static const unsigned int SHIFT = 16u;
        static std::vector<unsigned int> count((1u << SHIFT), 0);
        
        std::vector<unsigned int> temp(elements.size());
        processCountSortIteration(elements, temp, count, [](unsigned int x) -> unsigned int
                                                         {
                                                             return (x & ((1u << SHIFT) - 1));
                                                         }
        );
        processCountSortIteration(temp, elements, count, [](unsigned int x) -> unsigned int
                                                         {
                                                             return (x >> SHIFT);
                                                         }
        );
        
        for (auto const &element: elements)
        {
            printf("%u ", element);
        }
        printf("\n");
        
        for (unsigned int i = 1; i < elements.size(); ++i)
        {
            if (elements[i] == elements[i - 1])
                return true;
        }
        return false;
    }
    
    bool isBadHashFunction(std::vector <unsigned int> const &elements)
    {
        innerTablesElements.assign(sizeOfTable, std::vector<unsigned int> ());
        
        for (auto const &element: elements)
        {
            innerTablesElements[hash(element)].push_back(element);
        }
        
        unsigned long long sumOfSquaresOfInnerTableSizes = 0;
        
        for (unsigned int i = 0; i < sizeOfTable; ++i)
        {
            sumOfSquaresOfInnerTableSizes += square(innerTablesElements[i].size());
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
    
    void init(std::vector <unsigned int> const &elements)
    {   
        chooseSortFunction(elements, *this, [](unsigned int x) -> unsigned int
                                            {
                                                return x;
                                            }
        );
        
        fillInnerHashTables();
    }
    
public:
    PerfectHashTable(std::vector<unsigned int> const &elements)
    {
        if (!hasEqualElements(elements))
        {
            init(elements);
        }
    }
    
    void insert(unsigned int element) 
    {
        innerHashTables[hash(element)].insert(element);
    }
    
    void erase(unsigned int element)
    {
        innerHashTables[hash(element)].erase(element);
    }
    
    bool find(unsigned int element)
    {
        return innerHashTables[hash(element)].find(element);
    }
};

int main()
{
    unsigned int n;
    assert(scanf("%u", &n) == 1);
    std::vector <unsigned int> elements(n);
    for (unsigned int i = 0; i < n; ++i)
    {
        assert(scanf("%u", &elements[i]) == 1);
    }
    PerfectHashTable elems(elements);
    int m;
    assert(scanf("%d", &m) == 1);
    while (m --> 0)
    {
        int t, k;
        assert(scanf("%d %d", &t, &k) == 2);
        if (t == 1)
            elems.insert(k);
        else if (t == 2)
            elems.erase(k);
        else
            printf("%d\n", int(elems.find(k)));
    }
    return 0;
}