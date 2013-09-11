#include "std.h"

typedef struct paging_dir paging_dir;
typedef struct paging_page paging_page;

enum {pagesize = 4096,
      tablesize = 1024};

void paging_init ();

void paging_setDir (paging_dir* dir);

paging_page* paging_getPage (paging_dir* dir, void* address, bool make);