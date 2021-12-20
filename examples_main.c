#include "btstack_config.h"
#include "btstack_run_loop.h"
#include "btstack_run_loop_posix.h"
#include "btstack_memory.h"

#include "hci_transport.h"
#include "hci_transport_usb.h"

#include "btstack_tlv_posix.h"
#include "ble/le_device_db_tlv.h"

#include <stdio.h>
#include <libusb.h>

int btstack_main(void);

int
main(int ac, char** av){
    int skip_device = 0;
    int found_device = 0;
    struct libusb_device_descriptor d;
    uint8_t path[8];
    int pathlen;
    int i;
    ssize_t lis_len;
    btstack_tlv_posix_t tlv_posix_ctx;
    const btstack_tlv_t* tlv;
    libusb_device** lis;
    libusb_init(NULL);
    lis_len =  libusb_get_device_list(NULL, &lis);
    for(i=0;i<lis_len;i++){
        libusb_get_device_descriptor(lis[i], &d);
        printf("Device %x:%x\n",d.idVendor,d.idProduct);
        if(d.idVendor == 0x0a12 && d.idProduct == 1){
            if(skip_device){
                skip_device = 0;
                printf("Skip one.\n");
            }else{
                found_device = 1;
                pathlen = libusb_get_port_numbers(lis[i], path, 8);
                break;
            }
        }
    }

    libusb_free_device_list(lis, 0);
    if(found_device){
        printf("pathlen = %d\n", pathlen);
        hci_transport_usb_set_path(pathlen, path);
    }else{
        printf("not fouusd.\n");
        return 1;
    }


    btstack_memory_init();
    btstack_run_loop_init(btstack_run_loop_posix_get_instance());

    hci_init(hci_transport_usb_instance(), NULL);

    /* configure TLV db */
    tlv = btstack_tlv_posix_init_instance(&tlv_posix_ctx, "db.tlv");
    btstack_tlv_set_instance(tlv, &tlv_posix_ctx);
    le_device_db_tlv_configure(tlv, &tlv_posix_ctx);

    /* Run app */
    btstack_main();

    btstack_run_loop_execute();
    return 0;
}
