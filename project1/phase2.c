#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_ACCOUNTS 1
#define NUM_THREADS 6
#define TRANSACTIONS_PER_TELLER 10

typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;  // Mutex for this account
} Account;


// Global accounts array (shared resource)
Account accounts[NUM_ACCOUNTS];

// Protected transactions
void deposit (int account_id , double amount) {
  if (pthread_mutex_lock(&accounts[account_id].lock) != 0) {
        perror("Failed to acquire lock for deposit");
        return;
  }
  // Critical section - only one thread can execute this at a time
  printf("Depositing $%.2f\n", amount);
  double temp = accounts[account_id].balance;
  usleep(100);
  accounts[account_id].balance = (temp + amount);
  accounts[account_id].transaction_count++;
  pthread_mutex_unlock(&accounts[account_id].lock);
}

void withdrawal (int account_id , double amount) {
  if (pthread_mutex_lock(&accounts[account_id].lock) != 0) {
        perror("Failed to acquire lock for withdrawal");
        return;
  }
  printf("Withdrawing $%.2f\n", amount);
  double temp = accounts[account_id].balance;
  usleep(100);
  accounts[account_id].balance = (temp - amount);
  accounts[account_id].transaction_count++;
  printf("Updated balance: $%.2f\n", accounts[account_id].balance);
  pthread_mutex_unlock(&accounts[account_id].lock);
}

// Thread function
void* teller_thread(void* arg) {
    int teller_id = *(int*) arg;  // Cast void* to int* and dereference
    unsigned int seed = time(NULL) + teller_id;

    // Perform multiple transactions
    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
        // TODO: Select random account
        int randIndex = rand_r (&seed) % NUM_ACCOUNTS; // from 0 to n - 1


        
        // TODO: Perform deposit or withdrawal (this will have race conditions!)
        int choice = (rand_r(&seed) + time(NULL)) % 2;
        // int money = (rand_r(&seed) + time(NULL)) % 101;
        int money = 5;
        
        if (pthread_mutex_lock(&accounts[randIndex].lock) != 0) {
        perror("Failed to acquire lock for deposit");
        return;
        }   
        printf("Initial balance : $%.2f\n", accounts[randIndex].balance);
        pthread_mutex_unlock(&accounts[randIndex].lock);
        if (choice == 0)
        { 
          
          if (randIndex > NUM_ACCOUNTS || randIndex < 0)
          {
            printf("Account %d does not exist, cancelling transaction.\n", randIndex);
            return 1;
          }
          deposit(accounts[randIndex].account_id, money);
        }
        else
        {
          if (randIndex > NUM_ACCOUNTS || randIndex < 0)
          {
            printf("Account %d does not exist, cancelling transaction.\n", randIndex);
            return 1;
          }
          withdrawal(accounts[randIndex].account_id, money);
          if (accounts[randIndex].balance < 0)
          {
            accounts[randIndex].balance += money;
            printf("Transaction denied. Not enough balance to withdraw.");
          }
          
        }
        
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

    clock_t start = clock();
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

    // Destroy mutexes for each account
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
      pthread_mutex_destroy(&accounts[i].lock);
    }

    clock_t end = clock();

    double performance = (double)(end - start) / CLOCKS_PER_SEC;

    // Print final balances
    printf("\nFinal Account Balances:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: $%.2f\n", accounts[i].account_id, accounts[i].balance);
    }

    printf("\nFinal performance time (with mutexes):%lf seconds\n", performance);

    return 0;
}

