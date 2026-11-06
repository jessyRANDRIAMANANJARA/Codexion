*This project has been created as part of the 42 curriculum by hrandri2*

## Description

CODExION is a multithreaded simulation project that models coders working in a shared co-working environment, competing for limited USB dongles to compile their quantum code. The project demonstrates advanced concepts in concurrent programming, including thread synchronization, resource management, and scheduling algorithms.

## Goal

The main goal is to simulate multiple coders who must acquire USB dongles to compile their code, while managing deadlines to avoid burnout. The simulation must handle concurrent access to shared resources, prevent deadlocks, and implement fair scheduling policies.

## Instructions

### Compilation
 
```bash
make
```

### Execution

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

**Example:**
```bash
./codexion 6 1000 200 100 150 5 50 fifo
```

### Parameters

- `number_of_coders`: Number of coders and dongles in the simulation
- `time_to_burnout`: Time in milliseconds before a coder burns out if they don't compile
- `time_to_compile`: Time in milliseconds required for compilation
- `time_to_debug`: Time in milliseconds spent debugging
- `time_to_refactor`: Time in milliseconds spent refactoring
- `number_of_compiles_required`: Number of compilations needed per coder to complete
- `dongle_cooldown`: Time in milliseconds a dongle is unavailable after release
- `scheduler`: Scheduling algorithm - `fifo` or `edf`

## Blocking Cases Handled

### Deadlock Prevention
- **Coffman's Conditions**: The implementation prevents circular wait by ensuring coders release dongles if they can't acquire both within a reasonable time
- **Resource Ordering**: Dongles are acquired in a consistent order (left then right) to prevent deadlocks
- **Timeout Mechanism**: Coders have a maximum number of attempts to acquire dongles before giving up

### Starvation Prevention
- **Fair Arbitration**: Both FIFO and EDF scheduling algorithms ensure no coder is permanently denied access to dongles
- **Priority Queue**: Custom heap implementation provides fair resource allocation
- **Liveness Guarantee**: The monitor thread ensures the simulation progresses and no coder starves indefinitely

### Cooldown Handling
- **Dongle Cooldown**: After release, dongles remain unavailable for the specified cooldown period
- **Time-based Tracking**: Precise timestamp tracking ensures cooldown periods are respected
- **Atomic Operations**: Dongle state changes are protected by mutexes to prevent race conditions

### Precise Burnout Detection
- **Monitor Thread**: Dedicated thread continuously monitors coder deadlines
- **Real-time Tracking**: Burnout detection occurs within 10ms of the actual deadline
- **Immediate Termination**: Simulation stops immediately when a coder burns out

### Log Serialization
- **Mutex Protection**: All log output is protected by a dedicated mutex
- **Atomic Writes**: Each log message is written atomically to prevent interleaving
- **Timestamp Precision**: Logs include millisecond-accurate timestamps

## Thread Synchronization Mechanisms

### Mutexes
- **Dongle Protection**: Each dongle has its own mutex to protect its state (available/taken)
- **Coder State**: Mutexes protect individual coder states (alive/running, compile count)
- **Simulation Control**: Global mutex protects simulation-wide state (should_stop flag)
- **Log Serialization**: Dedicated mutex ensures thread-safe output

### Condition Variables
- **Dongle Waiting**: Coders wait on condition variables when dongles are unavailable
- **Monitor Coordination**: Monitor thread uses condition variables to coordinate with coders
- **Efficient Wakeups**: Broadcasting ensures waiting coders are promptly notified

### Custom Event Implementation
- **Priority Queue**: Custom heap implementation manages scheduling priorities
- **Time-based Events**: Deadline tracking for EDF scheduling
- **Event Ordering**: Events are processed in chronological order

### Race Condition Prevention
- **Atomic Operations**: Critical sections are protected by mutexes
- **State Consistency**: Shared state is modified atomically to prevent inconsistent states
- **Memory Barriers**: Proper locking ensures visibility of state changes across threads

### Thread-safe Communication
- **Shared Data Structures**: Protected access to shared simulation state
- **Signaling Mechanisms**: Condition variables enable efficient inter-thread communication
- **Graceful Termination**: Clean shutdown of all threads when simulation ends

## Resources

- [POSIX Threads Documentation](https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html)
- [Condition Variables Tutorial](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html)
- [Deadlock Prevention Techniques](https://en.wikipedia.org/wiki/Deadlock)
- [Priority Queue Implementation](https://en.wikipedia.org/wiki/Priority_queue)
- [EDF Scheduling Algorithm](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

## AI Usage

AI tools were used for:

- **Code Structure**: Initial project layout and file organization
- **Documentation**: README improvement and technical writing assistance
- **Algorithm Review**: Verification of scheduling and synchronization logic
- **Error Handling**: Best practices for error checking and resource cleanup
- **Others**: functions refactoring, some tests and output analysis
