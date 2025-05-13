#include <lexer.h>

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

    if (argc != 2) {
        CKG_LOG_ERROR("Usage: splc <filename>\n");
        return -1;
    }

	char* file_name = argv[1];
	u64 source_length = 0;
	CKG_Error file_err = CKG_ERROR_SUCCESS;
	u8* source = ckg_io_read_entire_file(file_name, &source_length, &file_err);
	if (CKG_ERROR_IO_RESOURCE_NOT_FOUND) {
		CKG_LOG_ERROR("Can't find file: %s\n", file_name);
        return -1;
	}

	Lexer lexer = lexer_create();
	SPL_Token* token_stream = lexer_consume_token_stream(&lexer, source, source_length);


	ckg_vector_free(token_stream);
    ckg_arena_free(&arena);
}