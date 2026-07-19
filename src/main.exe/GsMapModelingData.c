#include "common.h"
#include "main.exe.h"

void GsMapModelingData(unsigned long *model)
{
    struct TMD_STRUCT *object;
    int count;
    int i;

    if (*model & 1)
        return;

    *model |= 1;
    ++model;
    i = 0;
    count = *model++;
    if (count > 0)
    {
        do
        {
            object = (struct TMD_STRUCT *)(model + i * 7);
            object->vertop = (unsigned long *)((unsigned long)object->vertop +
                                               (unsigned long)model);
            object->nortop = (unsigned long *)((unsigned long)object->nortop +
                                              (unsigned long)model);
            object->primtop = (unsigned long *)((unsigned long)object->primtop +
                                               (unsigned long)model);
            ++i;
        } while (i < count);
    }
}
