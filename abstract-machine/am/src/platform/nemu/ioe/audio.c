#include <am.h>
#include <nemu.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

static uint32_t audio_address = 0;
void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
    cfg->present = true;
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
    outl(AUDIO_FREQ_ADDR,ctrl->freq);
    outl(AUDIO_CHANNELS_ADDR,ctrl->channels);
    outl(AUDIO_SAMPLES_ADDR,ctrl->samples);
    outl(AUDIO_INIT_ADDR,1);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
    stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
    Area s = ctl->buf;
    uint32_t sbuf_size = inl(AUDIO_SBUF_SIZE_ADDR);
    uint8_t *ab = (uint8_t *)(uintptr_t)AUDIO_SBUF_ADDR;
    for(int i = 0; i < (s.end-s.start); i++){
        ab[audio_address] = *((uint8_t *)(s.start)+i);
        audio_address= (audio_address+ 1) % sbuf_size;
    }
    outl(AUDIO_COUNT_ADDR, inl(AUDIO_COUNT_ADDR) + (s.end-s.start));
}
