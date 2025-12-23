import sys
import math
import random
import time
import timeit 
import matplotlib.pyplot as plt
import numpy as np

# Utitilty functions - some are implemented, others you must implement yourself.

# function to plot the bar graph and average runtimes of N trials
# Please note that this function only plots the graph and does not save it
# To save the graphs you must use plot.save(). Refer to matplotlib documentation
def draw_plot(run_arr, mean):
    x = np.arange(0, len(run_arr),1)
    fig=plt.figure(figsize=(20,8))
    plt.bar(x,run_arr)
    plt.axhline(mean,color="red",linestyle="--",label="Avg")
    plt.xlabel("Iterations")
    plt.ylabel("Run time in seconds")
    plt.title("Algorithm run times - Experiment X") #i would change the title as appropriate each time
    plt.show()

# function to generate random list 
# @args : length = number of items 
#       : max_value maximum value
def create_random_list(length, max_value, item=None, item_index=None):
    random_list = [random.randint(0,max_value) for _ in range(length)]
    if item!= None:
        random_list.insert(item_index,item)

    return random_list

# function to generate reversed list of a given size and with a given maximum value
def create_reverse_list(length, max_value, item=None, item_index=None):
    reversed_list = create_random_list(length, max_value,item,item_index)
    reversed_list.sort(reverse=True)

    return reversed_list

# function to generate near sorted list of a given size and with a given maximum value
def create_near_sorted_list(length, max_value, item=None, item_index=None):
    near_sorted_list = create_random_list(length, max_value,item,item_index)
    near_sorted_list.sort()
    
    step = math.floor(length // (0.25 * length))
    
    for i in range(0, length, step):
        swap_range = random.randint(-step//2.5, step//2.5)
        if swap_range < 0 :
            swap_dex = max(0, i + swap_range)
        else:
            swap_dex = min(length -1, i + swap_range)
        
        near_sorted_list[i], near_sorted_list[swap_dex] = near_sorted_list[swap_dex], near_sorted_list[i]

    return near_sorted_list

# function to generate near sorted list of a given size and with a given maximum value
def reduced_unique_list(length, max_value, item=None, item_index=None):
    reduced_list = create_random_list(length, max_value,item,item_index)
    reduced_list = list(set(reduced_list))
    random.shuffle(reduced_list)

    return reduced_list

# Implementation of sorting algorithms
class BubbleSort:
    def __init__(self, items_to_sort):
        self.items = items_to_sort
        self.sorted_items=[]
        
        ### your implementation for bubble sort goes here 
        for i in range(len(items_to_sort) - 1):
            for j in range(len(items_to_sort) - 1, i, -1):
                if items_to_sort[j] < items_to_sort[j - 1]:
                    items_to_sort[j], items_to_sort[j - 1] = items_to_sort[j - 1], items_to_sort[j]
                    
        self.sorted_items = items_to_sort
    def get_sorted(self,):
        return self.sorted_items
    
class InsertionSort:
    def __init__(self, items_to_sort):
        self.items = items_to_sort
        self.sorted_items=[]
        
        ### your implementation for insertion sort goes here 
        for j in range(1, len(items_to_sort)):
            key = items_to_sort[j]
            i = j - 1
            while i >=0 and items_to_sort[i] > key:
                items_to_sort[i + 1] = items_to_sort[i]
                i = i - 1
            items_to_sort[i + 1] = key
            
        self.sorted_items = items_to_sort

    def get_sorted(self,):
        return self.sorted_items
    
class SelectionSort:
    def __init__(self, items_to_sort):
        self.items = items_to_sort
        self.sorted_items=[]
        
        ### your implementation for selection sort goes here 
        for i in range(len(items_to_sort) - 1):
            mindex = i
            minval = items_to_sort[i]
            for j in range(i + 1, len(items_to_sort)):
                if items_to_sort[j] < minval :
                    mindex = j
                    minval = items_to_sort[j]
            items_to_sort[mindex] = items_to_sort[i]
            items_to_sort[i] = minval
            
        self.sorted_items = items_to_sort

    def get_sorted(self,):
        return self.sorted_items
    
class MergeSort:
    def __init__(self, items_to_sort):
        self.items = items_to_sort
        self.sorted_items=[]
        
        ### your implementation for selection sort goes here 
        self.merge_sort(0, len(items_to_sort) - 1)
        
        self.sorted_items = self.items
        
    def merge_sort(self, p, r):
        if p < r:
            q = (p + r)//2
            self.merge_sort(p, q)
            self.merge_sort(q+1, r)
            self.merge(p, q, r)
        return
        
    def merge(self, p, q, r):
        n1 = q - p + 1
        n2 = r - q
        L = [0] * (n1 + 1)
        R = [0] * (n2 + 1)
        for i in range(n1):
            L[i] = self.items[p + i]
        for j in range(n2):
            R[j] = self.items[q + j + 1]
        L[n1] = math.inf
        R[n2] = math.inf
        i = 0
        j = 0
        for k in range(p, r+1):
            if L[i] <= R[j] :
                self.items[k] = L[i]
                i += 1
            else:
                self.items[k] = R[j]
                j += 1
        return

    def get_sorted(self,):
        return self.sorted_items
    
class QuickSort:
    def __init__(self, items_to_sort):
        self.items = items_to_sort
        self.sorted_items=[]
        
        ### your implementation for selection sort goes here 
        self.quick_sort(0, len(items_to_sort)-1)
        
        self.sorted_items = self.items
        
    def quick_sort(self, p, r):
        if p < r:
            q = self.partition(p, r)
            self.quick_sort(p, q-1)
            self.quick_sort(q+1, r)
        return
    
    def partition(self, p, r):
        x = self.items[r]
        i = p - 1
        for j in range(p, r):
            if self.items[j] < x:
                i += 1
                self.items[i], self.items[j] = self.items[j], self.items[i]
        self.items[i + 1], self.items[r] = self.items[r], self.items[i + 1]
        return i + 1

    def get_sorted(self,):
        return self.sorted_items

# test all algorithm implementations
test_case = []   
print("Test case array input: ",test_case)

#example run for QuickSort
quick_sort = QuickSort(test_case)
print("After sorting by Quick Sort: ",quick_sort.get_sorted())

#testing was done in seperate jupyter notebook; each implementation was checked against python's built in sort()

# run all algorithms
def experiment_A():
    
    # Insert your code for experiment A design here 
    N = 80
    
    #bubble:
    run_times = []
    for _ in range(N):
        L = create_random_list(10000,50000)
        start = timeit.default_timer()
        
        bubble = BubbleSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times)) #technically, i ran the code seperately(in a jupyter notebook) so i can title each graph differently
    #means were manually recorded
    
    #insertion:
    run_times = []
    for _ in range(N):
        L = create_random_list(10000,50000)
        start = timeit.default_timer()

        insertion = InsertionSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times))

    #selection:
    run_times = []
    for _ in range(N):
        L = create_random_list(10000,50000)
        start = timeit.default_timer()

        selection = SelectionSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
    
    draw_plot(run_times, np.mean(run_times))
        
    #merge:
    run_times = []
    for _ in range(N):
        L = create_random_list(10000,50000)
        start = timeit.default_timer()

        merge = MergeSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
    
    draw_plot(run_times, np.mean(run_times))

    #quick:
    run_times = []
    for _ in range(N):
        L = create_random_list(10000,50000)
        start = timeit.default_timer()

        quick = QuickSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times))

    return 0

def experiment_B():
    
    # Insert your code for experiment B design here 
    N = 100
    
    #bubble:
    run_times = []
    for _ in range(N):
        L = create_near_sorted_list(5000,7000)
        start = timeit.default_timer()
        
        bubble = BubbleSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times)) #technically, i ran the code seperately(in a jupyter notebook) so i can title each graph differently
    #means were manually recorded
    
    #insertion:
    run_times = []
    for _ in range(N):
        L = create_near_sorted_list(5000,7000)
        start = timeit.default_timer()

        insertion = InsertionSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times))

    #selection:
    run_times = []
    for _ in range(N):
        L = create_near_sorted_list(5000,7000)
        start = timeit.default_timer()

        selection = SelectionSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
    
    draw_plot(run_times, np.mean(run_times))
        
    #merge:
    run_times = []
    for _ in range(N):
        L = create_near_sorted_list(5000,7000)
        start = timeit.default_timer()

        merge = MergeSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
    
    draw_plot(run_times, np.mean(run_times))

    #quick:
    run_times = []
    for _ in range(N):
        L = create_near_sorted_list(5000,7000)
        start = timeit.default_timer()

        quick = QuickSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times))

    return 0

def experiment_C():
    
    
    # Insert your code for experiment C design here 
    N = 100
    
    #bubble:
    run_times = []
    for _ in range(N):
        L = create_reverse_list(10000,50000)
        start = timeit.default_timer()
        
        bubble = BubbleSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times)) #technically, i ran the code seperately(in a jupyter notebook) so i can title each graph differently
    #means were manually recorded
    
    #insertion:
    run_times = []
    for _ in range(N):
        L = create_reverse_list(10000,50000)
        start = timeit.default_timer()

        insertion = InsertionSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times))

    #selection:
    run_times = []
    for _ in range(N):
        L = create_reverse_list(10000,50000)
        start = timeit.default_timer()

        selection = SelectionSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
    
    draw_plot(run_times, np.mean(run_times))
        
    #merge:
    run_times = []
    for _ in range(N):
        L = create_reverse_list(10000,50000)
        start = timeit.default_timer()

        merge = MergeSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
    
    draw_plot(run_times, np.mean(run_times))

    #quick:
    sys.setrecursionlimit(10000)
    
    run_times = []
    for _ in range(N):
        L = create_reverse_list(10000,50000)
        start = timeit.default_timer()

        quick = QuickSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times))
    
    sys.setrecursionlimit(3000) #putting it back for my machine
    
    return 0

def experiment_D():
    
    # Insert your code for experiment D design here 
    sizes = [50, 500, 1000, 2000, 5000] #reduced list sizes
    N = 80
    
    averages = []
    for i in sizes:
        run_times = []
        for _ in range(N):
            L = create_random_list(i, i*2) #not using random sorted lists
            start = timeit.default_timer()

            bubble = BubbleSort(L)

            stop = timeit.default_timer()
            run_times.append(stop-start)
        averages.append(np.mean(run_times))
        
    draw_plot(averages, None) #ran individually, with changed labels and everything of course
    #i also commented out the code drawing the red average line as it makes no sense here
    #averages were also recorded in seperate file

    averages = []
    for i in sizes:
        run_times = []
        for _ in range(N):
            L = create_random_list(i, i*2)
            start = timeit.default_timer()

            insertion = InsertionSort(L)

            stop = timeit.default_timer()
            run_times.append(stop-start)
        averages.append(np.mean(run_times))
        
    draw_plot(averages, None)

    averages = []
    for i in sizes:
        run_times = []
        for _ in range(N):
            L = create_random_list(i, i*2)
            start = timeit.default_timer()

            selection = SelectionSort(L)

            stop = timeit.default_timer()
            run_times.append(stop-start)
        averages.append(np.mean(run_times))
        
    draw_plot(averages, None)
    
    averages = []
    for i in sizes:
        run_times = []
        for _ in range(N):
            L = create_random_list(i, i*2)
            start = timeit.default_timer()

            merge = MergeSort(L)

            stop = timeit.default_timer()
            run_times.append(stop-start)
        averages.append(np.mean(run_times))
        
    draw_plot(averages, None)
    
    averages = []
    for i in sizes:
        run_times = []
        for _ in range(N):
            L = create_random_list(i, i*2)
            start = timeit.default_timer()

            quick = QuickSort(L)

            stop = timeit.default_timer()
            run_times.append(stop-start)
        averages.append(np.mean(run_times))
        
    draw_plot(averages, None)

    return 0

def experiment_E():
    
    # Insert your code for experiment E design here 
    N = 100
    
    #bubble:
    run_times = []
    for _ in range(N):
        L = reduced_unique_list(5000,10000)
        start = timeit.default_timer()
        
        bubble = BubbleSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times)) #technically, i ran the code seperately(in a jupyter notebook) so i can title each graph differently
    #means were manually recorded
    
    #insertion:
    run_times = []
    for _ in range(N):
        L = reduced_unique_list(5000,10000)
        start = timeit.default_timer()

        insertion = InsertionSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times))

    #selection:
    run_times = []
    for _ in range(N):
        L = reduced_unique_list(5000,10000)
        start = timeit.default_timer()

        selection = SelectionSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
    
    draw_plot(run_times, np.mean(run_times))
        
    #merge:
    run_times = []
    for _ in range(N):
        L = reduced_unique_list(5000,10000)
        start = timeit.default_timer()

        merge = MergeSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
    
    draw_plot(run_times, np.mean(run_times))

    #quick:
    run_times = []
    for _ in range(N):
        L = reduced_unique_list(5000,10000)
        start = timeit.default_timer()

        quick = QuickSort(L)

        stop = timeit.default_timer()
        run_times.append(stop-start)
        
    draw_plot(run_times, np.mean(run_times)) 

    return 0

# call each experiment
experiment_A()
experiment_B()
experiment_C()
experiment_D()
experiment_E()
