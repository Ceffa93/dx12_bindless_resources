#if __cplusplus 
using DescriptorHandle = unsigned int;
#define ALIGN_256 __declspec(align(256))
#else
#define DescriptorHandle unsigned int
#define ALIGN_256
#endif

