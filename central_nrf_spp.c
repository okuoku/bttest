#include "btstack.h"

#include <stdio.h>
#include <stdint.h>

static btstack_packet_callback_registration_t the_callback;

static void
hci_event_handler(uint8_t packet_type, uint16_t channel,
                  uint8_t* packet, uint16_t size){

    uint8_t event = hci_event_packet_get_type(packet);
    char namebuf[257];
    //printf("Packet Type: %d\n", packet_type);
    if(packet_type != HCI_EVENT_PACKET){
        return;
    }
    switch(event){
        case BTSTACK_EVENT_STATE:
            if(btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
                gap_set_scan_params(0 /* Passive */,
                                    0x30 /* Short enough */,
                                    0x30 /* Short enough */,
                                    0 /* All */);
                gap_start_scan();
            }
            break;
        case GAP_EVENT_ADVERTISING_REPORT:
            {
                const uint8_t* adv = 
                    gap_event_advertising_report_get_data(packet);
                uint16_t adv_len = 
                    gap_event_advertising_report_get_data_length(packet);
                ad_context_t ctx;
                for(ad_iterator_init(&ctx, adv_len, adv);
                    ad_iterator_has_more(&ctx);
                    ad_iterator_next(&ctx)){
                    uint8_t type = ad_iterator_get_data_type(&ctx);
                    uint8_t len = ad_iterator_get_data_len(&ctx);
                    const uint8_t* data = ad_iterator_get_data(&ctx);
                    switch(type){
                        case BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME:
                        case BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME:
                            memset(namebuf,0,257);
                            memcpy(namebuf,data,len);
                            printf("Name: %s\n", namebuf);
                            break;
                    }
                }
            }
            break;
        default:
            break;
    }
}

int
btstack_main(void){
    l2cap_init();
    sm_init();
    gatt_client_init();

    the_callback.callback = hci_event_handler;

    hci_add_event_handler(&the_callback);

    hci_power_control(HCI_POWER_ON);

    return 0;
}
