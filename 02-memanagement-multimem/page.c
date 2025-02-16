
#include "types.h"
#include "platform.h"

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

static ptr_t _alloc_start = 0;
static ptr_t _alloc_end = 0;
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
	page->flags = flag;
}

static inline ptr_t _align_page(ptr_t address)
{
        ptr_t order = (1 << PAGE_ORDER) - 1;
        return (address + order) & (~order);
}

void page_init()
{
        ptr_t _heap_start_aligned = _align_page(HEAP_START);

        uint32_t num_reserved_pages = LENGTH_IROM / (1*1024*1024);

        _num_pages = (HEAP_SIZE - (_heap_start_aligned - HEAP_START))/ PAGE_SIZE - num_reserved_pages;

        struct Page *page = (struct Page *)HEAP_START;
        for (int i = 0; i < _num_pages; i++) {
                _clear(page);
                page++;
        }
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
					_set_flag(page_i,PAGE_TAKEN);			
					page_k++;
				}
				_set_flag(page_k - 1,PAGE_LAST);		
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
	while(_is_free(page)){
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
        uart_puts("alloc 1\r\n");
        //page_free(p);

        void *p2 = page_alloc(7);
        uart_puts("alloc 2\r\n");
        page_free(p2);

        void *p3 = page_alloc(4);
        uart_puts("alloc 3\r\n");
}



