#include "os.h"
#include "types.h"
#include "platform.h"

extern ptr_t IROM_START;
extern ptr_t IROM_SIZE;
extern ptr_t DROM_START;
extern ptr_t DROM_SIZE;

extern ptr_t TEXT_START;
extern ptr_t TEXT_END;
extern ptr_t DATA_START;
extern ptr_t DATA_END;
extern ptr_t RODATA_START;
extern ptr_t RODATA_END;
extern ptr_t BSS_START;
extern ptr_t BSS_END;
extern ptr_t HEAP_START;
extern ptr_t HEAP_SIZE;

extern void uart_puts(char *s);

static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_pages = 0;

#define PAGE_SIZE 4096
#define PAGE_ORDER 12

#define PAGE_TAKEN (uint8_t)(1 << 0)
#define PAGE_LAST  (uint8_t)(1 << 1)

struct Page{
	uint8_t flags;
};

static void _clear(struct Page *page){
	page->flags = 0;
}

static uint8_t _is_free(struct Page *page){
	if(page->flags & PAGE_TAKEN)
		return 0;
	else
		return 1;
}

static uint8_t _is_last(struct Page *page){
	if(page->flags & PAGE_LAST)
		return 1;
	else
		return 0;
}

static void _set_flag(struct Page *page,uint8_t flag){
	page->flags |= flag;
}

static inline ptr_t _align_page(ptr_t address)
{
        ptr_t order = (1 << PAGE_ORDER) - 1;
        return (address + order) & (~order);
}

void page_init()
{
        ptr_t _heap_start_aligned = _align_page(HEAP_START);

	uint32_t num_reserved_pages = LENGTH_RAM / (384 * 1024);
	uint32_t heap_reserved = HEAP_SIZE - (_heap_start_aligned - HEAP_START);
	uint32_t page_reserved = heap_reserved / PAGE_SIZE;

        _num_pages = page_reserved - num_reserved_pages; 

	printf("HEAP START:%p,HEAP_SIZE = %d\r\n",HEAP_START,HEAP_SIZE);
	printf("HEAP RESERVED = 0x%lx,\n"
	       "PAGE RESERVED = %d,\n",
	       heap_reserved,page_reserved);

	printf("num reserved1 = %d,\n",_num_pages);
	
	printf("num reserved2 = %d,\n",page_reserved - num_reserved_pages);

	printf("HEAP_START = %p(aligned to %p), HEAP_SIZE = 0x%lx,\n"
               "num of reserved pages = %d, num of pages to be allocated for heap = %d\n",
               HEAP_START, _heap_start_aligned, HEAP_SIZE,
               num_reserved_pages, _num_pages);

        struct Page *page = (struct Page *)HEAP_START;
        for (int i = 0; i < _num_pages; i++) {
                _clear(page);
                page++;
        }

        _alloc_start = _heap_start_aligned + num_reserved_pages * PAGE_SIZE;
        _alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);

        printf("IROM STARTS at: %p,SIZE : %d\n", IROM_START, IROM_SIZE);
        printf("DROM STARTS at: %p,SIZE : %d\n", DROM_START, DROM_SIZE);
	
        printf("TEXT:   %p -> %p\n", TEXT_START, TEXT_END);
        printf("RODATA: %p -> %p\n", RODATA_START, RODATA_END);
        printf("DATA:   %p -> %p\n", DATA_START, DATA_END);
        printf("BSS:    %p -> %p\n", BSS_START, BSS_END);
        printf("HEAP:   %p -> %p\n", _alloc_start, _alloc_end);
}
	
void *page_alloc(int npages){
	uint8_t found = 0;
	struct Page *page_i = (struct Page *)HEAP_START;
	for(int i = 0;i <= _num_pages - npages;i++){
		if(_is_free(page_i)){
			found = 1;
			struct Page *page_j = page_i + 1;
			for(int j = i + 1;j < i + npages;j++){
				if(! _is_free(page_j)){
					found = 0;
					break;
				}
				page_j++;
			}
			
			if(found){
				struct Page *page_k = page_i;
				for(int k = 0;k < npages;k++){
					_set_flag(page_k,PAGE_TAKEN);		
					page_k++;
				}
				page_k--;
				_set_flag(page_k,PAGE_LAST);		
				return (void *)(_alloc_start + i * PAGE_SIZE);
			}
		}	
		page_i++;
	}
	return (void *)0;
}

void page_free(void *p){
	if(!p || (ptr_t) p >=_alloc_end){
		return;
	}

	struct Page *page = (struct Page *)HEAP_START;
	page += ((ptr_t)p - _alloc_start) / PAGE_SIZE;
	while(! _is_free(page)){
		if(_is_last(page)){
			_clear(page);
			break;
		}
		else{
			_clear(page);
			page++;
		}
	}
}	

void page_test()
{
        void *p = page_alloc(2);
        printf("p = %p\n", p);
        //page_free(p);

        void *p2 = page_alloc(7);
        printf("p2 = %p\n", p2);
        page_free(p2);

        void *p3 = page_alloc(4);
        printf("p3 = %p\n", p3);
}



