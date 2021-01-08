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
#include "voice.hpp"

namespace q = cycfi::q;
using namespace q::literals;

struct osc
{
    osc(uint32_t sampling_rate, float pitch_scale, q::envelope::config env1_cfg, 
        q::envelope::config env2_cfg, int type, float filter_cutoff, float filter_q, float filter_env_mod)
    {
        this->sampling_rate = sampling_rate;
        this->pitch_scale = pitch_scale;
        this->env1_cfg = env1_cfg;
        this->env2_cfg = env2_cfg;
        this->filter_cutoff = filter_cutoff;
        this->filter_q = filter_q;
        this->filter_env_mod = filter_env_mod;

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
            keys[i.first].envelope2();
            frame += keys[i.first].filter(osc_func(keys[i.first].phase_iterator++) * keys[i.first].envelope1());
        }
        return frame / MAX_VOICES;
    }

    void process_filters() {
        for (auto i : keys) {
            keys[i.first].filter.cutoff(keys[i.first].envelope2());
        }
    }

    void add_voice(uint8_t key)
    {
        if (keys.size() == MAX_VOICES && !releasing.size())
        {
            if (releasing.size())
            {
                keys.erase(releasing.front());
                releasing.pop();
            }
            else {
                return;
            }
        }
        keys[key] = voice(q::midi::note_frequency(key) * pitch_scale, this->sampling_rate, env1_cfg,
            env2_cfg, filter_cutoff, filter_q, filter_env_mod);
        keys[key].envelope1.trigger(1.0f);
        keys[key].envelope2.trigger(1.0f);
    }

    void release_voice(uint8_t key)
    {
        if (keys.find(key) == keys.end())
        {
            return;
        }
        releasing.push(key);
        keys[key].envelope1.release();
        keys[key].envelope2.release();
    }

    void process_remove()
    {
        if (releasing.size())
        {
            while (keys[releasing.front()].envelope1.state() == 0)
            {
                keys.erase(releasing.front());
                releasing.pop();
                if (!releasing.size()) {
                    break;
                }
            }
        }
    };

    q::envelope::config env1_cfg;
    q::envelope::config env2_cfg;
    std::unordered_map<uint8_t, struct voice> keys;
    std::queue<uint8_t> releasing;
    uint32_t sampling_rate;
    float pitch_scale;
    float filter_cutoff, filter_q, filter_env_mod;
    std::function < float (q::phase_iterator) > osc_func;
};