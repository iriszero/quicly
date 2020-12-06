#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <quicly/profiling.h>
#include <emmintrin.h>
#include <sched.h>

#include <signal.h>
#include <stdlib.h>

void profiling_int_handler(int dummy) {
    profiling_stop();

    profiling_print_times();
    exit(1);
}

int _cpu_affinity() {
    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    
    return sched_setaffinity(0, sizeof(mask), &mask);
}

void profiling_init() {
    if (_cpu_affinity() != 0) {
        printf("Profiling: CPU affinity set error");
    }
    profiling_clear_data();
}

void profiling_start() {
    profiling_data.is_profiling = 1;
    profiling_func_start(&profiling_data.profiling_main);
}

void profiling_stop() {
    profiling_func_end(&profiling_data.profiling_main);
    profiling_data.is_profiling = 0;
}

void profiling_func_start(struct time_per_function *tpf) {
#ifdef PROFILING
    unsigned int dummy;
    if (profiling_data.is_profiling) {
        _mm_mfence();
        tpf->recent = __rdtscp(&dummy);
        _mm_mfence();
    }
#endif
}

void profiling_func_end(struct time_per_function *tpf) {
#ifdef PROFILING
    unsigned int dummy;

    if (profiling_data.is_profiling) {
        _mm_mfence();
        tpf->sum += __rdtscp(&dummy) - tpf->recent;
        tpf->recent = 0;
        tpf->count ++;
        _mm_mfence();
    }
#endif
}

void _profiling_print_time(struct time_per_function tpf) {
    
    if (tpf.count == 0) {
        printf("%15llu\n", tpf.sum);
        return;
    }
    printf("%15llu\t%10d\t%15llu\n", tpf.sum, tpf.count, tpf.sum / (unsigned long long)(tpf.count));
}

void profiling_print_times() {
    printf("Counter Info\n");
    fflush(stdout);
    

    printf("%-40s","profiling_main");
    _profiling_print_time(profiling_data.profiling_main);

        printf("%-40s",">quicly_select");
        _profiling_print_time(profiling_data.quicly_select);
        printf("%-40s",">quicly_read");
        _profiling_print_time(profiling_data.quicly_read);
            printf("%-40s", ">>quicly_recvmsg");
            _profiling_print_time(profiling_data.quicly_recvmsg);
            printf("%-40s", ">>quicly_decode_packet");
            _profiling_print_time(profiling_data.quicly_decode_packet);
            printf("%-40s", ">>quicly_receive");
            _profiling_print_time(profiling_data.quicly_receive);
                printf("%-40s", ">>>decrypt_packet");
                _profiling_print_time(profiling_data.decrypt_packet);
                printf("%-40s", ">>>handle_payload");
                _profiling_print_time(profiling_data.handle_payload);
                    printf("%-40s", ">>>>handle_stream_frame");
                    _profiling_print_time(profiling_data.handle_stream_frame);
                        printf("%-40s", ">>>>>quicly_recvbuf_shift");
                        _profiling_print_time(profiling_data.quicly_recvbuf_shift);
                    printf("%-40s", ">>>>handle_ack_frame");
                    _profiling_print_time(profiling_data.handle_ack_frame);

        printf("%-40s",">quicly_send_pending_wrapper");
        _profiling_print_time(profiling_data.quicly_send_pending_wrapper);
            printf("%-40s",">>quicly_send_pending");
            _profiling_print_time(profiling_data.quicly_send_pending);
                printf("%-40s", ">>>quicly_sendmsg");
                _profiling_print_time(profiling_data.quicly_sendmsg);
                printf("%-40s", ">>>do_send");
                _profiling_print_time(profiling_data.do_send);
                    printf("%-40s",">>>>quicly_send_stream");
                    _profiling_print_time(profiling_data.quicly_send_stream);
                        printf("%-40s",">>>>>allocate_ack_eliciting_frame");
                        _profiling_print_time(profiling_data.allocate_ack_eliciting_frame);
                            printf("%-40s",">>>>>>aead_do_encrypt");
                            _profiling_print_time(profiling_data.aead_do_encrypt);
}

void profiling_clear_data() {
    memset(&profiling_data, 0, sizeof(profiling_data));
}