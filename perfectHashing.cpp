#include <cstdio>
#include <vector>
#include "perfectHashing.h"

int main()
{
    unsigned int n;
    assert(scanf("%u", &n) == 1);
    std::vector<unsigned int> elements(n);
    for (unsigned int i = 0; i < n; ++i)
    {
        assert(scanf("%u", &elements[i]) == 1);
    }
    NPerfectHash::PerfectHashTable elems;
    elems.init(elements);
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
    
    