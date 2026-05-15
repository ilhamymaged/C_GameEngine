#include "AGmemory.h"

#include <platform/platform.h>
#include <core/logger.h>

#include "AGstring.h"
#include <stdio.h>

typedef struct memory_state {
    u64 total_allocated;
    u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
} memory_state;

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "ARRAY      ",
    "DARRAY     ",
    "DICT       ",
    "RING_QUEUE ",
    "BST        ",
    "STRING     ",
    "APPLICATION",
    "JOB        ",
    "TEXTURE    ",
    "MAT_INST   ",
    "RENDERER   ",
    "GAME       ",
    "TRANSFORM  ",
    "ENTITY     ",
    "ENTITY_NODE",
    "SCENE      ", 
    "MESH        ",
    "MODEL       ", 
    "SHADER      ",
    "SHADER_UNIFORM" };

static memory_state state;

void init_memory() {
    platform_zero_memory(&state, sizeof(state));
}

void shutdown_memory() {

}

// TODO: Aligned Memory
void* AGallocate(u64 size, memory_tag type) {
    if (type == MEMORY_TAG_UNKNOWN) {
        AG_WARN("AGallocate called using MEMORY_TAG_UNKNOWN, Re-class This allocation");
    }

    state.total_allocated += size;
    state.tagged_allocations[type] += size;

    void* block = platform_allocate(size, FALSE);
    platform_zero_memory(block, size);
    return block;
} 

void  AGfree(void* block, u64 size, memory_tag type) {
    if (type == MEMORY_TAG_UNKNOWN) {
        AG_WARN("AGfree called using MEMORY_TAG_UNKNOWN, Re-class This allocation");
    }
    state.total_allocated -= size;
    state.tagged_allocations[type] -= size;

    platform_free(block, FALSE);
} 

void* AGzero_memory(void* block, u64 size) {
    return platform_zero_memory(block, size);
}

void* AGcopy_memory(void* dest, const void* source, u64 size) {
    return platform_copy_memory(dest, source, size);
}

void* AGset_memory(void* dest, i32 value, u64 size) {
    return platform_set_memory(dest, value, size);
}

char* get_memory_usage_str() {
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    char buffer[8000] = "System memory usege (tagged):\n";
    u64 offset = string_length(buffer);
    for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if (state.tagged_allocations[i] >= gib) {
            unit[0] = 'G';
            amount = state.tagged_allocations[i] / (float)gib;
        } else if (state.tagged_allocations[i] >= mib) {
            unit[0] = 'M';
            amount = state.tagged_allocations[i] / (float)mib;
        } else if (state.tagged_allocations[i] >= kib) {
            unit[0] = 'K';
            amount = state.tagged_allocations[i] / (float)kib;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float)state.tagged_allocations[i];
        }

        i32 length = snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }
    char* out_string = string_duplicate(buffer);
    return out_string;
}

