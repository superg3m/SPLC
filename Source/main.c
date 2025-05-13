#include <ckg.h>

#define TOTAL_MEMORY_SIZE KiloBytes(30)
void* custom_alloc_callback(CKG_Allocator* allocator, size_t allocation_size) {
	CKG_Arena* arena = (CKG_Arena*)allocator->ctx;
	return ckg_arena_push_custom(arena, allocation_size);
}

void custom_free_callback(CKG_Allocator* allocator, void* data) {
	(void)allocator; 
	(void)data;
	return;
}

int main(int argc, char** argv) {
	u8 program_stack_memory[TOTAL_MEMORY_SIZE] = {0};
	CKG_Arena arena = ckg_arena_create_fixed(program_stack_memory, TOTAL_MEMORY_SIZE, true);
	ckg_bind_custom_allocator(custom_alloc_callback, custom_free_callback, &arena);

    if (argc != 3) {
        CKG_LOG_ERROR("Usage: splc <filename>\n");
        return -1;
    }



    ckg_arena_free(&arena);
}