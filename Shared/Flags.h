#include "Shared.h"

ALIGN_256 struct Flags
{
    DescriptorHandle srv2D;
    DescriptorHandle srv3D;
    DescriptorHandle uav2D;
    DescriptorHandle uav3D;
    DescriptorHandle sampler;
};