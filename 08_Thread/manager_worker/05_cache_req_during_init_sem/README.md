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

However, this implementation is very restricted:

1. It requires the user to call `RunWorker()` and `StopWorker()` interleavely. And `RunWorker()` is always called first.
2. After calling `QuitWorker()`, the user can't call any API.

Some thing different, but not sure if it is merit or flaw:

1. Only `StopWorker()` is sync, other APIs (`NewWorker()`/`RunWorker()`/`QuitWorker()`) are all async.
