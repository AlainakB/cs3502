#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_ACCOUNTS 3
#define NUM_THREADS 4
#define TRANSACTIONS_PER_TELLER 5

typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;  // Mutex for this account
} Account;


// Global accounts array (shared resource)
Account accounts[NUM_ACCOUNTS];


void transfer (int from_id , int to_id , double amount) {
  printf("Thread %lu: Attempting transfer from %d to %d \n", pthread_self(), from_id, to_id);

  pthread_mutex_lock(&accounts[from_id].lock);
  printf("Thread %lu: Locked account %d\n", pthread_self(), from_id);
  
  usleep(100);
  
  printf("Thread %lu: Waiting for account %d\n", pthread_self(), to_id);
  pthread_mutex_lock(&accounts[to_id].lock);
  
  accounts[from_id].balance -= amount;
  accounts[to_id].balance += amount;
  
  pthread_mutex_unlock(&accounts[to_id].lock);
  pthread_mutex_unlock(&accounts[from_id].lock);
  
}

// Thread function
void* teller_thread(void* arg) {
    int teller_id = *(int*) arg;  // Cast void* to int* and dereference
    unsigned int seed = time(NULL) + teller_id;

    // Perform multiple transactions
    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
        // TODO: Select random account
        int randIndex = (rand_r(&seed)) % NUM_ACCOUNTS; //from 0 to n - 1
        int randIndex2 = (rand_r(&seed)) % NUM_ACCOUNTS; //from 0 to n - 1
        
        // TODO: Perform deposit or withdrawal (this will have race conditions!)
        int choice = (rand_r(&seed)) % 2;
        int money = (rand_r(&seed)) % 101;
        
        printf("Initial balance : $%.2f\n", accounts[randIndex].balance);
        
        printf("Thread %i: Transferring $%d\n", teller_id, money);
          transfer(accounts[randIndex].account_id, accounts[randIndex2].account_id, money);
      
        printf("Teller %d: Transaction %d for account %d\n\n", teller_id, i, accounts[randIndex].account_id);
    }

    return NULL;
}


int main() {
    // Initialize accounts
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
         pthread_mutex_init(&accounts[i].lock, NULL);  // Initialize the mutex
        accounts[i].balance = 1000.0;  // starting balance
        accounts[i].transaction_count = 0;
    }

    // Create threads
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    for ( int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    // Print final balances
    printf("\nFinal Account Balances:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: $%.2f\n", accounts[i].account_id, accounts[i].balance);
    }

    return 0;
}

