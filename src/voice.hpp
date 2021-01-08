#include <q/synth/envelope.hpp>
#include <q/fx/lowpass.hpp>
#include <q/support/literals.hpp>
#include <q/synth/sin.hpp>

namespace q = cycfi::q;
using namespace q::literals;

struct voice {
    voice() : phase_iterator(1, 1), envelope1(1), envelope2(1), filter(20000, 0.707, 40000)
    {};

    voice(q::frequency freq, uint32_t spr, q::envelope::config envelope1_cfg, q::envelope::config envelope2_cfg, 
        float filter_cutoff, float filter_q, float filter_env_mod) :
        phase_iterator(freq, spr), envelope1(envelope1_cfg, spr), 
        envelope2(envelope2_cfg, spr), filter(filter_cutoff, filter_q, spr)
    {};

    q::phase_iterator phase_iterator;
    q::envelope envelope1;
    q::envelope envelope2;
    q::reso_filter filter;
};