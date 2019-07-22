#include "KVM.h"
#include "Keyboard.h"
#include "Protocol.h"

void KVMInit(void)
{
    KeyboardInit();
    ProtocolInit();
}

void KVMPoll(void)
{
}
