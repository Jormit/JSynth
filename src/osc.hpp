/************************************************
 * osc.hpp:
 * Contains the oscillator classes which handle
 * management of current notes being played and
 * audio output.
 ***********************************************/

#include <functional>
#include <set>
#include <queue>
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

        switch (type)
        {
        case NONE:
            osc_func = [](q::phase_iterator a) { return 0.0f; };
            break;
        case SINE:
            osc_func = q::sin;
            break;
        case SAW:
            osc_func = q::saw;
            break;
        case SQR:
            osc_func = q::square;
            break;
        case TRI:
            osc_func = q::triangle;
            break;
        }        
    }

    float process_frame()
    {
        float frame = 0.0f;
        for (auto i : keys)
        {
            frame += osc_func(keys[i.first].phase_iterator++) * keys[i.first].envelope();
        }
        return frame / MAX_VOICES;
    }

    void add_voice(uint8_t key)
    {
        if (keys.size() == MAX_VOICES && !releasing.size())
        {          
            return;
        }
        keys[key] = voice(q::midi::note_frequency(key) * pitch_scale, this->sampling_rate, env_cfg);
        keys[key].envelope.trigger(1.0f);
    }

    void release_voice(uint8_t key)
    {
        if (keys.find(key) == keys.end())
        {
            return;
        }
        releasing.push(key);
        keys[key].envelope.release();
    }

    void process_remove()
    {
        if (releasing.size())
        {
            while (keys[releasing.front()].envelope.state() == 0)
            {
                keys.erase(releasing.front());
                releasing.pop();
                if (!releasing.size()) {
                    break;
                }
            }
        }        
    };

    q::envelope::config env_cfg;
    std::unordered_map<uint8_t, struct voice> keys;
    std::queue<uint8_t> releasing;
    uint32_t sampling_rate;
    float pitch_scale;
    std::function < float (q::phase_iterator) > osc_func;
};