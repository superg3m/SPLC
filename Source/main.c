#include <lexer.h>
#include <spl_parser.h>
#include <interpreter.h>

#define TOTAL_STACK_MEMORY_SIZE KiloBytes(800)
#define TOTAL_HEAP_MEMORY_SIZE MegaBytes(35)
void* custom_alloc_callback(CKG_Allocator* allocator, size_t allocation_size) {
	CKG_Arena* arena = (CKG_Arena*)allocator->ctx;
	return ckg_arena_push_custom(arena, allocation_size);
}

void custom_free_callback(CKG_Allocator* allocator, void* data) {
	CKG_Arena* arena = (CKG_Arena*)allocator->ctx;
	ckg_arena_pop_data(arena, data);
}

// Date: May 21, 2025
// TODO(Jovanni): Make it so arenas have some sort of free list and pop ability so I can recycle memory.

int main(int argc, char** argv) {
	// u8 program_stack_memory[TOTAL_STACK_MEMORY_SIZE] = {0};
	void* program_heap_memory = ckg_alloc(TOTAL_HEAP_MEMORY_SIZE);
	CKG_Arena arena = ckg_arena_create_fixed(program_heap_memory, TOTAL_HEAP_MEMORY_SIZE, false);
	ckg_bind_custom_allocator(custom_alloc_callback, custom_free_callback, &arena);

	if (argc != 2) {
		CKG_LOG_ERROR("Usage: splc <filename>\n");
		return -1;
	}

	char* file_name = argv[1];
	u64 source_length = 0;
	CKG_Error file_err = CKG_ERROR_SUCCESS;
	u8* source = ckg_io_read_entire_file(file_name, &source_length, &file_err);
	if (file_err != CKG_ERROR_SUCCESS) {
		return file_err;
	}

	Lexer lexer = lexer_create();
	SPL_Token* token_stream = lexer_consume_token_stream(&lexer, (char*)source, source_length);
	CKG_LOG_PRINT("LEXER:\n");
	for (int i = 0; i < ckg_vector_count(token_stream); i++) {
		SPL_Token token = token_stream[i];
		token_print(token, "    ");
	}

	CKG_LOG_PRINT("Parser:\n");
	ASTNode* ast = parse(token_stream);
	ast_pretty_print(ast);

	CKG_LOG_PRINT("Interpreter:\n");
	interpret_ast(ast);

	ckg_vector_free(token_stream);

	CKG_LOG_SUCCESS("used/cap: %zu / %zu = %f\n", arena.used, arena.capacity, (float)arena.used / (float)arena.capacity);
	ckg_arena_free(&arena);
}