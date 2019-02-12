#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

int
main(int ArgCount, char **Args)
{
    int PointerCount = 0;
    GetPointerDevices(&PointerCount, 0);

    POINTER_DEVICE_INFO *PenInfos = malloc(sizeof(POINTER_DEVICE_INFO) * PointerCount);

    GetPointerDevices(&PointerCount, PenInfos);

    for(int Index = 0;
        Index < PointerCount;
        ++Index)
    {
        int PropertyCount = 0;
        GetPointerDeviceProperties(PenInfos[Index].device, &PropertyCount, 0);

        POINTER_DEVICE_PROPERTY *Properties = malloc(sizeof(POINTER_DEVICE_PROPERTY) * PropertyCount);
        printf("PEN %d:\n", Index);
        if(GetPointerDeviceProperties(PenInfos[Index].device, &PropertyCount, Properties))
        {
            for(int PropertyIndex = 0;
                PropertyIndex < PropertyCount;
                ++PropertyIndex)
            {
                printf("  logicalMin: %d\n", Properties[PropertyIndex].logicalMin);
                printf("  logicalMax: %d\n", Properties[PropertyIndex].logicalMax);
                printf("  physicalMin: %d\n", Properties[PropertyIndex].physicalMin);
                printf("  physicalMax: %d\n", Properties[PropertyIndex].physicalMax);
                printf("  unit: %u\n", Properties[PropertyIndex].unit);
                printf("  unitExponent: %u\n", Properties[PropertyIndex].unitExponent);
                printf("  usagePageId: %u\n", (unsigned short)Properties[PropertyIndex].usagePageId);
                printf("  usageId: %u\n\n", (unsigned short)Properties[PropertyIndex].usageId);
            }
        }
    }

    printf("Got %d pens back\n", PointerCount);
    return(0);
}