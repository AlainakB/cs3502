// ============================================
// consumer.c - Consumer process starter
// ============================================
//

#include "buffer.h"

// Global variables for cleanup
shared_buffer_t* buffer = NULL;
sem_t* mutex = NULL;
sem_t* empty = NULL;
sem_t* full = NULL;
int shm_id = -1;

void cleanup() {
    // Detach shared memory
    if (buffer != NULL) {
        shmdt(buffer);
    }
    
    // Close semaphores
    if (mutex != SEM_FAILED) sem_close(mutex);
    if (empty != SEM_FAILED) sem_close(empty);
    if (full != SEM_FAILED) sem_close(full);
}

void signal_handler(int sig) {
    printf("\nConsumer: Caught signal %d, cleaning up...\n", sig);
    cleanup();
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <consumer_id> <num_items>\n", argv[0]);
        exit(1);
    }
    
    int consumer_id = atoi(argv[1]);
    int num_items = atoi(argv[2]);
    
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Seed random number generator
    srand(time(NULL) + consumer_id * 100);
    
    // TODO: Attach to shared memory
    shm_id = shmget (SHM_KEY, sizeof(shared_buffer_t),
    IPC_CREAT | 0666);
    if (shm_id < 0) {
       perror("shmget - failed");
       exit(1);
    }
    
    // TODO: Open semaphores (don't use O_CREAT - producer creates them)
    sem_t* mutex = sem_open ("/sem_mutex", 0);
    sem_t* empty = sem_open ("/sem_empty", 0);
    sem_t* full = sem_open ("/sem_full", 0);
    
    printf("Consumer %d: Starting to consume %d items\n", consumer_id, num_items);
    
    // TODO: Main consumption loop
    for (int i = 0; i < num_items; i++) {
        usleep(rand() % 100000); // Simulate work
        
        // TODO: Wait for full slot
        sem_wait(full); // Wait for item
        
        // TODO: Enter critical section
        sem_wait(mutex); // Enter critical section

        // TODO: Remove item from buffer
        // Remove from buffer
        item_t item = buffer -> buffer[buffer -> tail];
        buffer -> tail = (buffer -> tail + 1) % BUFFER_SIZE;
        buffer -> count--;
        
        // TODO: Exit critical section
        printf("Consumer %d: Consumed value %d from Producer %d \n",
        consumer_id, item.value, item.producer_id);

        sem_post(mutex); // Exit critical section
        
        // TODO: Signal empty slot
        sem_post(empty); // Signal slot available

        // Simulate consumption time
        usleep(rand() % 100000);
    }
    
    printf("Consumer %d: Finished consuming %d items\n", consumer_id, num_items);
    cleanup();
    return 0;
}
