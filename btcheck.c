#include "btstack_config.h"
#include "btstack_run_loop.h"
#include "btstack_run_loop_posix.h"
#include "btstack_memory.h"

#include "hci_transport.h"
#include "hci_transport_usb.h"

#include "btstack_tlv_posix.h"

int
main(int ac, char** av){
    btstack_memory_init();
    btstack_run_loop_init(btstack_run_loop_posix_get_instance());


    return 0;
}
