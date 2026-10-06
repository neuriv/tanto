static void native_string_playback(const unsigned* slots, unsigned count, unsigned stance, unsigned family, bool custom=false) {
    // Exercise the setter and resource ownership, including same-key Ishida phases.
    // Native selection of each follow-up's timing/input row is covered separately.
    static uint8_t quick[0xD0],payload[0xB0];
    const uint32_t quick_keys[]={0xCB3,0xC76,0xCF0};
    const int32_t quick_motions[]={3100,2100,4100};
    memset(quick,0,sizeof(quick));memset(payload,0,sizeof(payload));quick[0x40]=1;
    put(quick,0,quick_keys[stance]);put(quick,0x20,address(payload));
    put(payload,0x18,uint64_t(0x8000000594C0000ULL));put(payload,0x20,quick_motions[stance]);
    const auto lookup=original_lookup;
    original_lookup=[](void* context,uint32_t key,uint32_t* index)->uint64_t {
        if (same_field(address(quick),0,key)) {if (index) *index=0;SetLastError(ACTION_ERROR);return address(quick);}
        return native_lookup(context,key,index);
    };
    bindings(false);boss_active=0;
    for (auto& action : boss_private_actions) action={};
    put(player.data(),0x58,address(neutral.data()));put(player.data(),0x470,stance);
    const auto fallbacks=native_idle_fallbacks;
    const uint32_t entry=family==1 ? quick_keys[stance] : boss_adapters[slots[0]].player_key;
    assert(observed_action(player.data(),entry,nullptr));
    assert(boss_active && boss_active_slot==slots[0] && same_field(address(player.data()),0x58,boss_private_descriptor_address(slots[0])));
    if (custom) for (auto& binding : boss_skill_bindings) binding={};
    for (unsigned cycle=0;cycle<2;++cycle) for (unsigned phase=0;phase<count;++phase) {
        const unsigned next=slots[(phase+1)%count];
        const bool accepted=observed_action(player.data(),boss_imports[next].key,nullptr);
        if (!accepted) std::fprintf(stderr,"String root=%u phase=%u next=%u key=%X reason=%ld active=%ld slot=%u\n",
            slots[0],phase,next,boss_imports[next].key,long(dispatch->control.last_reason),long(boss_active),boss_active_slot);
        assert(accepted);
        assert(boss_active && boss_active_slot==next && same_field(address(player.data()),0x58,boss_private_descriptor_address(next)));
        assert(native_idle_fallbacks==fallbacks && GetLastError()==ACTION_ERROR);
    }
    assert(observed_action(player.data(),0xD5F,nullptr) && !boss_active);bindings(false);
    original_lookup=lookup;++checks;
}
