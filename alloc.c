#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#define HEAPSIZE 1024	 
typedef struct heapchunk_data_t {
	size_t size; 
	int inuse; 
}heapchunk_data_t; 
typedef struct free_list_t {
	heapchunk_data_t* data; 
	struct free_list_t* next;
} free_list_t; 
static free_list_t* free_listp = NULL; 
static void* heap_start = NULL;
static void* heap_current = NULL;
static void* heap_end= NULL;
void align_to_page(void){
	size_t page_size = sysconf(_SC_PAGESIZE);
	void* brk = (void*)sbrk(0);
	if(brk == (void*)-1){
		perror("FAILED TO ALIGN TO PAGE");
	} 
	size_t misaligned = (unsigned long)brk % page_size; 
	if(misaligned != 0){
		size_t offset = page_size - misaligned;
	}
}
void free_list_init(void){
	if(free_listp == NULL){
		static free_list_t free_list; 
		free_list.data = NULL;
		free_list.next = NULL;
		free_listp = &free_list;
	} 
} 
void heap_init(void){
	if(heap_start==NULL){
		heap_start = (void *)sbrk(HEAPSIZE);
		if(heap_start != (void *)-1){
			heap_current = heap_start;
			heap_end = heap_start + HEAPSIZE; 
		}
	}
} 
void* heap_alloc(size_t size){
	heap_init(); 
	free_list_init(); 
	free_list_t* current = free_listp->next; 
	free_list_t* prev = free_listp; 
	size_t total_chunk_size = sizeof(heapchunk_data_t) + size;
	if(total_chunk_size % 16 != 0){
		align_to_page();
	}
	if(current!=NULL){
		if(current->data->inuse == 0 && current->data->size >= total_chunk_size){
			current->data->inuse = 1;
			current->data->size = total_chunk_size; 
			prev->next = current->next; 
			void* user_ptr = (char *)current->data + sizeof(heapchunk_data_t); 
			return user_ptr;
		} 
	} 
	if(heap_end-heap_current > total_chunk_size){
		void* this_alloc_start = heap_current; 
		heapchunk_data_t* chunk_info = (heapchunk_data_t*)this_alloc_start;
		chunk_info->size = total_chunk_size;
		chunk_info->inuse = 1;
		void* this_alloc_end = (char *)heap_current + total_chunk_size; 
		heap_current = this_alloc_end;
		void* user_ptr = this_alloc_start + sizeof(heapchunk_data_t); 
		return user_ptr; 
	} 
	if(total_chunk_size > heap_end-heap_current){
		void* newchunk_start = heap_current;
		heapchunk_data_t* chunk_info = (heapchunk_data_t*)newchunk_start;
		chunk_info->size = total_chunk_size; 
		void* newchunk_end = (char *)heap_current + total_chunk_size;
		heap_end = (void*)sbrk(total_chunk_size); 
		heap_end = newchunk_end; 
		heap_current = heap_end; 
		void* user_ptr = newchunk_start + sizeof(heapchunk_data_t); 
		return user_ptr; 
	} 
	return NULL; 
} 
void free(void* user_ptr){
	if(user_ptr==NULL) return; 
	void* meta = (char *)user_ptr - sizeof(heapchunk_data_t);
	heapchunk_data_t* chunk_info = (heapchunk_data_t *)meta;
	chunk_info->inuse = 0;
	free_list_t* new_free_node = (free_list_t *)meta; 
	new_free_node->data = chunk_info; 
	new_free_node->next = free_listp->next; 
	free_listp->next = new_free_node; 
} 
int main(){
}
