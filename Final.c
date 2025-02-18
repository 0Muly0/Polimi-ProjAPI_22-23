#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

//COMMANDS
#define ADD_ST "aggiungi-stazione"
#define REM_ST "demolisci-stazione"
#define ADD_AU "aggiungi-auto"
#define REM_AU "rottama-auto"
#define PLAN "pianifica-percorso"

typedef struct Car Car;
typedef struct CarHeap CarHeap;
typedef struct Station Station;
typedef struct Highway Highway;
typedef struct BSFStat BFSStat;
typedef struct QueueNode QueueNode;
typedef struct Queue Queue;

typedef enum { RED, BLACK } Color;
typedef enum { W, B, G } BFSColor;

typedef struct Car {
    int autonomy;
    short int quantity;
} Car;

typedef struct CarHeap {
    Car** cars;
    int heapLength;
    int heapSize;
} CarHeap;

typedef struct Station{
    int distance;

    CarHeap* carpool;

    Color color;
    struct Station* left;
    struct Station* right;
    struct Station* parent;
} Station;

typedef struct Highway{
    Station* staNIL;
    Station* staRoot;
} Highway;

typedef struct BSFStat {
    int distance;
    int maxAuto;
    BFSColor color;
    struct BSFStat* path;
} BSFStat;

typedef struct QueueNode {
    BSFStat* s;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    struct QueueNode* h;
    struct QueueNode* t;
    int size;
} Queue;

//STATION FUNCTIONS
void leftRotateStat(Highway* h, Station* x);
void rightRotateStat(Highway* h, Station* x);
Station* statSuccessor(Highway* h, Station* x);
Station* statPredecessor(Highway* h, Station* x);
void transplantStat(Highway* h, Station* u, Station* v);
Station* findMinStat(Station* s, Station* NIL);
Station* findMaxStat(Station* s, Station* NIL);
Station* statFind(Highway* h, int dist);
Station* statInsert(Highway* h, int dist);
void statInsertFixup(Highway* h, Station* z);
void statDelete(Highway* h, Station* s);
void statDeleteFixup(Highway* h, Station* s);
void planTrip(Highway* h, Station* s, Station* t, bool fwd);
void printTripList(Station** path, int pathSize);
void printTripBfs(BSFStat* s, BSFStat* t);

//CAR FUNCTIONS
void maxHeapify(Car** cars, int size, int idx);
void buildMaxHeap(CarHeap* carpool);
int carFind(CarHeap** carpool, int autonomy);
int extractMaxCar(CarHeap* carpool);
void carInsert(CarHeap** carpool, int autonomy);
void carDelete(CarHeap** carpool, int idx);
void carFree(CarHeap** carpool);


int main() {
    Highway* h = (Highway*) malloc(sizeof(Highway));
    h->staRoot = NULL;
    h->staNIL = (Station*)malloc(sizeof(Station));
    h->staNIL->color = BLACK;
    h->staNIL->distance = -1;

    int space = -1;
    int ch = getc_unlocked(stdin);
    int commLength = 0;
    int p0L = 0;
    int p1L = 0;
    char* comm = (char*)malloc(20*sizeof(char));
    char* p0 = (char*)malloc(11*sizeof(char));
    char* p1 = (char*)malloc(11*sizeof(char));
    Station* s = NULL;

    while(ch != EOF) {

        if (isalpha(ch) || ch == 45) {
            //Command reading
            comm[commLength] = (char)ch;
            commLength++;
            ch = getc_unlocked(stdin);
        } else if (space == -1 && ch == 32) {
            //First space after command
            comm[commLength] = '\0';
            space = 1;
            ch = getc_unlocked(stdin);
        } else if (isdigit(ch) || space == 2) {
            if (space == 1) {
                do {
                    p0[p0L] = (char)ch;
                    p0L++;
                    ch = getc_unlocked(stdin);
                } while (ch != 32 && ch != 10);

                space = 2;
                if(ch == 32) {
                    ch = getc_unlocked(stdin);
                }
            } else {
                space = -1;

                //Extract first number
                int x = atoi(p0);
                s = statFind(h, x);

                if(strcmp(comm, ADD_ST) == 0) {
                    if(s == h->staNIL) {
                        s = statInsert(h, x);

                        //Avoiding car number
                        while(isdigit(ch)) {
                            ch = getc_unlocked(stdin);
                        }

                        if(ch != 10) {
                            ch = getc_unlocked(stdin);

                            //Populating carpool
                            while(ch != 10) {
                                p1L = 0;
                                do {
                                    p1[p1L] = (char)ch;
                                    p1L++;
                                    ch = getc_unlocked(stdin);
                                } while(isdigit(ch));

                                int aut = atoi(p1);

                                int c = carFind(&s->carpool,aut);
                                if(c != -1) {
                                    s->carpool->cars[c]->quantity++;
                                } else {
                                    carInsert(&s->carpool,aut);
                                }

                                if(ch == 32) {
                                    free(p1);
                                    p1 = (char*)malloc(11*sizeof(char));
                                    ch = getc_unlocked(stdin);
                                }
                            }
                        }

                        printf("aggiunta\n");
                    } else {
                        while(ch != 10) {
                            ch = getc_unlocked(stdin);
                        }
                        printf("non aggiunta\n");
                    }

                } else if (strcmp(comm, REM_ST) == 0) {

                    if(s != h->staNIL) {
                        statDelete(h, s);
                        printf("demolita\n");
                    } else {
                        printf("non demolita\n");
                    }

                } else if (strcmp(comm, ADD_AU) == 0) {

                    if(s != h->staNIL) {
                        p1L = 0;
                        do {
                            p1[p1L] = (char)ch;
                            p1L++;
                            ch = getc_unlocked(stdin);
                        } while(isdigit(ch));

                        int aut = atoi(p1);

                        int c = carFind(&s->carpool,aut);
                        if(c != -1) {
                            s->carpool->cars[c]->quantity++;
                        } else {
                            carInsert(&s->carpool,aut);
                        }

                        printf("aggiunta\n");
                    } else {
                        while(ch != 10) {
                            ch = getc_unlocked(stdin);
                        }
                        printf("non aggiunta\n");
                    }
                } else if (strcmp(comm, REM_AU) == 0) {

                    if(s != h->staNIL) {
                        p1L = 0;
                        do {
                            p1[p1L] = (char)ch;
                            p1L++;
                            ch = getc_unlocked(stdin);
                        } while(isdigit(ch));

                        int aut = atoi(p1);

                        int c = carFind(&s->carpool,aut);
                        if(c != -1) {
                            if(s->carpool->cars[c]->quantity > 1) {
                                s->carpool->cars[c]->quantity--;
                            } else {
                                carDelete(&s->carpool, c);
                            }

                            printf("rottamata\n");
                        } else {
                            printf("non rottamata\n");
                        }
                    } else {
                        while(ch != 10) {
                            ch = getc_unlocked(stdin);
                        }
                        printf("non rottamata\n");
                    }

                } else if (strcmp(comm, PLAN) == 0) {
                    p1L = 0;
                    do {
                        p1[p1L] = (char)ch;
                        p1L++;
                        ch = getc_unlocked(stdin);
                    } while(isdigit(ch));

                    //Extract pointers
                    Station* A = statFind(h, x);
                    Station* B = statFind(h, atoi(p1));

                    //Generates BFS tree for start station
                    if(A->distance < B->distance) {
                        planTrip(h, A, B, 1);
                    } else {
                        planTrip(h, A, B, 0);
                    }
                    printf("\n");
                }
            }
        } else if (ch == 10) {
            free(p0);
            free(p1);
            free(comm);

            p0 = (char*)malloc(11*sizeof(char));
            p1 = (char*)malloc(11*sizeof(char));
            comm = (char*)malloc(20*sizeof(char));
            p0L = 0;
            p1L = 0;
            commLength = 0;
            ch = getc_unlocked(stdin);
        }
    }

    return 0;
}

//QUEUE
void enqueue(Queue* q, BSFStat* s) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->s = s;
    newNode->next = NULL;

    if(q->t == NULL) {
        q->h = newNode;
        q->t = newNode;
    } else {
        q->t->next = newNode;
        q->t = newNode;
    }
    q->size++;
}

BSFStat* dequeue(Queue* q) {
    if(q->h == NULL) {
        return NULL;
    } else {
        QueueNode* temp = q->h;
        q->h = q->h->next;

        if(q->h == NULL) {
            q->t = NULL;
        }
        q->size--;

        BSFStat* s = temp->s;
        free(temp);
        return s;
    }
}

void freeQueue(Queue* q) {
    while (q->h != NULL) {
        dequeue(q);
    }
    free(q);
}

void bfs(Highway* h, Station* s, Station* t) {
    BSFStat** support = (BFSStat**)malloc(sizeof(BFSStat*));
    int suppSize = 0;
    int suppLen = 0;

    Station* curr = s;
    Station* tPred = statPredecessor(h, t);
    while(curr != tPred) {
        buildMaxHeap(curr->carpool);

        BSFStat* supp = (BFSStat*)malloc(sizeof(BFSStat));
        supp->distance = curr->distance;
        supp->maxAuto = extractMaxCar(curr->carpool);
        if(curr == t) {
            supp->color = G;
        } else {
            supp->color = W;
        }
        supp->path = NULL;

        if(suppSize >= suppLen) {
            suppLen += 5;
            support = (BFSStat**)realloc(support, suppLen*sizeof(BFSStat*));
        }
        support[suppSize] = supp;
        suppSize++;

        curr = statPredecessor(h, curr);
    }

    Queue* Q = (Queue*)malloc(sizeof(Queue));
    Q->h = NULL;
    Q->t = NULL;
    Q->size = 0;
    enqueue(Q, support[suppSize-1]);

    int i=suppSize-1;
    while(Q->size != 0) {
        BSFStat* u = dequeue(Q);
        BSFStat* prec;

        for(int k = i-1; k>=0; k--) {
            prec = support[k];

            if(prec->color == W && (prec->distance - prec->maxAuto <= u->distance)) {
                prec->color = G;
                prec->path = u;
                enqueue(Q, prec);
            }
        }

        u->color = B;
        i--;
    }

    freeQueue(Q);
    printTripBfs(support[0], support[suppSize-1]);

    for(int j = suppSize-1; j>=0; j--) {
        free(support[j]);
    }
    free(support);
}

//RED AND BLACK STAT
void leftRotateStat(Highway* h, Station* x) {
    Station** root = &h->staRoot;

    Station * y = x->right;
    x->right = y->left;

    if(y->left != h->staNIL) {
        y->left->parent = x;
    }
    y->parent = x->parent;

    if(x->parent == h->staNIL) {
        *root = y;
    } else if(x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

void rightRotateStat(Highway* h, Station* y) {
    Station** root = &h->staRoot;

    Station* x = y->left;
    y->left = x->right;

    if (x->right != h->staNIL) {
        x->right->parent = y;
    }

    x->parent = y->parent;
    if (y->parent == h->staNIL) {
        *root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}

Station* statSuccessor(Highway* h, Station* x) {
    Station* NIL = h->staNIL;

    if(x->right != NIL) {
        return findMinStat(x->right, NIL);
    }

    Station* y = x->parent;
    while(y != NIL && x==y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

Station* statPredecessor(Highway* h, Station* x) {
    Station* NIL = h->staNIL;

    if(x->left != NIL) {
        return findMaxStat(x->left, NIL);
    }

    Station* y = x->parent;
    while(y != NIL && x==y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

Station* minPredecessor(Highway* h, Station* s, Station* t) {
    Station* curr = findMinStat(h->staRoot, h->staNIL);

    while(curr != t) {
        int maxAuto = extractMaxCar(curr->carpool);

        if(curr->distance + maxAuto >= t->distance) {
            if(curr->distance > s->distance) {
                return curr;
            } else {
                return s;
            }
        }

        curr = statSuccessor(h, curr);
    }

    return h->staNIL;
}

void transplantStat(Highway* h, Station* u, Station* v) {
    Station** root = &h->staRoot;

    if(u->parent == h->staNIL) {
        *root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }

    v->parent = u->parent;
}

Station* findMinStat(Station* s, Station* NIL) {
    if(s == NULL) {
        return NIL;
    }

    while (s->left != NIL) {
        s = s->left;
    }
    return s;
}

Station* findMaxStat(Station* s, Station* NIL) {
    if(s == NULL) {
        return NIL;
    }

    while (s->right != NIL) {
        s = s->right;
    }
    return s;
}

Station* statFind(Highway* h, int dist) {
    Station* s = h->staRoot;
    Station* NIL = h->staNIL;

    if(s == NULL) {
        return NIL;
    } else {
        while(s != NIL && s->distance != dist) {
            if(dist < s->distance) {
                s = s->left;
            } else {
                s = s->right;
            }
        }
        return s;
    }
}

Station* statInsert(Highway* h, int dist) {
    Station** root = &h->staRoot;
    Station* newStat = (Station*)malloc(sizeof(Station));

    newStat->distance = dist;
    newStat->carpool = (CarHeap*)malloc(sizeof(CarHeap));
    newStat->carpool->cars = NULL;
    newStat->carpool->heapLength = 0;
    newStat->carpool->heapSize = 0;

    newStat->color = RED;
    newStat->left = h->staNIL;
    newStat->right = h->staNIL;
    newStat->parent = h->staNIL;

    if((*root)==NULL) {
        newStat->color = BLACK;
        (*root) = newStat;
    } else {
        Station* y = h->staNIL;
        Station* x = *root;

        while(x != h->staNIL) {
            y = x;
            if(dist < x->distance) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        newStat->parent = y;
        if(y == h->staNIL) {
            *root = newStat;
        } else if (dist < y->distance) {
            y->left = newStat;
        } else {
            y->right = newStat;
        }

        statInsertFixup(h,newStat);
    }

    return newStat;
}

void statInsertFixup(Highway* h, Station* z) {
    Station** root = &h->staRoot;

    while(z->parent->color == RED) {
        if(z->parent == z->parent->parent->left) {
            Station* y = z->parent->parent->right;

            if(y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotateStat(h,z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotateStat(h,z->parent->parent);
            }
        } else {
            Station * y = z->parent->parent->left;

            if(y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotateStat(h,z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotateStat(h,z->parent->parent);
            }
        }
    }

    (*root)->color = BLACK;
}

void statDelete(Highway* h, Station* z) {
    Station** root = &h->staRoot;

    Station* x;
    Station* y = z;
    Color yOC = y->color;

    if(z->left == h->staNIL) {
        x = z->right;
        transplantStat(h,z,z->right);
    } else if (z->right == h->staNIL) {
        x = z->left;
        transplantStat(h,z,z->left);
    } else {
        y = findMinStat(z->right,h->staNIL);
        yOC = y->color;
        x = y->right;

        if(y->parent == z) {
            x->parent = y;
        } else {
            transplantStat(h,y,y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        transplantStat(h,z,y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if(yOC == BLACK) {
        statDeleteFixup(h,x);
    }

    if((*root)== h->staNIL) {
        *root = NULL;
    }

    carFree(&z->carpool);
    free(z);
}

void statDeleteFixup(Highway* h, Station* x) {
    Station** root = &h->staRoot;

    while(x!= *root && x->color == BLACK) {
        if(x == x->parent->left) {
            Station* w = x->parent->right;

            if(w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotateStat(h,x->parent);
                w = x->parent->right;
            }

            if(w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotateStat(h,w);
                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                leftRotateStat(h,x->parent);
                x = *root;
            }
        } else {
            Station * w = x->parent->left;

            if(w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotateStat(h,x->parent);
                w = x->parent->left;
            }

            if(w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotateStat(h,w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotateStat(h,x->parent);
                x = *root;
            }
        }
    }

    x->color = BLACK;
}

void planTrip(Highway* h, Station* s, Station* t, bool fwd) {

    if(fwd == 1) {
        int pathSize = 1;
        Station** path = (Station**)malloc(200*sizeof(Station*));
        path[0] = t;

        Station* curr = s;
        do {
            buildMaxHeap(curr->carpool);
            curr = statSuccessor(h, curr);
        } while (curr != t);

        // s < t
        Station* mp = minPredecessor(h, s, t);

        while(mp != s && mp != h->staNIL) {
            path[pathSize] = mp;
            pathSize++;

            mp = minPredecessor(h, s, mp);
        }

        if(mp == h->staNIL) {
            printf("nessun percorso");
        } else {
            path[pathSize] = s;
            pathSize++;
            printTripList(path, pathSize);
            free(path);
        }
    } else {
        bfs(h, s, t);
    }
}

void printTripList(Station** path, int pathSize) {
    for(int i = pathSize-1; i>=0; i--) {
        if(i != 0) {
            printf("%d ", path[i]->distance);
        } else {
            printf("%d", path[i]->distance);
        }
    }
}

void printTripBfs(BSFStat* s, BFSStat* t) {
    if(s->distance == t->distance) {
        printf("%d", t->distance);
    } else if (s->path == NULL) {
        printf("nessun percorso");
    } else {
        printf("%d ", s->distance);
        printTripBfs(s->path, t);
    }
}

//HEAP
void maxHeapify(Car** cars, int size, int idx) {
    int max = idx;
    int l = 2*idx + 1;
    int r = 2*idx + 2;

    if(l < size && cars[l]->autonomy > cars[idx]->autonomy) {
        max = l;
    }

    if(r < size && cars[r]->autonomy > cars[max]->autonomy) {
        max = r;
    }

    if(max != idx) {
        Car* temp = cars[max];
        cars[max] = cars[idx];
        cars[idx] = temp;

        maxHeapify(cars, size, max);
    }
}

void buildMaxHeap(CarHeap* carpool) {
    for(int i = carpool->heapSize/2 - 1; i >= 0; i--) {
        maxHeapify(carpool->cars, carpool->heapSize, i);
    }
}

int carFind(CarHeap** carpool, int autonomy) {

    for(int i=0; i<(*carpool)->heapSize; i++) {
        if((*carpool)->cars[i]->autonomy == autonomy) {
            return i;
        }
    }

    return -1;
}

int extractMaxCar(CarHeap* carpool) {
    if (carpool->heapSize == 0) {
        return 0;
    } else {
        return carpool->cars[0]->autonomy;
    }
}

void carInsert(CarHeap** carpool, int autonomy) {
    //Initialize car
    Car* newCar = (Car*)malloc(sizeof(Car));
    newCar->autonomy = autonomy;
    newCar->quantity = 1;

    //Elongate the carHeap if necessary
    if((*carpool)->heapSize >= (*carpool)->heapLength) {
        (*carpool)->heapLength += 5;
        (*carpool)->cars = (Car**)realloc((*carpool)->cars, (*carpool)->heapLength * sizeof(Car*));
    }

    //Insert car into array
    (*carpool)->cars[(*carpool)->heapSize] = newCar;
    (*carpool)->heapSize++;
}

void carDelete(CarHeap** carpool, int idx) {

    free((*carpool)->cars[idx]);
    for(int i=idx; i<(*carpool)->heapSize; i++) {
        (*carpool)->cars[i] = (*carpool)->cars[i+1];
    }

    (*carpool)->heapSize--;
}

void carFree(CarHeap** carpool) {
    for(int i=(*carpool)->heapSize-1; i>=0; i--) {
        free((*carpool)->cars[i]);
    }
    free(*carpool);
}