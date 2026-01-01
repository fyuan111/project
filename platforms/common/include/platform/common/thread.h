#pragma once

#include "atomic.hpp"
 

class WorkQueueManager;

extern fml::atomic_int globe_tid;

int get_tid();

bool destroy_thread(int tid);

uint8_t workqueue_id(uint8_t priority);



