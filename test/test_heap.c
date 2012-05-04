#include <sys/types.h>
#include "trema.h"
#include "heap.h"

int
main() {
  heap_ops_t ops = {
    NULL, NULL
  };

  heap_t *heap = create_heap( 1000, &ops );

  if ( heap != NULL ) {
    return -1;
  } else {
    return 1;
  }
}
