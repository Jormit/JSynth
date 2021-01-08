/************************************************
 * osc.hpp:
 * Contains the oscillator classes which handle
 * management of current notes being played and
 * audio output.
 ***********************************************/

#include <functional>
#include <vector>
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
    osc(int num, int max_voices, const int *types, const float *pitches, uint32_t sampling_rate,
        q::envelope::config env1_cfg, q::envelope::config env2_cfg, 
        float filter_cutoff, float filter_q, float filter_env_mod)
    {
        this->num = num;
        this->max_voices = max_voices;
        this->sampling_rate = sampling_rate;
        this->pitches = pitches;
        this->env1_cfg = env1_cfg;
        this->env2_cfg = env2_cfg;
        this->filter_cutoff = filter_cutoff;
        this->filter_q = filter_q;
        this->filter_env_mod = filter_env_mod;
        this->normalize_factor = (double) (num * max_voices);

        for (int i = 0; i < num; i++) {
            switch (types[i])
            {
            case NONE:
                osc_funcs.push_back([](q::phase_iterator a) { return 0.0f; });
                break;
            case SINE:
                osc_funcs.push_back(q::sin);
                break;
            case SAW:
                osc_funcs.push_back(q::saw);
                break;
            case SQR:
                osc_funcs.push_back(q::square);
                break;
            case TRI:
                osc_funcs.push_back(q::triangle);
                break;
            }
        }        
    }

    float process_frame()
    {
        float frame = 0.0f;
        for (auto key : keys)
        {
            float temp_frame = 0.0f;
            auto _env1 = keys[key.first].envelope1();
            auto _env2 = keys[key.first].envelope2();
            for (int i = 0; i < num; i++) {
                temp_frame += osc_funcs[i](keys[key.first].phase_iterators[i]++) * _env1;
            }
            frame += keys[key.first].filter(temp_frame);
        }
        return frame / (normalize_factor);
    }

    void process_filters() {
        for (auto i : keys) {
            keys[i.first].filter.cutoff(keys[i.first].envelope2());
        }
    }

    void add_voice(uint8_t key)
    {
        adding.push(key);                
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

    void process_add()
    {
       
        while (adding.size())
        {
            if (keys.size() == max_voices) {
                if (releasing.size()) {
                    keys.erase(releasing.front());
                }
                else {
                    adding = std::queue<uint8_t>();
                    return;
                }
            }
            uint8_t to_add = adding.front();
            adding.pop();
            keys[to_add] = voice(num, pitches, (float)q::midi::note_frequency(to_add), this->sampling_rate, env1_cfg,
                env2_cfg, filter_cutoff, filter_q, filter_env_mod);
            keys[to_add].envelope1.trigger(1.0f);
            keys[to_add].envelope2.trigger(1.0f);
        }

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

    int num;
    int max_voices;
    q::envelope::config env1_cfg;
    q::envelope::config env2_cfg;
    std::unordered_map<uint8_t, struct voice> keys;
    std::queue<uint8_t> releasing;
    std::queue<uint8_t> adding;
    uint32_t sampling_rate;
    const float *pitches;
    float filter_cutoff, filter_q, filter_env_mod;
    std::vector <std::function < float(q::phase_iterator) >> osc_funcs;
    float normalize_factor;
};