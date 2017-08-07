State transitions:

                    ___________________
                   |                   |
                   |                   v
    "START"--> Initing -> Inited -> Running
                   |       | ^         |
                   |       | |_________|
                   |       |    stop()
                   |       v
                   -----> "END"


Advantages of this implementation:

1. Easier implementation and more readable.
2. Only two semaphores used between the controlor and one worker. Less overhead.

Note:

1. Only `StopWorker()` is sync, other APIs (`NewWorker()`/`RunWorker()`/`QuitWorker()`) are all async.
