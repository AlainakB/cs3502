Overview:
This a project simulation of a multi-threaded banking system. Each phase represents a problem that could be caused by threading or its resolution including:
- Race Conditions (Phase 1)
- Synchronization via mutexes (Phase 2)
- Deadlocks (Phase 3)
- Deadlock prevention (Phase 4)

Features:
Using 6 threads, 10 transactions, and 1 account
- Race conditions
- Mutuex synchronization

Using 4 threads, 3 transactions, and 3 accounts
- Deadlocks
- Deadlock prevention

Simply run software to observe the effects mentioned above. Compile using gcc -Wall -pthread phase<num>.c -o phase<num>



