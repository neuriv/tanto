static void ishida_transition_cases() {
    // Repeated C5B and shared endings must follow the selected route, on either attack button.
    constexpr unsigned keys[]={0xC5B,0xC5B,0xC5C,0xC58, 0xC71,0xC72,0xC6E,
        0xC5C,0xC58,0xC7A, 0xC78,0xC5B,0xC5C,0xC58, 0xC6C,0xC6D,0xC71,0xC72,0xC6E};
    constexpr unsigned roots[]={0,4,7,10,14,19};
    static uint8_t descriptors[19][0xD0],payloads[19][0xB0],rows[19][40][0x30];
    static uint64_t pointers[19][40],entries[19];
    for (unsigned family : {1u,2u}) {
        replacement_reset();boss_import_count=21;
        const auto source=boss_imports[2];const auto player_adapter=boss_adapters[2];
        put(heavy_payloads[0].data(),0x18,uint64_t(0x8000000594C0000ULL));
        put(heavy_payloads[0].data(),0,uint64_t(0x20000000002ULL));
        put(heavy_payloads[0].data(),0x2A,int16_t(0));
        put(heavy_payloads[0].data(),0x2C,int16_t(1));
        for (unsigned route=0;route<5;++route) for (unsigned i=roots[route];i<roots[route+1];++i) {
            auto& move=boss_imports[i+2];move=source;move.key=keys[i];move.flags=0x10018480000ULL;
            move.recovery_frame=-1;move.next_variant=i+1<roots[route+1] ? int16_t(i+3) : -1;
            move.next_start=move.next_end=0;
            for (const auto& profile : cast_pulse_profiles) if (profile.key==move.key && profile.flags==move.flags) {
                move.motion=profile.motion;move.transition_count=profile.rows;
            }
            assert(ishida_phase(move));move.descriptor=address(descriptors[i]);move.payload=address(payloads[i]);
            boss_adapters[i+2]=player_adapter;boss_adapters[i+2].kind=i==roots[route] ? 2 : 4;
            boss_move_settings[i+2]={1,40,30,36,uint16_t(family)};boss_private_actions[i+2]={};
            memset(descriptors[i],0,0xD0);memset(payloads[i],0,0xB0);descriptors[i][0x40]=1;
            put(descriptors[i],0,move.key);put(descriptors[i],0x20,move.payload);
            put(descriptors[i],0x78,address(pointers[i]));put(descriptors[i],0x82,move.transition_count);
            put(payloads[i],0x18,move.flags);put(payloads[i],0x20,move.motion);put(payloads[i],0x24,int16_t(-1));
            // Actual archive fields: +28=-1, +2A=0, +2C=1, +2E=2 for every phase.
            // The 0.5.2 fixture invented set4/-1 and failed to exercise real data.
            put(payloads[i],0,uint64_t(0x10000010002ULL));
            put(payloads[i],0x28,int16_t(-1));put(payloads[i],0x2A,int16_t(0));
            put(payloads[i],0x2C,int16_t(1));put(payloads[i],0x2E,int16_t(2));
            put(payloads[i],0x40,int16_t(31));put(payloads[i],0x42,int16_t(0));put(payloads[i],0x44,int16_t(-1));
            put(payloads[i],0x9A,int16_t(29));put(payloads[i],0x9C,int16_t(0));put(payloads[i],0x9E,int16_t(-1));
            put(payloads[i],0x16,int16_t(0));entries[i]=move.descriptor;
            for (unsigned r=0;r<move.transition_count;++r) {memset(rows[i][r],0xff,0x30);pointers[i][r]=address(rows[i][r]);}
            rows[i][0][10]=2;rows[i][0][11]=0;rows[i][0][12]=1;
            put(rows[i][0],20,int16_t(move.next_variant>=0 ? keys[i+1] : keys[roots[route]]));
        }
        // Routes repeat keys, but the native source bank has one descriptor per key.
        // Aliases share that source and own distinct private actions/continuations.
        unsigned records=0;
        for (unsigned i=0;i<19;++i) {
            unsigned first=0;while (keys[first]!=keys[i]) ++first;
            if (first<i) {
                boss_imports[i+2].descriptor=boss_imports[first+2].descriptor;
                boss_imports[i+2].payload=boss_imports[first+2].payload;
            } else entries[records++]=boss_imports[i+2].descriptor;
        }
        put(jin_bank.data(),0x128,address(entries));put(jin_bank.data(),0x130,uint32_t(records));
        for (unsigned route=0;route<5;++route) for (unsigned i=roots[route];i<roots[route+1];++i) {
            boss_skill_bindings[0]={family==1 ? 5u : 1u,1,roots[route]+3,0xCF5,4300,46,0x8000000594C0000ULL};
            const unsigned next=i+1<roots[route+1] ? i+1 : roots[route];
            assert(boss_native_successor(i+2,keys[next])==int(next+2));
            assert(boss_prepare_private_action(i+2));
            const auto& clone=boss_private_actions[i+2];unsigned input_rows=0;
            uint64_t adapted_flags=0;memcpy(&adapted_flags,clone.payload+0x18,8);
            assert(adapted_flags==0x8000000594C0000ULL);
            assert(!memcmp(clone.payload+0x28,payloads[i]+0x28,8));
            assert(ishida_native_movement_mode(payloads[i])==1); // Reproduce the boss-only position policy.
            assert(ishida_native_movement_mode(heavy_payloads[0].data())==0);
            assert(ishida_native_movement_mode(clone.payload)==0); // Adapted entry and every phase stay player-owned.
            uint64_t movement=0;memcpy(&movement,clone.payload,8);
            assert(movement==0x10000000002ULL); // Only bit16 changes; the source action family remains intact.
            assert(!memcmp(clone.payload+0x40,payloads[i]+0x40,0x60));
            assert(!memcmp(clone.descriptor+0x48,descriptors[i]+0x48,0x30)); // Combat tables stay source-owned.
            int16_t onset=0,cost=0;memcpy(&onset,clone.payload+0x38,2);memcpy(&cost,clone.payload+0x16,2);
            assert(cost>0 && onset==ishida_phase(boss_imports[i+2])->next_frame);
            bool dodge=false;
            for (unsigned r=0;r<clone.transition_count;++r) {
                const auto* row=clone.transition_bodies[r];int16_t target=0;memcpy(&target,row+20,2);
                uint16_t condition=0;memcpy(&condition,row,2);
                if (condition==0x15) assert(target==-1); // Both wall-recoil rows are disabled.
                if (condition==0xA4) assert(target==0xD1F || target==0xD20); // Enemy guard deflection remains native.
                if (condition==0x34) assert(target==0xE0); // Death remains native.
                if (target==0xD12) {
                    int16_t gate=0;memcpy(&gate,row+32,2);assert(gate==onset);dodge=true;
                }
                if (target!=int16_t(keys[next])) continue;
                assert(row[10]==0 || row[10]==2);assert(row[11]==(family==1 ? 0 : 1));++input_rows;
            }
            assert(input_rows==2 && dodge);++checks;
        }
        for (unsigned route=0;route<5;++route) {
            boss_skill_bindings[0]={family==1 ? 5u : 1u,1,roots[route]+3,0xCF5,4300,46,0x8000000594C0000ULL};
            unsigned slots[5]{};const unsigned count=roots[route+1]-roots[route];
            for (unsigned i=0;i<count;++i) slots[i]=roots[route]+i+2;
            native_string_playback(slots,count,2,family);
        }
        // Colliding action IDs and other boss signatures keep William's ordinary recoil rows.
        boss_imports[2]=source;boss_adapters[2]=player_adapter;boss_private_actions[2]={};
        put(reinterpret_cast<void*>(source.payload),0,uint64_t(1u<<16)); // Other signatures retain their movement policy.
        assert(boss_prepare_private_action(2));unsigned recoil_rows=0;
        assert(ishida_native_movement_mode(boss_private_actions[2].payload)==1);
        for (unsigned r=0;r<boss_private_actions[2].transition_count;++r) {
            const auto* row=boss_private_actions[2].transition_bodies[r];uint16_t condition=0;int16_t target=0;
            memcpy(&condition,row,2);memcpy(&target,row+20,2);
            if (condition==0x15 && (target==0xD1F || target==0xD20)) ++recoil_rows;
        }
        assert(recoil_rows==2);++checks;
    }
}
