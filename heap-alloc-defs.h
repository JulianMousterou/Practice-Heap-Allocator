#define HEAPSIZE 4096
#include <stddef.h>
typedef struct heap_metadata {
	size_t size;
} heap_metadata; 
typedef struct heap_free {
	size_t size;
	struct heap_free* next; 
	struct heap_free* prev; 
} heap_free;  
