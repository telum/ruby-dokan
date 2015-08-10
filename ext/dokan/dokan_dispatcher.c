#include "dokan_ops.h"

void dokan_dispatcher_init(void)
{
    while(1) {
        dokan_dispatcher_loop();
    }
}

void dokan_dispatcher_loop(void)
{
    WaitForSingleObject(drs.dispatchEvent, INFINITE);

    // Dispatching

    SetEvent(drs.dispatchedEvent);
}
