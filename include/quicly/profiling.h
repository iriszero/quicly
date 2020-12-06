#ifndef quicly_profiling_h
#define quicly_profiling_h

#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>
#include <stdio.h>
#include <string.h>

typedef unsigned long long rdtsc_t;
struct time_per_function {
    rdtsc_t sum, recent;
    int count;
};

struct st_profiling_data {
    struct time_per_function
        profiling_main,
        quicly_select,
        quicly_read,
            quicly_recvmsg,
            quicly_decode_packet,
            quicly_receive,
                decrypt_packet,
                handle_payload,
                    handle_stream_frame,
                        quicly_recvbuf_shift,
                    handle_ack_frame,
            
        quicly_send_pending_wrapper,
            quicly_send_pending,
                quicly_sendmsg,
                do_send,
                    quicly_send_stream,
                        allocate_ack_eliciting_frame,
                            aead_do_encrypt

    ;

    /*
        ,

        picoquic_packet_loop,
            picoquic_select_ex,
                select,
                picoquic_recvmsg,
            picoquic_incoming_packet,
                picoquic_remove_header_protection,
                picoquic_remove_packet_protection,
                picoquic_decode_stream_frame,
                    picoquic_stream_network_input,
                        picoquic_queue_network_input,
                        picoquic_stream_data_callback,
                picoquic_decode_ack_frame,
            picoquic_prepare_next_packet,
                picoquic_protect_packet,
            picoquic_send_through_socket,
                sendmsg,
        
        memcpy_wrapper
    ;
    */

    int is_profiling;
} profiling_data;

void profiling_init();
void profiling_start();
void profiling_stop();

void profiling_func_start(struct time_per_function *tpf);
void profiling_func_end(struct time_per_function *tpf);

void profiling_print_times();
void profiling_clear_data();

void profiling_int_handler(int dummy);

#ifdef __cplusplus
}
#endif


#endif