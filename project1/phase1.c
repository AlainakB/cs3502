#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_ACCOUNTS 3
#define NUM_THREADS 4
#define TRANSACTIONS_PER_TELLER 5

// Shared data structure
typedef struct {
    int account_id;
    double balance;
    int transaction_count;
} Account;

// Global accounts array (shared resource)
Account accounts[NUM_ACCOUNTS];

// Thread function
void* teller_thread(void* arg) {
    int teller_id = *(int*) arg;  // Cast void* to int* and dereference
    unsigned int seed = teller_id;
    
    

    // Perform multiple transactions
    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
        // TODO: Select random account
        int randIndex = rand_r(&seed) % NUM_ACCOUNTS; //from 0 to n - 1
        printf("Initial balance : %.2f\n", accounts[randIndex].balance);
        // TODO: Perform deposit or withdrawal (this will have race conditions!)
        int choice = rand_r(&seed) % 2;
        int money = rand_r(&seed) % 1001;
        
        if (choice == 0)
        {
          printf("Thread %i: Depositing %d\n", teller_id, money);
          accounts[randIndex].balance += money;
        }
        else
        {
          printf("Thread %i: Withdrawing %d\n", teller_id, money);
          accounts[randIndex].balance -= money;
        
        accounts[randIndex].transaction_count += 1;
        printf("Teller %d: Transaction %d\n\n", teller_id, i);
        }
    }

    return NULL;
}

int main() {
    // Initialize accounts
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
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

    // Print final balances
    printf("\nFinal Account Balances:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: %.2f\n", accounts[i].account_id, accounts[i].balance);
    }

    return 0;
}

