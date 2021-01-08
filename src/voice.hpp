#include <q/synth/envelope.hpp>
#include <q/fx/lowpass.hpp>
#include <q/support/literals.hpp>
#include <q/synth/sin.hpp>
#include <vector>

namespace q = cycfi::q;
using namespace q::literals;

struct voice {
    voice() : envelope1(1), envelope2(1), filter(20000, 0.707, 40000)
    {
    };

    voice(int num, const float *pitches, float freq, uint32_t spr, q::envelope::config envelope1_cfg,
        q::envelope::config envelope2_cfg, float filter_cutoff, float filter_q, float filter_env_mod) :
        envelope1(envelope1_cfg, spr), envelope2(envelope2_cfg, spr), filter(filter_cutoff, filter_q, spr)
    {
        for (int i = 0; i < num; i++) 
        {
            phase_iterators.push_back(q::phase_iterator(((double)pitches[i] * freq), spr));
        }
    };

    std::vector<q::phase_iterator> phase_iterators;
    q::envelope envelope1;
    q::envelope envelope2;
    q::reso_filter filter;
};