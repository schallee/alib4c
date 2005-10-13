#ifndef queue_h
#define queue_h

#include <stdint.h>

#define queue_flag_free_data	1
#define queue_status_success	0
#define queue_status_closed	1
#ifndef queue_object
	typedef void queue_t;
	typedef uint_fast8_t queue_flags_t;
#endif

queue_t *q_init(void);
int q_enque(queue_t *q, void *item, queue_flags_t flags);
void *q_dequeue(queue_t *q);
int q_deinit(queue_t *q);
unsigned int q_num(queue_t *q);
void q_close(queue_t *q);
void q_open(queue_t *q);

#endif
