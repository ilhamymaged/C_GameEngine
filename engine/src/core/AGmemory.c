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

u64 get_memory_usage(memory_tag type) {
    return state.tagged_allocations[type];
}

void get_remaining_memory_str() {
    // Check to see if there's remaining memory we haven't freed.
    if (state.total_allocated != 0) {
        AG_FATAL("Memory leak detected: %llu bytes still allocated", state.total_allocated);
        char memory_buffer[8000];
        get_memory_usage_buffer(memory_buffer, sizeof(memory_buffer));
        AG_INFO(memory_buffer);
    }
}   

void get_memory_usage_buffer(char* buffer, u64 size) {
    if (!buffer || size == 0) {
        return;
    }

    const u64 gib = 1024ULL * 1024ULL * 1024ULL;
    const u64 mib = 1024ULL * 1024ULL;
    const u64 kib = 1024ULL;

    u64 offset = 0;

    // Write header
    i32 written = snprintf(
        buffer + offset,
        size - offset,
        "System memory usage (tagged):\n"
    );

    if (written < 0) {
        return;
    }

    offset += written;

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
            unit[1] = '\0';
            amount = (float)state.tagged_allocations[i];
        }

        // Prevent overflow
        if (offset >= size) {
            break;
        }

        written = snprintf(
            buffer + offset,
            size - offset,
            "  %s: %.2f%s\n",
            memory_tag_strings[i],
            amount,
            unit
        );

        if (written < 0) {
            break;
        }

        offset += written;
    }

    // Ensure null termination
    buffer[size - 1] = '\0';
}