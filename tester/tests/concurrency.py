from concurrent.futures import ThreadPoolExecutor

def thread_function(func, nbWorker, code):
    count = 0
    with ThreadPoolExecutor(max_workers=nbWorker) as executor:
        futures = [executor.submit(func) for _ in range(nbWorker)]
        for f in futures:
            if (f.result() == code):
                count += 1
    return (count)
    
def timeout_sim(func, nbWorker, code):
    print(f"{nbWorker} timeout simulation:")
    count = thread_function(func, nbWorker, code)
    if (count == nbWorker):
        print(f"{count}/{nbWorker} \033[32mSuccess\033[0m")
    else:
        print(f"{count}/{nbWorker} \033[31mFailure\033[0m")
