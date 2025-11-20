#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> 

#define NUM_ACCOUNTS 1
#define NUM_THREADS 6
#define TRANSACTIONS_PER_TELLER 50

// Shared data structure
typedef struct {
    int account_id;
    double balance;
    int transaction_count;
} Account;

// Stores a record of a transaction
typedef struct {
    int teller_id;
    int account_id;
    double amount;
    int iteration;
    double initial_balance;
    double final_balance;
    int choice; // 1 for deposit, 0 for withdrawal
} Transaction_Log;

// Global accounts array (shared resource)
Account accounts[NUM_ACCOUNTS];

// Global transaction log for threads to store transactions
Transaction_Log  transactions[NUM_THREADS * TRANSACTIONS_PER_TELLER];

// Thread function
void* teller_thread(void* arg) {
    int teller_id = *(int*) arg;  // Cast void* to int* and dereference
    unsigned int seed = time(NULL) + teller_id;
    int randIndex = rand_r(&seed) % NUM_ACCOUNTS; //from 0 to n - 1


    // Perform multiple transactions
    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
        int transaction_index = teller_id * TRANSACTIONS_PER_TELLER + i; // transaction log for ith transaction
        transactions[transaction_index].initial_balance = accounts[randIndex].balance;

        
        int choice = rand_r(&seed) % 2;
        double money = 5;
        
        if (choice == 0)
        {
          if (randIndex < 0 || randIndex >= NUM_ACCOUNTS)
          {
            printf("Account %d does not exist, cancelling transaction.\n", randIndex);
            return NULL;
          } // Handles non-existant account edge case
          usleep(100);
          accounts[randIndex].balance += money;
        }
        else
        {
          if (randIndex < 0 || randIndex >= NUM_ACCOUNTS)
          {
            printf("Account %d does not exist, cancelling transaction.\n", randIndex);
            return NULL;
          }
          usleep(100);
          accounts[randIndex].balance -= money;
          if (accounts[randIndex].balance < 0)
          {
            accounts[randIndex].balance += money;
          } // Prevents negative balance
        }

        // Each transaction has its own index -> no race condition
        accounts[randIndex].transaction_count += 1;
        transactions[transaction_index].teller_id = teller_id;
        transactions[transaction_index].account_id = accounts[randIndex].account_id;
        transactions[transaction_index].amount = money;
        transactions[transaction_index].choice = choice;
        transactions[transaction_index].iteration = i;
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
    
    double expected_result[NUM_ACCOUNTS];
    for (int i = 0; i < NUM_ACCOUNTS; i++)
    {
      expected_result[i] = 1000; // initializes expected balance to 0
    }

    // Uses the transaction log to serially calculate the balance from what the threads reported
    for (int i = 0; i < (NUM_THREADS * TRANSACTIONS_PER_TELLER); i++)
    { 
      int index = transactions[i].account_id;
      printf("Initial balance : $%.2f\n", transactions[i].initial_balance);
      if (!transactions[i].choice) // deposit == 0
      {
        printf("Thread %i: Depositing $%.2f\n", transactions[i].teller_id, transactions[i].amount);
        expected_result[index] += transactions[i].amount;
      }
      else
      {
        printf("Thread %i: Withdrawing $%.2f\n", transactions[i].teller_id, transactions[i].amount);
        expected_result[index] -= transactions[i].amount;
      }
      if (expected_result[index] < 0)
      {
        expected_result[index] += transactions[i].amount;
        printf("Transaction denied. Not enough balance to withdraw.\n");
      } // Prevents negative balance
      printf("Teller %d: Transaction %d for account %d\n\n", transactions[i].teller_id, transactions[i].iteration, transactions[i].account_id);

    }

    // Print final balances
    printf("\nFinal Account Balances:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: %.2f\n", accounts[i].account_id, accounts[i].balance);
        printf("Expected: %.2f\n", expected_result[i]);
    }
    return 0;
}