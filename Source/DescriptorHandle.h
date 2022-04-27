#include <utility>

template <class Name>
class DescriptorHandle
{
public:
    explicit DescriptorHandle(unsigned int v = 0)
        : type(v) 
    {}

    operator unsigned int () { return type; }

    unsigned int get() { return type; }

private:
    unsigned int type;
    unsigned int _pad[3];
};
