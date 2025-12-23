#Culmination of our hard work!! 
#Incredible credit to my group members, Jinny Kim and Rebecca Yang!!

import csv
import math
import time
import heapq
import pandas as pd
import random
import matplotlib.pyplot as plt
from io import StringIO
import numpy as np

# data structures and useful functions ----------------------------------------------
class WeightedGraph: 
    # using adjacency list
    def __init__(self, nodes):
        self.graph = {} #Dictionary to store adjacency lists for each node.
        self.weight={} #Dictionary to store weights of edges 
        self.line = {}
        for i in range(nodes): #because station starts from id 1 
            self.graph[i] = []
        
    def has_edge(self, src, dst):
        return dst in self.graph[src]
    
    def add_edge(self,src,dst,weight,line=None):

        if not self.has_edge(src,dst): #Prevent duplicate edges 
            self.graph[src].append(dst)
            self.weight[(src,dst)]=weight
            if line is not None:
                self.line[(src, dst)] = line

            self.graph[dst].append(src)
            self.weight[(dst,src)]= weight #Store weight for both directions 
            if line is not None:
                self.line[(dst, src)] = line


    def get_total_weight(self,):
        total = 0
        for src, neighbors in enumerate(self.graph): #src: current node, enumerate - 0 value, 1 value, 2 value..
            for dst in neighbors: #src value is fixed, dst is iterated within neighbors until its done. 
                    total+= self.weight[(src,dst)]

        return total/2 #Divide by 2 to avoid double counting (ex: 0->1 and 1->0, they are the same.)

    def get_graph(self,):
        return self.graph
    

    #get the weight of the edge between node1 and node2.
    def get_weight(self, node1, node2):
        if (node1,node2) in self.weight:
            return self.weight[(node1,node2)]
        else:
            return "No distance: Station1 and Station2 are not neighbors"
        
    
    def get_size(self,):
        return len(self.graph)  
    
class Node:
    def __init__(self, value, key=float('inf')):
        self.value = value   
        self.key = key     

class MinHeap:
    def __init__(self, data):
        self.items = data 
        self.length = len(data) 
        self.map = {} 
        self.build_heap()  
        self.map = {}
        for i in range(self.length):
            self.map[self.items[i].value] = i

    def find_left_index(self, index):
        return 2 * (index + 1) - 1

    def find_right_index(self, index):
        return 2 * (index + 1)

    def find_parent_index(self, index):
        return (index + 1) // 2 - 1

    def heapify(self, index):
        smallest_known_index = index
        if self.find_left_index(index) < self.length and self.items[self.find_left_index(index)].key < self.items[index].key:
            smallest_known_index = self.find_left_index(index)
        if self.find_right_index(index) < self.length and self.items[self.find_right_index(index)].key < self.items[smallest_known_index].key:
            smallest_known_index = self.find_right_index(index)
        if smallest_known_index != index:
            self.items[index], self.items[smallest_known_index] = self.items[smallest_known_index], self.items[index]
            self.map[self.items[index].value] = index
            self.map[self.items[smallest_known_index].value] = smallest_known_index
            self.heapify(smallest_known_index) 

    def build_heap(self):
        for i in range(self.length // 2 - 1, -1, -1): 
            self.heapify(i)

    def insert(self, node):
        if len(self.items) == self.length:
            self.items.append(node) 
        else:
            self.items[self.length] = node  
        self.map[node.value] = self.length  
        self.length += 1 
        self.swim_up(self.length - 1)  

    def extract_min(self):
        self.items[0], self.items[self.length - 1] = self.items[self.length - 1], self.items[0]
        self.map[self.items[self.length - 1].value] = self.length - 1
        self.map[self.items[0].value] = 0
        min_node = self.items[self.length - 1]
        self.length -= 1 
        self.map.pop(min_node.value)  
        self.heapify(0) 
        return min_node

    def decrease_key(self, value, new_key):
        if new_key >= self.items[self.map[value]].key:
            return
        index = self.map[value]  
        self.items[index].key = new_key 
        self.swim_up(index)  

    def swim_up(self, index):
        while index > 0 and self.items[index].key < self.items[self.find_parent_index(index)].key: 
            self.items[index], self.items[self.find_parent_index(index)] = self.items[self.find_parent_index(index)], self.items[index]
            self.map[self.items[index].value] = index
            self.map[self.items[self.find_parent_index(index)].value] = self.find_parent_index(index)
            index = self.find_parent_index(index)  

    def is_empty(self):
        return self.length == 0


#takes in the IDs of two stations and computes the euclidean distance between them
#uses the longitude and latitude of the stations from the stations dictionary
def station_dist(a, b):
    latA = stations[a]["lat"]
    lonA = stations[a]["lon"]
    latB = stations[b]["lat"]
    lonB = stations[b]["lon"]
    return math.sqrt((latB - latA) ** 2 + (lonB - lonA) ** 2)


# searching algorithms ----------------------------------------------
# (RETURNS ([SHORTEST PATH AS ARRAY OF NODES], NUMBER OF LINES (if 1, no transfers), LENGTH OF SHORTEST PATH))
# RETURNS INFINITY IF PATH NOT FOUND (PATH DOESN'T EXIST)

# reconstruct path function with line count
def reconstruct_path(graph, dict, dst):
    path = [dst]
    prev_node = dict[dst]
    while prev_node is not None:
        path.append(prev_node)
        prev_node = dict[prev_node]
    path.reverse()
    
    lines = set()
    for i in range(len(path) - 1):
        u, v = path[i], path[i + 1]
        if (u, v) in graph.line:
            lines.add(graph.line[(u, v)])
        elif (v, u) in graph.line:
            lines.add(graph.line[(v, u)])
        

    transfers = len(lines) - 1
    # if 1, no transfers
    # if 2, one transfer
    # if more lines were taken along the path, multiple transfers
    return path, transfers

# src dst dijkstra on undirected g

#2.1 Dijkstra's
def dijkstra(graph, source, destination):

    #Initialization
    dist = {}
    prev = {}
    for node in graph.graph:
        dist[node] = math.inf
        prev[node] = None
    dist[source] = 0
    pq = MinHeap([Node(node, dist[node]) for node in graph.graph])

    while not pq.is_empty():
        u = pq.extract_min().value
        
        if u == destination:
            return reconstruct_path(graph, prev, destination), dist[destination]

        for v in graph.graph[u]:
            alt = dist[u] + graph.weight[(u, v)]
            if alt < dist[v]:
                dist[v] = alt
                prev[v] = u
                pq.decrease_key(v, alt)

    return math.inf #PATH NOT FOUND

# src dst A* on undirected g
# built in with specific heuristic of distance to destination, no seperate heuristic function
def A_Star(graph, source, destination): 

    #Initialization
    dist = {}
    prev = {}
    for node in graph.graph:
        dist[node] = math.inf
        prev[node] = None
    dist[source] = 0
    pq = [(station_dist(source,destination), source)]#f's are computed as needed
    # streamlined the comparison with how Python auto compares first value in a tuple first
    
    seen = set()

    while pq:
        _, u = heapq.heappop(pq)
        if u in seen: #dijkstra processes each node only once
            continue #only added because of the awkward way i used heapq ;-;
        
        if u == destination:
            return reconstruct_path(graph, prev, destination), dist[destination]
        
        seen.add(u)

        for v in graph.graph[u]:
            alt = dist[u] + graph.weight[(u, v)]
            if alt < dist[v]:
                dist[v] = alt
                prev[v] = u
                if v in seen: #limits duplicates in the pq
                    continue
                #might still have dupes but need to make sure smallest f is in pq
                #so it's like a primitive decrease_key lol
                heapq.heappush(pq, (station_dist(v,destination), v))
                
    return math.inf #PATH NOT FOUND


#data processing
#----------------------------------------------------------------------------------------------------------------------------

def calculate_euclidean_distance(lat1, lon1, lat2, lon2):
    return math.sqrt((lat2 - lat1) ** 2 + (lon2 - lon1) ** 2)

def build_tube_graph_from_stations_and_connections(stations_csv, connections_csv):

    # Parse stations data (from stations CSV)---------------------------------------------------------
    stations = {}
    csv_reader = csv.reader(StringIO(stations_csv))
    next(csv_reader)  # Skip header row
    
    for row in csv_reader:
        if not row or len(row) < 8:
            continue
        
        station_id = int(row[0])
        lat = float(row[1]) if row[1] else None
        lon = float(row[2]) if row[2] else None
        name = row[3].strip('"')
        display_name = row[4].strip('"') if row[4] and row[4] != "NULL" else name
        zone = row[6]
        total_lines = int(row[7]) if row[7] else 0
        rail = int(row[8]) if len(row) > 8 and row[8] else 0
        
        stations[station_id] = {
            "id": station_id,
            "name": name,
            "display_name": display_name,
            "lat": lat,
            "lon": lon,
            "zone": zone,
            "total_lines": total_lines,
            "rail": rail,
        }
    #-----------------------------------------------------------------------------------------------
    
    # Create the graph with number of stations
    max_station_id = max(stations.keys()) if stations else 0
    graph = WeightedGraph(max_station_id + 1)
    
    # Parse connections data (from connections CSV)
    csv_reader = csv.reader(StringIO(connections_csv))
    next(csv_reader)  # Skip header row

    # Now connect each station based on the provided connections
    for row in csv_reader:
        if not row or len(row) < 4:
            continue
        
        station1_id = int(row[0])
        station2_id = int(row[1])
        line_id = row[2]

        # Check if both stations exist in the station dictionary
        if station1_id in stations and station2_id in stations:
            lat1, lon1 = stations[station1_id]["lat"], stations[station1_id]["lon"]
            lat2, lon2 = stations[station2_id]["lat"], stations[station2_id]["lon"]
            
            # Calculate Euclidean distance between the stations
            if lat1 is not None and lon1 is not None and lat2 is not None and lon2 is not None:
                distance = calculate_euclidean_distance(lat1, lon1, lat2, lon2)
                graph.weight[(station1_id,station2_id)]= distance
                
                # Add the edge to the graph with time as weight
                graph.add_edge(station1_id, station2_id, distance)

                graph.line[(station1_id, station2_id)] = line_id
                graph.line[(station2_id, station1_id)] = line_id
    
    return graph, stations

# Read the CSV files
with open("london_stations.csv", "r", encoding="utf-8") as file:
    stations_csv = file.read()

with open("london_connections.csv", "r", encoding="utf-8") as file:
    connections_csv = file.read()

# Run the function
graph, stations = build_tube_graph_from_stations_and_connections(stations_csv, connections_csv) 
print(graph.get_graph())  
print(graph.get_weight(1,234)) 


# experiments ----------------------------------------------
station_lines = {s: set() for s in stations}
for (u, v), line in graph.line.items():
    station_lines[u].add(line)
    station_lines[v].add(line)

def pair_type(src, dst, transfers):
    if transfers == 0:
        return "same_line"
    elif transfers == 1:
        return "adjacent_line"
    else:
        return "multi_transfer"



results = []
station_ids = list(stations.keys())
sample_pairs = random.sample([(a, b) for a in station_ids for b in station_ids if a != b], 200)

for src, dst in sample_pairs:
    t1 = time.perf_counter()
    dijkstra_result, _ = dijkstra(graph, src, dst)
    dijkstra_time = time.perf_counter() - t1

    t2 = time.perf_counter()
    astar_result, _ = A_Star(graph, src, dst)
    astar_time = time.perf_counter() - t2

    if dijkstra_result == math.inf or astar_result == math.inf:
        continue

    _, transfers = dijkstra_result
    
    print(f"{src} → {dst}: {pair_type(src, dst, transfers)}")
    results.append({
        "src": src,
        "dst": dst,
        "dijkstra_time": dijkstra_time,
        "astar_time": astar_time,
        "transfers": transfers,
        "pair_type": pair_type(src, dst, transfers)
    })
df = pd.DataFrame(results)
for ptype in ["same_line", "adjacent_line", "multi_transfer"]:
    subset = df[df["pair_type"] == ptype]
    print(f"\n--- {ptype.upper()} ---")
    print("Avg Dijkstra Time:", subset["dijkstra_time"].mean())
    print("Avg A* Time:      ", subset["astar_time"].mean())
    print("Avg Transfers:    ", subset["transfers"].mean())
    print("Sample Size:      ", len(subset))

pair_types = ["same_line", "adjacent_line", "multi_transfer"]
avg_dijkstra = []
avg_astar = []
avg_transfers = []
counts = []

for p in pair_types:
    subset = df[df["pair_type"] == p]
    if not subset.empty:
        avg_dijkstra.append(subset["dijkstra_time"].mean())
        avg_astar.append(subset["astar_time"].mean())
        avg_transfers.append(subset["transfers"].mean())
        counts.append(len(subset))
    else:
        avg_dijkstra.append(0)
        avg_astar.append(0)
        avg_transfers.append(0)
        counts.append(0) 

#  Runtime Comparison 
x = np.arange(len(pair_types))
width = 0.35

plt.figure(figsize=(10, 6))
plt.bar(x, avg_dijkstra, width=width, label="Dijkstra", align="center")
plt.bar([i + width for i in x], avg_astar, width=width, label="A*", align="center")
plt.xticks([i + width / 2 for i in x], pair_types)
plt.ylabel("Average Runtime (seconds)")
plt.title("Dijkstra vs A* Runtime by Pair Type")
plt.legend()
plt.grid(axis="y")
plt.tight_layout()
plt.show()

#  Average Transfers 
plt.figure(figsize=(8, 5))
plt.bar(pair_types, avg_transfers, color="gray")
plt.ylabel("Average Transfers")
plt.title("Average Transfers by Pair Type")
plt.grid(axis="y")
plt.tight_layout()
plt.show()

# Sample Size
plt.figure(figsize=(8, 5))
plt.bar(pair_types, counts, color="lightblue")
plt.ylabel("Number of Pairs Sampled")
plt.title("Sample Count by Pair Type")
plt.grid(axis="y")
plt.tight_layout()
plt.show()
