#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_REGIONS 100
#define MAX_FOOD_TYPES 10

typedef struct {
    int id;
    char name[50];
    int hungerLevel; // Priority for resource allocation
} Region;

typedef struct {
    int regionID;
    int foodType;
    int quantity;
    int expiryDays;
} FoodBatch;

typedef struct Node {
    FoodBatch batch;
    struct Node *next;
} Node;

typedef struct {
    Node *front, *rear;
} LinkedListQueue;

typedef struct {
    int inventory[MAX_FOOD_TYPES]; // Quantities of each food type
} HashTable[MAX_REGIONS];

typedef struct {
    int graph[MAX_REGIONS][MAX_REGIONS];
    Region regions[MAX_REGIONS];
    int numRegions;
} FoodNetwork;

// Priority Queue
typedef struct {
    int id;
    int priority; // Hunger level
} PriorityQueue[MAX_REGIONS];

int pqSize = 0;

// Queue functions
void enqueue(LinkedListQueue *queue, FoodBatch batch) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->batch = batch;
    newNode->next = NULL;
    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
        return;
    }
    queue->rear->next = newNode;
    queue->rear = newNode;
}

FoodBatch dequeue(LinkedListQueue *queue) {
    if (queue->front == NULL) {
        FoodBatch empty = {-1, -1, 0, 0};
        return empty;
    }
    Node *temp = queue->front;
    FoodBatch batch = temp->batch;
    queue->front = queue->front->next;
    if (queue->front == NULL) queue->rear = NULL;
    free(temp);
    return batch;
}

// Priority Queue functions
void insert(PriorityQueue pq, int id, int priority) {
    int i = pqSize++;
    while (i > 0 && pq[(i - 1) / 2].priority < priority) {
        pq[i] = pq[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    pq[i].id = id;
    pq[i].priority = priority;
}

int extractMax(PriorityQueue pq) {
    if (pqSize == 0) return -1;
    int id = pq[0].id;
    pq[0] = pq[--pqSize];
    int i = 0;
    while (2 * i + 1 < pqSize) {
        int left = 2 * i + 1, right = 2 * i + 2, max = i;
        if (pq[left].priority > pq[max].priority) max = left;
        if (right < pqSize && pq[right].priority > pq[max].priority) max = right;
        if (max == i) break;
        PriorityQueue temp;
        temp->id = pq[i].id;
        temp->priority = pq[i].priority;

        pq[i].id = pq[max].id;
        pq[i].priority = pq[max].priority;

        pq[max].id = temp->id;
        pq[max].priority= temp->priority;

        i = max;
    }
    return id;
}

// Graph functions
void addEdge(FoodNetwork *network, int src, int dest, int weight) {
    network->graph[src][dest] = weight;
    network->graph[dest][src] = weight; // Bidirectional
}

void initGraph(FoodNetwork *network, int numRegions) {
    network->numRegions = numRegions;
    for (int i = 0; i < numRegions; i++) {
        for (int j = 0; j < numRegions; j++) {
            network->graph[i][j] = INT_MAX; // No direct route
        }
    }
}

// Main logic
void distributeFood(FoodNetwork *network, HashTable inventory) {
    PriorityQueue pq;
    for (int i = 0; i < network->numRegions; i++) {
        insert(pq, i, network->regions[i].hungerLevel);
    }

    while (pqSize > 0) {
        int regionID = extractMax(pq);
        printf("Distributing food to region %s (Hunger Level: %d)\n",
               network->regions[regionID].name, network->regions[regionID].hungerLevel);
        for (int foodType = 0; foodType < MAX_FOOD_TYPES; foodType++) {
            if (inventory[regionID].inventory[foodType] > 0) {
                printf("  - Food Type %d: %d units distributed\n",
                       foodType, inventory[regionID].inventory[foodType]);
                inventory[regionID].inventory[foodType] = 0; // Consumed
            }
        }
    }
}

// Test the system
int main() {
    FoodNetwork network;
    HashTable inventory = {0};

    initGraph(&network, 3);

    strcpy(network.regions[0].name, "Region A");
    network.regions[0].hungerLevel = 5;

    strcpy(network.regions[1].name, "Region B");
    network.regions[1].hungerLevel = 8;

    strcpy(network.regions[2].name, "Region C");
    network.regions[2].hungerLevel = 3;

    inventory[0].inventory[1] = 100; // Region A food type 1
    inventory[1].inventory[2] = 200; // Region B food type 2

    distributeFood(&network, inventory);

    return 0;
}
