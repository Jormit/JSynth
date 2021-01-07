#include <set>
#include <unordered_map>
#include <q/support/literals.hpp>
#include <q/synth/saw.hpp>
#include <q/synth/sin.hpp>
#include <q/synth/square.hpp>
#include <q/synth/triangle.hpp>
#include <q_io/midi_stream.hpp>
#include <q/synth/envelope.hpp>

#include "config.hpp"

namespace q = cycfi::q;
using namespace q::literals;

float null_osc(q::phase_iterator phase) {
    return 0.0f;
}

struct voice {
    voice() : phase_iterator(1, 1), envelope(1)
    {};

    voice(q::frequency freq, uint32_t spr, q::envelope::config envelope_cfg) :
        phase_iterator(freq, spr), envelope(envelope_cfg, spr)
    {};

    q::phase_iterator phase_iterator;
    q::envelope envelope;
};

struct osc
{
    osc(uint32_t sampling_rate, float pitch_scale, q::envelope::config env_cfg, int type)
    {
        this->sampling_rate = sampling_rate;
        this->pitch_scale = pitch_scale;
        this->env_cfg = env_cfg;
        
    }

    float process_frame()
    {
        float frame = 0.0f;
        for (auto i : keys)
        {
            frame += q::saw(keys[i.first].phase_iterator++) * keys[i.first].envelope();
        }
        return frame / MAX_VOICES;
    }

    void add_voice(uint8_t key)
    {
        if (keys.size() == MAX_VOICES)
        {
            return;
        }
        keys[key] = voice(q::midi::note_frequency(key) * pitch_scale, this->sampling_rate, env_cfg);
        keys[key].envelope.trigger(1.0f);
    }

    void remove_voice(uint8_t key)
    {
        if (keys.find(key) == keys.end())
        {
            return;
        }
        to_delete.insert(key);
    }

    void process_remove()
    {
        for (auto elem : to_delete)
        {
            keys.erase(elem);
        }
        to_delete.clear();
    };

    q::envelope::config env_cfg;
    std::unordered_map<uint8_t, struct voice> keys; // Current Voice storage.
    std::set<uint8_t> to_delete; // Delete buffer to avoid deleting mid iteration.
    uint32_t sampling_rate;
    float pitch_scale;
    float (*osc_func)(q::phase_iterator);
};