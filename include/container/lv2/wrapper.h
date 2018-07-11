/*
 * wrapper.h
 *
 *  Created on: 12 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_WRAPPER_H_
#define CONTAINER_LV2_WRAPPER_H_

#include <data/cvector.h>
#include <data/chashmap.h>

#include <core/IWrapper.h>
#include <core/NativeExecutor.h>

namespace lsp
{
    /** LV2 common plugin wrapper class
     *
     */
    class LV2Wrapper: public IWrapper
    {
        private:
            cvector<LV2Port>    vExtPorts;
            cvector<LV2Port>    vAllPorts;      // List of all created ports, for garbage collection
            cvector<LV2Port>    vPluginPorts;   // All plugin ports sorted in urid order
            cvector<LV2Port>    vMeshPorts;
            cvector<port_t>     vGenMetadata;   // Generated metadata

            plugin_t           *pPlugin;
            LV2Extensions      *pExt;
            IExecutor          *pExecutor;      // Executor service
            void               *pAtomIn;        // Atom input port
            void               *pAtomOut;       // Atom output port
            float              *pLatency;       // Latency output port
            size_t              nPatchReqs;     // Number of patch requests
            size_t              nStateReqs;     // Number of state requests
            ssize_t             nSyncTime;      // Synchronization time
            ssize_t             nSyncSamples;   // Synchronization counter
            ssize_t             nClients;       // Number of clients

        protected:
            LV2Port *create_port(const port_t *meta, const char *postfix);
            void create_ports(const port_t *meta);

            void receive_atoms(size_t samples);
            void transmit_atoms(size_t samples);
            static LV2Port *find_by_urid(cvector<LV2Port> &v, LV2_URID urid);
            static void sort_by_urid(cvector<LV2Port> &v);

        public:
            inline explicit LV2Wrapper(plugin_t *plugin, LV2Extensions *ext)
            {
                pPlugin     = plugin;
                pExt        = ext;
                pExecutor   = NULL;
                pAtomIn     = NULL;
                pAtomOut    = NULL;
                pLatency    = NULL;
                nPatchReqs  = 0;
                nStateReqs  = 0;
                nSyncTime   = 0;
                nSyncSamples= 0;
                nClients    = 0;
            }

            virtual ~LV2Wrapper()
            {
                pPlugin     = NULL;
                pExt        = NULL;
                pExecutor   = NULL;
                pAtomIn     = NULL;
                pAtomOut    = NULL;
                pLatency    = NULL;
                nPatchReqs  = 0;
                nStateReqs  = 0;
                nSyncTime   = 0;
                nSyncSamples= 0;
                nClients    = 0;
            }

        public:
            // Basic LV2 part
            void init(float srate);
            void destroy();

            inline void activate()      {   pPlugin->activate();    }
            inline void deactivate()    {   pPlugin->deactivate();  }

            inline void connect(size_t id, void *data);
            inline void run(size_t samples);

            // State part
            inline void save_state(
                LV2_State_Store_Function   store,
                LV2_State_Handle           handle,
                uint32_t                   flags,
                const LV2_Feature *const * features
            );

            inline void restore_state(
                LV2_State_Retrieve_Function retrieve,
                LV2_State_Handle            handle,
                uint32_t                    flags,
                const LV2_Feature *const *  features
            );

            // Job part
            virtual IExecutor *get_executor();

            inline void job_run(
                LV2_Worker_Respond_Handle   handle,
                LV2_Worker_Respond_Function respond,
                uint32_t                    size,
                const void*                 data
            )
            {
                LV2Executor *executor = static_cast<LV2Executor *>(pExecutor);
                executor->run_job(handle, respond, size, data);
            }

            inline void job_response(size_t size, const void *body) {}
            inline void job_end() {}

    };

    LV2Port *LV2Wrapper::create_port(const port_t *p, const char *postfix)
    {
        LV2Port *result = NULL;

        switch (p->role)
        {
            case R_MESH:
                if (pExt->atom_supported())
                {
                    result = new LV2MeshPort(p, pExt);
                    vMeshPorts.add(result);
                }
                else
                    result = new LV2Port(p, pExt);
                break;
            case R_MIDI:
                if (pExt->atom_supported())
                {
                    if (IS_OUT_PORT(p))
                        result      = new LV2MidiOutputPort(p, pExt);
                    else
                        result      = new LV2MidiInputPort(p, pExt);
                }
                else
                    result = new LV2Port(p, pExt);
                break;
            case R_PATH:
                if (pExt->atom_supported())
                    result      = new LV2PathPort(p, pExt);
                else
                    result      = new LV2Port(p, pExt);
                break;

            case R_AUDIO:
                result      = new LV2AudioPort(p, pExt);
                break;

            case R_PORT_SET:
            {
                char postfix_buf[LSP_MAX_PARAM_ID_BYTES];
                LV2PortGroup   *pg      = new LV2PortGroup(p, pExt);
                pPlugin->add_port(pg);
                vPluginPorts.add(pg);

                for (size_t row=0; row<pg->rows(); ++row)
                {
                    // Generate postfix
                    snprintf(postfix_buf, sizeof(postfix_buf)-1, "%s_%d", (postfix != NULL) ? postfix : "", int(row));

                    // Clone port metadata
                    port_t *cm          = clone_port_metadata(p->members, postfix_buf);
                    if (cm == NULL)
                        continue;

                    vGenMetadata.add(cm);

                    size_t col          = 0;
                    for (; cm->id != NULL; ++cm, ++col)
                    {
                        if (IS_GROWING_PORT(cm))
                            cm->start    = cm->min + ((cm->max - cm->min) * row) / float(pg->rows());
                        else if (IS_LOWERING_PORT(cm))
                            cm->start    = cm->max - ((cm->max - cm->min) * row) / float(pg->rows());

                        LV2Port *p          = create_port(cm, postfix_buf);
                        if ((p != NULL) && (p->metadata()->role != R_PORT_SET))
                        {
                            vPluginPorts.add(p);
                            pPlugin->add_port(p);
                        }
                    }
                }

                result  = pg;
                break;
            }

            case R_CONTROL:
            case R_METER:
                if (IS_OUT_PORT(p))
                    result      = new LV2OutputPort(p, pExt);
                else
                    result      = new LV2InputPort(p, pExt);
                break;

            default:
                break;
        }

        // Register created port for garbage collection
        if (result != NULL)
            vAllPorts.add(result);

        return result;
    }

    void LV2Wrapper::create_ports(const port_t *meta)
    {
        for (const port_t *port = meta; port->id != NULL; ++port)
        {
            // Create port
            LV2Port *p = create_port(port, NULL);
            if (p == NULL)
                continue;

            // Register port
            switch (port->role)
            {
                case R_PORT_SET:
                    break;

                case R_MESH:
                case R_PATH:
                    pPlugin->add_port(p);
                    vPluginPorts.add(p);
                    break;

                case R_AUDIO:
                case R_MIDI:
                case R_CONTROL:
                case R_METER:
                    p->set_id(pPlugin->ports_count());
                    pPlugin->add_port(p);
                    vPluginPorts.add(p);
                    vExtPorts.add(p);
                    lsp_trace("Added external port id=%s, external_id=%d", p->metadata()->id, int(vExtPorts.size() - 1));
                    break;

                default:
                    break;
            }
        }
    }

    void LV2Wrapper::sort_by_urid(cvector<LV2Port> &v)
    {
        ssize_t items = v.size();
        if (items < 2)
            return;

        for (ssize_t i=0; i<(items-1); ++i)
            for (ssize_t j=i+1; j<items; ++j)
                if (v.at(i)->get_urid() > v.at(j)->get_urid())
                    v.swap(i, j);
    }

    void LV2Wrapper::init(float srate)
    {
        // Get plugin metadata
        const plugin_metadata_t *m  = pPlugin->get_metadata();

        // Bind ports
        lsp_trace("Binding ports");
        create_ports(m->ports);

        // Sort port lists
        sort_by_urid(vPluginPorts);
        sort_by_urid(vMeshPorts);

        // Initialize plugin
        lsp_trace("Initializing plugin");
        pPlugin->init(this);
        pPlugin->set_sample_rate(srate);
        nSyncSamples        = srate / MESH_REFRESH_RATE;
        nClients            = 0;
    }

    LV2Port *LV2Wrapper::find_by_urid(cvector<LV2Port> &v, LV2_URID urid)
    {
        // Try to find the corresponding port
        ssize_t first = 0, last = v.size() - 1;
        while (first <= last)
        {
            size_t center   = (first + last) >> 1;
            LV2Port *p      = v[center];
            if (urid == p->get_urid())
                return p;
            else if (urid < p->get_urid())
                last    = center - 1;
            else
                first   = center + 1;
        }
        return NULL;
    }

    void LV2Wrapper::receive_atoms(size_t samples)
    {
        // Update synchronization
        if (nSyncTime <= 0)
        {
            size_t n_ports  = vMeshPorts.size();
            for (size_t i=0; i<n_ports; ++i)
            {
                mesh_t *mesh    = vMeshPorts[i]->getBuffer<mesh_t>();
//                lsp_trace("Mesh id=%s is waiting=%s", vMeshPorts[i]->metadata()->id, (mesh->isWaiting()) ? "true" : "false");
                if ((mesh != NULL) && (mesh->isWaiting()))
                    mesh->cleanup();
            }
        }

        // Get sequence
        if (pAtomIn == NULL)
            return;
        const LV2_Atom_Sequence *seq = reinterpret_cast<const LV2_Atom_Sequence *>(pAtomIn);

//        lsp_trace("pSequence->atom.type (%d) = %s", int(pSequence->atom.type), pExt->unmap_urid(pSequence->atom.type));
//        lsp_trace("pSequence->atom.size = %d", int(pSequence->atom.size));

        for (
            const LV2_Atom_Event* ev = lv2_atom_sequence_begin(&seq->body);
            !lv2_atom_sequence_is_end(&seq->body, seq->atom.size, ev);
            ev = lv2_atom_sequence_next(ev)
        )
        {
            lsp_trace("ev->body.type (%d) = %s", int(ev->body.type), pExt->unmap_urid(ev->body.type));

            // If the event is an object
            if (ev->body.type != pExt->uridObject)
                continue;

            // Analyze object type
            const LV2_Atom_Object *obj = reinterpret_cast<const LV2_Atom_Object*>(&ev->body);
            lsp_trace("obj->body.otype (%d) = %s", int(obj->body.otype), pExt->unmap_urid(obj->body.otype));
            lsp_trace("obj->body.id (%d) = %s", int(obj->body.id), pExt->unmap_urid(obj->body.id));

            if ((obj->body.id == pExt->uridState) && (obj->body.otype == pExt->uridStateChange)) // State change
            {
                for (
                    LV2_Atom_Property_Body *body = lv2_atom_object_begin(&obj->body) ;
                    !lv2_atom_object_is_end(&obj->body, obj->atom.size, body) ;
                    body = lv2_atom_object_next(body)
                )
                {
                    lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                    lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                    // Try to find the corresponding port
                    LV2Port *p = find_by_urid(vPluginPorts, body->key);
                    if ((p != NULL) && (p->get_type_urid() == body->value.type))
                        p->deserialize(&body->value);
                }
            }
            else if ((obj->body.id == pExt->uridState) && (obj->body.otype == pExt->uridStateRequest)) // State request
            {
                lsp_trace("triggered state request");
                nStateReqs  ++;
            }
            else if ((obj->body.id == pExt->uridChunk) && (obj->body.otype == pExt->uridPatchGet)) // PatchGet request
            {
                lsp_trace("triggered patch request");
                LV2_Atom_Property_Body *body    = lv2_atom_object_begin(&obj->body);

                while (!lv2_atom_object_is_end(&obj->body, obj->atom.size, body))
                {
                    lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                    lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                    body = lv2_atom_object_next(body);
                }

                // Increment the number of patch requests
                nPatchReqs  ++;
            }
            else if ((obj->body.id == pExt->uridChunk) && (obj->body.otype == pExt->uridPatchSet)) // PatchSet request
            {
                // Parse atom body
                const LV2_Atom_URID    *key     = NULL;
                const LV2_Atom         *value   = NULL;

                for (
                    LV2_Atom_Property_Body *body = lv2_atom_object_begin(&obj->body) ;
                    !lv2_atom_object_is_end(&obj->body, obj->atom.size, body) ;
                    body = lv2_atom_object_next(body)
                )
                {
                    lsp_trace("body->key (%d) = %s", int(body->key), pExt->unmap_urid(body->key));
                    lsp_trace("body->value.type (%d) = %s", int(body->value.type), pExt->unmap_urid(body->value.type));

                    if ((body->key  == pExt->uridPatchProperty) && (body->value.type == pExt->uridAtomUrid))
                    {
                        key     = reinterpret_cast<const LV2_Atom_URID *>(&body->value);
                        lsp_trace("body->value.body (%d) = %s", int(key->body), pExt->unmap_urid(key->body));
                    }
                    else if (body->key   == pExt->uridPatchValue)
                        value   = &body->value;

                    if ((key != NULL) && (value != NULL))
                    {
                        LV2Port *p = find_by_urid(vPluginPorts, body->value.type);
                        if ((p != NULL) && (p->get_type_urid() == value->type))
                            p->deserialize(value);

                        key     = NULL;
                        value   = NULL;
                    }
                }
            }
            else if ((obj->body.id == pExt->uridConnectUI) && (obj->body.otype == pExt->uridUINotification))
            {
                nClients    ++;
                lsp_trace("UI has connected, current number of clients=%d", int(nClients));
            }
            else if ((obj->body.id == pExt->uridDisconnectUI) && (obj->body.otype == pExt->uridUINotification))
            {
                nClients    --;
                lsp_trace("UI has disconnected, current number of clients=%d", int(nClients));
            }
        }
    }

    void LV2Wrapper::transmit_atoms(size_t samples)
    {
        // Get sequence
        if (pAtomOut == NULL)
            return;

        // Update synchronization time
        nSyncTime      -= samples;
        bool sync_req       = nSyncTime <= 0;
        if (sync_req)
            nSyncTime      += nSyncSamples;

        // Check that patch request is pending
        bool patch_req  = nPatchReqs > 0;
        if (patch_req)
            nPatchReqs      --;

        // Check that state request is pending
        bool state_req  = nStateReqs > 0;
        if (state_req)
            nStateReqs      --;

        size_t bytes_out    = 0;

        // Initialize forge
        LV2_Atom_Sequence *sequence = reinterpret_cast<LV2_Atom_Sequence *>(pAtomOut);
        pExt->forge_set_buffer(sequence, sequence->atom.size);

        // Forge sequence header
        LV2_Atom_Forge_Frame    seq;
        LV2_Atom_Forge_Frame    frame;
        pExt->forge_sequence_head(&seq, 0);

        // Serialize paths that are visible in global space
        size_t n_ports      = vExtPorts.size();
        for (size_t i=0; i<n_ports; ++i)
        {
            // Get port
            LV2Port *p = vExtPorts[i];
            if ((p == NULL) || (p->metadata()->role != R_PATH))
                continue;
            if (p->get_id() < 0) // Non-global paths are serialized via STATE CHANGE primitive
                continue;

            // Check that we need to transmit the value
            if ((!patch_req) && (!p->tx_pending()))
                continue;

            // Serialize path as patch
            lsp_trace("Serialize path id=%s, bytes_out=%d", p->metadata()->id, int(bytes_out));
            pExt->forge_frame_time(0); // Event header
            LV2_Atom *msg = pExt->forge_object(&frame, pExt->uridPatchMessage, pExt->uridPatchSet);
            pExt->forge_key(pExt->uridPatchProperty);
            pExt->forge_urid(p->get_urid());
            pExt->forge_key(pExt->uridPatchValue);
            p->serialize();
            pExt->forge_pop(&frame);
            bytes_out   += lv2_atom_total_size(msg);
        }

        // Allow transport only when there is at least one UI connected
        if (nClients > 0)
        {
            // Serialize pending for transmission ports
            bytes_out           = 0;
            n_ports             = vPluginPorts.size();
            LV2_Atom *msg       = NULL;

            for (size_t i=0; i<n_ports; ++i)
            {
                // Get port
                LV2Port *p = vPluginPorts[i];
                if (p == NULL)
                    continue;

                // Skip MESH ports and PATH ports visible in global space
                switch (p->metadata()->role)
                {
                    case R_AUDIO:
                    case R_MIDI:
                    case R_UI_SYNC:
                    case R_MESH:
                        continue;
                    case R_PATH:
                        if (p->get_id() >= 0) // Skip global PATH ports
                            continue;
                        break;
                    default:
                        break;
                }

                // Check that we need to transmit the value
                if ((!state_req) && (!p->tx_pending()))
                    continue;

                // Serialize value of the port
                lsp_trace("Serialize port id=%s, bytes_out=%d", p->metadata()->id, int(bytes_out));

                // Emit object header (if needed)
                if (msg == NULL)
                {
                    pExt->forge_frame_time(0);
                    msg         = pExt->forge_object(&frame, pExt->uridState, pExt->uridStateChange);
                }

                pExt->forge_key(p->get_urid());
                p->serialize();
                bytes_out       = lv2_atom_total_size(msg);

                // Emit object tail (if needed)
                if (bytes_out >= 0x1000)
                {
                    pExt->forge_pop(&frame);
                    msg         = NULL;
                    bytes_out   = 0;
                }
            }

            // Emit object tail (if needed)
            if (msg != NULL)
                pExt->forge_pop(&frame);

            // Serialize meshes (it's own primitive MESH)
            n_ports         = vMeshPorts.size();
            for (size_t i=0; i<n_ports; ++i)
            {
                LV2Port *p = vMeshPorts[i];
                if (p == NULL)
                    continue;
                if ((!sync_req) && (!p->tx_pending()))
                    continue;
                mesh_t *mesh = p->getBuffer<mesh_t>();
                if ((mesh == NULL) || (!mesh->containsData()))
                    continue;

                pExt->forge_frame_time(0);  // Event header
                msg         = pExt->forge_object(&frame, p->get_urid(), pExt->uridMeshType);
                p->serialize();
                pExt->forge_pop(&frame);
                bytes_out   += lv2_atom_total_size(msg);

                // Cleanup data of the mesh for refill
                mesh->cleanup();
            }
        }

        // Complete sequence
        pExt->forge_pop(&seq);

//        lsp_trace("Total bytes transferred = %d", int(bytes_out));
    }

    void LV2Wrapper::destroy()
    {
        // Shutdown and delete executor if exists
        if (pExecutor != NULL)
        {
            pExecutor->shutdown();
            delete pExecutor;
            pExecutor   = NULL;
        }

        // Drop plugin
        if (pPlugin != NULL)
        {
            pPlugin->destroy();
            delete pPlugin;

            pPlugin     = NULL;
        }

        // Cleanup ports
        for (size_t i=0; i < vAllPorts.size(); ++i)
        {
            lsp_trace("destroy port id=%s", vAllPorts[i]->metadata()->id);
            delete vAllPorts[i];
        }

        // Cleanup generated metadata
        for (size_t i=0; i<vGenMetadata.size(); ++i)
        {
            lsp_trace("destroy generated port metadata %p", vGenMetadata[i]);
            drop_port_metadata(vGenMetadata[i]);
        }

        vAllPorts.clear();
        vExtPorts.clear();
        vMeshPorts.clear();
        vPluginPorts.clear();
        vGenMetadata.clear();

        // Drop extensions
        if (pExt != NULL)
        {
            delete pExt;
            pExt        = NULL;
        }
    }

    inline void LV2Wrapper::connect(size_t id, void *data)
    {
        size_t ports_count  = vExtPorts.size();
        if (id < ports_count)
        {
            LV2Port *p      = vExtPorts[id];
            if (p != NULL)
                p->bind(data);
        }
        else
        {
            switch (id - ports_count)
            {
                case 0: pAtomIn     = data; break;
                case 1: pAtomOut    = data; break;
                case 2: pLatency    = reinterpret_cast<float *>(data); break;
                default:
                    lsp_warn("Unknown bind port: %d", int(id));
                    break;
            }
        }
    }

    inline void LV2Wrapper::run(size_t samples)
    {
        // Activate/deactivate the UI
        if (nClients > 0)
        {
            if (!pPlugin->ui_active())
                pPlugin->activate_ui();
        }
        else if (pPlugin->ui_active())
            pPlugin->deactivate_ui();

        // Port update flag
        bool update     = false;

        // First pre-process transport ports
        receive_atoms(samples);

        // Pre-rocess regular ports
        size_t n_all_ports = vAllPorts.size();
        for (size_t i=0; i<n_all_ports; ++i)
        {
            // Get port
            LV2Port *port = vAllPorts.at(i);
            if (port == NULL)
                continue;

            // Pre-process data in port
            if (port->pre_process(samples))
            {
                lsp_trace("port changed: %s", port->metadata()->id);
                update = true;
            }
        }

        // Check that input parameters have changed
        if (update)
        {
            lsp_trace("updating settings");
            pPlugin->update_settings();
        }

        // Call the main processing unit
        pPlugin->process(samples);

        // Transmit atoms (if possible)
        transmit_atoms(samples);

        // Post-process regular ports for changes
        for (size_t i=0; i<n_all_ports; ++i)
        {
            LV2Port *port = vAllPorts.at(i);
            if (port != NULL)
                port->post_process(samples);
        }

        // Transmit latency (if possible)
        if (pLatency != NULL)
            *pLatency   = pPlugin->get_latency();
    }

    IExecutor *LV2Wrapper::get_executor()
    {
        lsp_trace("executor = %p", reinterpret_cast<void *>(pExecutor));
        if (pExecutor != NULL)
            return pExecutor;

        // Create executor service
        if (pExt->sched != NULL)
        {
            lsp_trace("Creating LV2 executor service");
            pExecutor       = new LV2Executor(pExt->sched);
        }
        else
        {
            lsp_trace("Creating native executor service");
            pExecutor       = new NativeExecutor();
        }
        return pExecutor;
    }

    inline void LV2Wrapper::save_state(
        LV2_State_Store_Function   store,
        LV2_State_Handle           handle,
        uint32_t                   flags,
        const LV2_Feature *const * features)
    {
        pExt->init_state_context(store, NULL, handle, flags, features);

        size_t ports_count = vAllPorts.size();

        for (size_t i=0; i<ports_count; ++i)
        {
            // Get port
            LV2Port *lvp    = vAllPorts[i];
            if (lvp == NULL)
                continue;

            // Save state of port
            lvp->save();
        }

        pExt->reset_state_context();
    }

    inline void LV2Wrapper::restore_state(
        LV2_State_Retrieve_Function retrieve,
        LV2_State_Handle            handle,
        uint32_t                    flags,
        const LV2_Feature *const *  features
    )
    {
        pExt->init_state_context(NULL, retrieve, handle, flags, features);

        size_t ports_count = vAllPorts.size();

        for (size_t i=0; i<ports_count; ++i)
        {
            // Get port
            LV2Port *lvp    = vAllPorts[i];
            if (lvp == NULL)
                continue;

            // Restore state of port
            lvp->restore();
        }

        pExt->reset_state_context();
    }
}

#endif /* CONTAINER_LV2_WRAPPER_H_ */
