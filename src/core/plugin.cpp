#include <core/plugin.h>
#include <core/debug.h>

namespace lsp
{
    plugin_t::plugin_t(const plugin_metadata_t &mdata)
    {
        pMetadata       = &mdata;
        fSampleRate     = -1;
        nLatency        = 0;
    }

    plugin_t::~plugin_t()
    {
    }

    void plugin_t::init()
    {
    }

    void plugin_t::set_sample_rate(long sr)
    {
        if (fSampleRate != sr)
        {
            fSampleRate = sr;
            update_sample_rate(sr);
        }
    };

    void plugin_t::update_sample_rate(long sr)
    {
    }

    void plugin_t::activate()
    {
    }

    void plugin_t::deactivate()
    {
    }

    void plugin_t::destroy()
    {
        vPorts.clear();
    }
    
    void plugin_t::update_settings()
    {
    }

    void plugin_t::process(size_t samples)
    {
    }

}

