from concurrent.futures import ThreadPoolExecutor
from tests.socket_tests import concurrent_delete
import threading

def thread_function(func, nbWorker, code):
    count = 0
    with ThreadPoolExecutor(max_workers=nbWorker) as executor:
        futures = [executor.submit(func) for _ in range(nbWorker)]
        for f in futures:
            if (f.result() == code):
                count += 1
    return (count)
    
def timeout_sim(func, nbWorker, code):
    print(f"\033[1mSending {nbWorker} concurrent timeout to the server:\033[0m")
    count = thread_function(func, nbWorker, code)
    if (count == nbWorker):
        print(f"{count}/{nbWorker} \033[32mSuccess\033[0m\n")
    else:
        print(f"{count}/{nbWorker} \033[31mFailure\033[0m\n")

def test_concurrent_delete(path, nbWorker, code):
    print(f"\033[1mTest of {nbWorker} concurrent delete request on {path}\033[0m")
    deleteCodes = thread_function(lambda: concurrent_delete(path), nbWorker, code)
    print(f"{deleteCodes}/{nbWorker} - Expected: {nbWorker-1} if {path} exists\n")