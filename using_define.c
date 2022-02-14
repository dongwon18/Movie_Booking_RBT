/**
 * Copyright (C) 2020 by Dongwon Kim, All rights reserved.
 *
 * Assignment 3 of Algorithms
 *
 * File name : report3_2018312292_DongwonKim.c
 *
 * Summery of this code:
 *
 * Written by Dongwon Kim on November 24, 2020
 *            Student ID number : 2018312292
 *            School of Electrical Engineering
 *            Sungkyunkwan University
 *
 *  Modification History :
 *    1. Written by Dongwon Kim on Nov 24, 2020
 *
 *
 *  Compiler Used : gcc version 7.3.0, 32-bit
 *  Encoding : UTF - 8
 *  IDE used :Code Block 17.12
 */

#include <stdio.h>
#include <stdlib.h>

#define ID_MAX 10000
#define RAND_INPUT 3
#define DATE 3
#define TIME 3
#define MOVIE 3
#define SEAT 10
/**
 * enum for Red Black tree color
 */
typedef enum __color{
    BLACK = 0,
    RED
}COLOR;

/**
 * struct for Red Black tree node
 */
typedef struct __rbtNode{
    int key;
    int movie;
    int time;
    int date;
    int seat;
    COLOR color;
    struct __rbtNode *left;
    struct __rbtNode *right;
    struct __rbtNode *p;
}RBT_NODE;

/**
 * struct for Red Black Tree
 */
typedef struct __rbt{
    RBT_NODE *root;
    RBT_NODE *NIL;
    int num_node;
}RBT;

/**
 * function prototypes
 */
// Sub module
int make_reserve_id(RBT *T);
void seat_array(int **movie, int seat, int reserve);
void random_seat(RBT *T, int ***movie, int *show_time);
void create_list(int *A, int size, int range);
int *start_time(int *show_time);
// UI
void get_user_input(int ***movie, RBT *T, int *show_time);
int seat_select(int **movie);
void print_info(int *show_time);
// Data Structure, primitive function
int ***movie_array(void);
void free_movie(int ***movie);
RBT Tree_init(void);
void Tree_free(RBT_NODE *node);
void RB_INSERT(RBT *T, int key, int movie, int date, int time, int seat);
void RB_INSERT_FIXUP(RBT *T, RBT_NODE *newnode);
void LEFT_ROTATE(RBT *T, RBT_NODE *x);
void RIGHT_ROTATE(RBT *T, RBT_NODE *x);
RBT_NODE *rb_search(RBT *T, int z);
void RB_TRANSPLANT(RBT *T, RBT_NODE *u, RBT_NODE *v);
void RB_DELETE(RBT *T, int key);
void RB_DELETE_FIXUP(RBT *T, RBT_NODE *x);
RBT_NODE *T_MIN(RBT *T, RBT_NODE *x);
void Tree_print(RBT_NODE *node, RBT *T);
void PRINT_BST(RBT_NODE *node, RBT *T);
char color(RBT_NODE *node);
int Tree_height(RBT *T, RBT_NODE *cur);
int height(RBT *T, RBT_NODE *node);


int main(){
    int ***movie = movie_array();
    RBT T = Tree_init();
    int *show_time;
    show_time = start_time(show_time);
    printf("start time done\n");
    random_seat(&T, movie, show_time);
    get_user_input(movie, &T, show_time);



    free(show_time);
    Tree_free(T.root);
    free_movie(movie);
    return 0;
}



/*******************************sub module**********************************/
int make_reserve_id(RBT *T){
    int id = rand() % ID_MAX;
    while(rb_search(T, id) != NULL){
        id = rand() %ID_MAX;
    }
    return id;
}

/**
 * input
 *    seat: 0 ~ 99
 */
void seat_array(int **movie, int seat, int reserve){
    int row = seat / SEAT;  // 0 ~ 9
    int column = seat % SEAT; // 0 ~ 9

    if(reserve == 0){
        movie[row][column] = 0;
    }else{
        movie[row][column] = 1;
    }
}

void random_seat(RBT *T, int ***movie, int *show_time){
    int no_key = RAND_INPUT;
    int seats[no_key];
    //  create 30 of random keys (0 ~ 99)
    create_list(seats, no_key, SEAT * SEAT);
//    printf("create_list donein rand\n");
    int id, date, movie_no, time, seat;


    for(int i = 0; i < DATE * MOVIE * TIME; i++){
        for(int j = 0; j < no_key; j++){
            id = make_reserve_id(T);
            date = (int)i / (MOVIE * TIME) + 1;
            movie_no = i % (MOVIE * TIME) + 1;
//            printf("date: %d movie: %d\n", date, movie_no);
//            printf("%d ", show_time[i]);

            time = show_time[i];

//            printf("before\n");
            seat = seats[j];
//            printf("seat %d\n", seat);

            RB_INSERT(T, id, movie_no, date, time, seat);
//            printf("insert\n");
            seat_array(movie[i], seat, 1);
        }
    }
}


/**
 * create random number to fill in the array size (size)
 * if same value is in the array, pick the number again.
 * range: 0 ~ range - 1
 */
void create_list(int *A, int size, int range){
    for(int i = 0; i < size; i++){
        A[i] = rand() % range;
        for(int j = 0; j < i; j++){
            if(A[j] == A[i])
                i--;
        }
    }
}

int *start_time(int *show_time){
    show_time = (int *)malloc(sizeof(int) * DATE * MOVIE * TIME);
    if(show_time == NULL){
        printf("dynamic allocation failed\n");
        exit(-1);
    }
    // create random start time (range: 0 ~ 23)
    for(int i = 0; i < DATE * MOVIE * TIME; i++){
        show_time[i] = rand() % 24;
//        printf("%d ", show_time[i]);
    }
    return show_time;
}

/**********************************UI****************************************/
void get_user_input(int ***movie, RBT *T, int *show_time){
    int mode;
    int id;
    int movie_no;
    int seat, date, time;

    do{
        printf("Press 1(for booking) 2(for canceling) 0(for termination)\n");
        scanf("%d", &mode);
        if(mode == 1){
            printf("Choose date(1 ~ 3): ");
            scanf("%d", &date);
            while((date < 1) || (date > DATE)){
                printf("improper input (Choose 1 ~ 3)\n");
                printf("Choose date: ");
                scanf("%d", &date);
            }
            printf("Choose movie and time\n");
            //printf("1\t2\t3\n4\t5\t6\n7\t8\t9\n");
            print_info(show_time);
            printf("Choose the movie(1 ~ 9): ");
            scanf("%d", &movie_no);
            while((movie_no < 1) || (movie_no > MOVIE * TIME)){
                printf("improper input. (choose 1 ~ %d)\n", MOVIE * TIME);
                printf("Choose the movie(1 ~ %d): ", MOVIE * TIME);
                scanf("%d", &movie_no);
            }
            seat = seat_select(movie[movie_no]);
            seat_array(movie[movie_no], seat, 1);
            id = make_reserve_id(T);
            time = show_time[movie_no * date - 1];
            movie_no = movie_no % MOVIE + 1;

            RB_INSERT(T, id, movie_no, date, time, seat);
            printf("Height: %d Node no.: %d\n", Tree_height(T, T -> root), T -> num_node);
            printf("leaf: ");
            Tree_print(T -> root, T);
            printf("\n");


            printf("Reservation Completed\n");
            printf("Reservation ID: %d\n", id);
            printf("Date: %d Movie: %d\nTime %d:00 Seat %d\n", date, movie_no, time, seat);

        }else if(mode == 2){
            printf("Enter reservation ID: ");
            scanf("%d\n", &id);
            RBT_NODE *del = rb_search(T, id);

            // rb search -> null -> error
            while(del == NULL){
                printf("Reservation ID not found Try again\n");
                printf("Enter reservation ID: ");
                scanf("%d\n", &id);
                del = rb_search(T, id);
            }

            //           -> delete complete
            seat_array(movie[del ->movie], del -> seat, 0);
            RB_DELETE(T, id);
            printf("Height: %d Node no.: %d\n", Tree_height(T, T -> root), T -> num_node);
            printf("leaf: ");
            Tree_print(T -> root, T);
            printf("\n");

        }else if(mode == 0){
            break;
        }else{
            printf("improper input.\n");
        }
    }while(mode != 0);


}


int seat_select(int **movie){
    printf("seat no. (can reserve) X (already reserved)\n");
    for(int i = 0; i < SEAT; i++){
        for(int j = 0; j < SEAT; j++){
            if(movie[i][j] == 0){
                printf("%d\t", (i * SEAT + j));
            }else{
                printf("X\t");
            }
        }
        printf("\n");
    }
    int seat, row, column;
    printf("Choose seat no.: ");
    scanf("%d", &seat);
    while((seat < 0) || (seat > SEAT * SEAT)){
        printf("improper input. (choose 0 ~ %d)\n", SEAT * SEAT - 1);
        printf("Choose seat no.: ");
        scanf("%d", &seat);
    }
        row = seat / SEAT;
    column = seat % SEAT;

    while(movie[row][column] == 1){
        printf("The seat is already reserved.\n");
        printf("Choose seat no.: ");
        scanf("%d", &seat);
        while((seat < 0) || (seat > SEAT)){
            printf("improper input. (choose 0 ~ %d)\n", SEAT * SEAT - 1);
            printf("Choose seat no.: ");
            scanf("%d", &seat);
        }

        row = seat / SEAT;
        column = seat & SEAT;
    }

    return seat;
}

void print_info(int *show_time){
    // print the information
    printf("\t\tMovie\t\t\n");
    printf("\t\tA\t\tB\t\tC\n");
    printf("time\n");
    printf("\t 1. %d:00\t 4. %d:00\t 7. %d:00\n", show_time[0], show_time[3], show_time[6]);
    printf("\t 2. %d:00\t 5. %d:00\t 8. %d:00\n", show_time[1], show_time[4], show_time[7]);
    printf("\t 3. %d:00\t 6. %d:00\t 9. %d:00\n", show_time[2], show_time[5], show_time[8]);
}

/***************data structure/primitive function***************************/

/**
 * dynamic memory allocation and initialization to 0
 * movie[i]: pointer to movie[j][k]
 * movie[j][k]: 10 * 10 array for seat
 */
int ***movie_array(void){
    int ***movie = (int ***)malloc(sizeof(int **) * 27);
    if(movie == NULL){
        printf("dynamic memory allocation failed\n");
        exit(-1);
    }
    for(int i = 0; i < 27; i++){
        movie[i] = (int **)malloc(sizeof(int *) * 10);
        if(movie[i] == NULL){
            printf("dynamic memory allocation failed\n");
            exit(-1);
        }
        for(int j = 0; j < 10; j++){
            movie[i][j] = (int *)malloc(sizeof(int)* 10);
            if(movie[i][j] == NULL){
                printf("dynamic memory allocation failed\n");
                exit(-1);
            }
        }
    }

    //  initialization to 0
    for(int i = 0; i < 27; i++){
        for(int j = 0; j < 10; j++){
            for(int k = 0; k < 10; k++){
                movie[i][j][k] = 0;
            }
        }
    }
    return movie;
}

void free_movie(int ***movie){
    for(int i = 0 ; i < 27; i++){
        for(int j = 0; j < 10; j++){
            free(movie[i][j]);
        }
        free(movie[i]);
    }
}

RBT Tree_init(void){
    RBT T;
    T.num_node = 0;
    // set nil of T
    RBT_NODE *nil = (RBT_NODE *)malloc(sizeof(RBT_NODE));
    nil -> color = BLACK;
    nil -> key = 0;
    nil -> date = 0;
    nil -> time = 0;
    nil -> seat = 0;
    nil -> movie = 0;
    nil -> left = NULL;
    nil -> right = NULL;
    nil -> p = NULL;
    T.NIL = nil;
    T.root = T.NIL;

    return T;
}




/**
 * free dynamic memory
 */
void Tree_free(RBT_NODE *node){
    if(node != NULL){
        Tree_free(node -> right);
        Tree_free(node -> left);
    }
}



/**
 * red black tree insert
 * insert the node as done in binary search tree
 * set the color of node red
 * call RB_INSERT_FIXUP to maintain the rule of RBT
 */
void RB_INSERT(RBT *T, int key, int movie, int date, int time, int seat){
    // if there is no node that has the key value(no duplicating key in the tree)
    if(rb_search(T, key) == NULL){
        T -> num_node += 1;
        RBT_NODE *y, *x;
        y = T -> NIL;
        x = T -> root;

        // make new node
        RBT_NODE *newnode = (RBT_NODE *)malloc(sizeof(RBT_NODE));
        newnode -> key = key;
        newnode -> date = date;
        newnode -> movie = movie;
        newnode -> time = time;
        newnode -> seat = seat;
        newnode -> color = RED;
        newnode -> left = T -> NIL;
        newnode -> right = T -> NIL;

        // not the first insertion
        while(x != T -> NIL){
            y = x;
            if(key < x -> key){
                x = x -> left;
            }else{
                x = x -> right;
            }
        }
        //  y: parent of newnode
        newnode -> p = y;
        //  if the node is root
        if(y == T -> NIL){
            T -> root = newnode;
        //  set the position of newnode
        }else if(key < y -> key){
            y -> left = newnode;
        }else{
            y -> right = newnode;
        }

        //  to maintain the rule of RBT
        RB_INSERT_FIXUP(T, newnode);
    }else{
        printf("already exists\n");
    }
}

/**
 * to maintain the rule of RBT
 */
void RB_INSERT_FIXUP(RBT *T, RBT_NODE *newnode){
    RBT_NODE *y;
    //  if the parent of newnode is black, no problem
    //  if red, two red node can be in series -> problem
    while(newnode -> p -> color == RED){
        if(newnode -> p == newnode -> p -> p -> left){
            y = newnode -> p -> p -> right;
            //  case 1
            if(y -> color == RED){
                newnode -> p -> color = BLACK;
                y -> color = BLACK;
                newnode -> p -> p -> color = RED;
                newnode = newnode -> p -> p;
            }else{
                //  case 2
                if(newnode == newnode -> p -> right){
                    newnode = newnode -> p;
                    LEFT_ROTATE(T, newnode);
                }
                //  case 3
                newnode -> p -> color  = BLACK;
                newnode -> p -> p -> color = RED;
                RIGHT_ROTATE(T, newnode -> p -> p);
            }
        }else{
            y = newnode -> p -> p -> left;
            //  case 4
            if(y -> color == RED){
                newnode -> p -> color = BLACK;
                y -> color = BLACK;
                newnode -> p -> p -> color = RED;
                newnode = newnode -> p -> p;
            }else{
                //  case 5
                if(newnode == newnode -> p -> left){
                    newnode = newnode -> p;
                    RIGHT_ROTATE(T, newnode);
                }
                //  case 6
                newnode -> p -> color  = BLACK;
                newnode -> p -> p -> color = RED;
                LEFT_ROTATE(T, newnode -> p -> p);
            }
        }
    }
    //  set the color of root black
    T -> root -> color = BLACK;
}

/**
 * left rotate in RBT
 */
void LEFT_ROTATE(RBT *T, RBT_NODE *x){
    RBT_NODE *y = x -> right;
    x -> right = y -> left;  //  turn y's left subtree to x's right subtree
    if(y -> left != T -> NIL){
        y -> left -> p = x;
    }
    y -> p = x -> p;
    //  if x was root
    if(x -> p == T -> NIL){
        T -> root = y;
    }else if(x == x -> p -> left){
        x -> p -> left = y;
    }else{
        x -> p -> right = y;
    }
    y -> left = x;           //  put x in left of y
    x -> p = y;
}

/**
 * right rotation of RBT
 */
void RIGHT_ROTATE(RBT *T, RBT_NODE *x){
    RBT_NODE *y = x -> left;
    x -> left = y -> right;  //  turn y's right subtree to x's left subtree
    if(y -> right != T ->NIL){
        y -> right -> p = x;
    }
    y -> p = x -> p;
    // if x was root
    if(x -> p == T -> NIL){
        T -> root = y;
    }else if(x == x -> p -> left){
        x -> p -> left = y;
    }else{
        x -> p -> right = y;
    }
    y -> right = x;          //  put x in right of y
    x -> p = y;
}


/**
 * if there is the key in T, return the node address
 * else return NULL
 */
RBT_NODE *rb_search(RBT *T, int z){
    RBT_NODE *cur = T -> root;

    while(cur != T -> NIL){
        if(cur -> key == z){
            return cur;
        }
        if(z < cur -> key){
            cur = cur -> left;
        }else{
            cur = cur -> right;
        }
    }
    return NULL;
}

/**
 * swap u and v and remove u
 */
void RB_TRANSPLANT(RBT *T, RBT_NODE *u, RBT_NODE *v){
    //  if u is the root
    if(u -> p == T -> NIL){
        T -> root = v;
    }else if(u == u -> p -> left){
        u -> p -> left = v;
    }else{
        u -> p -> right = v;
    }
    v -> p = u -> p;
}

/**
 * delete the node that have key value
 * if there is no node in the tree, do noting
 * if the node exists, delete the node
 * fix up the tree to maintain RBT rule
 */
void RB_DELETE(RBT *T, int key){
    RBT_NODE *z = rb_search(T, key);
    //  check if there is such node
    if(z != NULL){
        T -> num_node -= 1;
        RBT_NODE *y = z;
        RBT_NODE *x;
        COLOR y_color = y -> color;  //  original color of y
        //  no left child
        if(z -> left == T -> NIL){
            x = z -> right;
            RB_TRANSPLANT(T, z, z -> right);
        //  only left child
        }else if(z -> right == T -> NIL){
            x = z -> left;
            RB_TRANSPLANT(T, z, z -> left);
        //  two child
        }else{
            //  find successor
            y = T_MIN(T, z -> right);
            y_color = y -> color;
            x = y -> right;
            if(y -> p == z){
                x -> p = y;
            }else{
                RB_TRANSPLANT(T, y, y -> right);
                y -> right = z -> right;
                y -> right -> p = y;
            }
            RB_TRANSPLANT(T, z, y);
            y -> left = z -> left;
            y -> left -> p = y;
            y -> color = z -> color;
        }
        if(y_color == BLACK){
            RB_DELETE_FIXUP(T, x);
        }
    }else{
        printf("no such node\n");
    }
}

void RB_DELETE_FIXUP(RBT *T, RBT_NODE *x){
    RBT_NODE *w;
    while(x != T -> root && x -> color == BLACK){
        if(x == x -> p -> left){
            w = x -> p -> right;
            //  case 1
            if(w -> color == RED){
                w -> color = BLACK;
                x -> p -> color = RED;
                LEFT_ROTATE(T, x -> p);
                w = x -> p -> right;
            }
            //  case 2
            if(w -> left -> color == BLACK && w -> right -> color == BLACK){
                w -> color = RED;
                x = x -> p;
            }else{
                //  case 3
                if(w -> right -> color == BLACK){
                w -> left -> color = BLACK;
                w -> color = RED;
                RIGHT_ROTATE(T, w);
                w = x -> p -> right;
                }
                //  case 4
                w -> color = x -> p -> color;
                x-> p -> color = BLACK;
                w -> right -> color = BLACK;
                LEFT_ROTATE(T, x -> p);
                x = T -> root;
            }
        }else{
            w = x -> p -> left;
            //  case 5
            if(w -> color == RED){
                w -> color = BLACK;
                x -> p -> color = RED;
                RIGHT_ROTATE(T, x -> p);
                w = x -> p -> left;
            }
            //  case 6
            if(w -> right -> color == BLACK && w -> left -> color == BLACK){
                w -> color = RED;
                x = x -> p;
            }else{
                //  case 7
                if(w -> left -> color == BLACK){
                w -> right -> color = BLACK;
                w -> color = RED;
                LEFT_ROTATE(T, w);
                w = x -> p -> left;
                }
                //  case 8
                w -> color = x -> p -> color;
                x->p -> color = BLACK;
                w -> left -> color = BLACK;
                RIGHT_ROTATE(T, x -> p);
                x = T -> root;
            }
        }
    }
    x -> color = BLACK;
}

/**
 * find and return the node that has minimum key value
 */
RBT_NODE *T_MIN(RBT *T, RBT_NODE *x){
    while(x -> left != T -> NIL){
        x = x -> left;
    }
    return x;
}

/**
 * print the leaf value of the RBT
 * order: left to right
 *
 * b: black, r: red
 */
void Tree_print(RBT_NODE *node, RBT *T){
    if(node != T -> NIL){

        if((node -> left == T -> NIL) &&(node -> right == T -> NIL)){
            printf("%d (%c)\n", node -> key, color(node));
        }
        Tree_print(node -> left, T);
        Tree_print(node -> right, T);
    }
}

/**
 * print the key value of Red Black Tree
 *
 * parent (color)
 * ----|left child (color)
 * ----|right child (color)
 * if there is no child, do not print
 *
 * b: black, r: red
 */
void PRINT_BST(RBT_NODE *node, RBT *T){
    if(node != T -> NIL){
        for(int i = 0; i < height(T, node); i++){
            printf("----|");
        }
        printf("%d (%c)\n", node -> key, color(node));
        PRINT_BST(node -> left, T);
        PRINT_BST(node -> right, T);
    }
}

/**
 * return char (color of the node)
 */
char color(RBT_NODE *node){
    if(node -> color == RED){
        return 'r';
    }else{
        return 'b';
    }
}

/**
 * height(depth) of the given tree
 * return the height
 * height of root = 0
 */
int Tree_height(RBT *T, RBT_NODE *cur){
    static int h_left = 0;
    static int h_right = 0;

    if(cur != T -> NIL){
        if(cur -> left != T -> NIL){
            h_left++;
        }
        if(cur -> right != T -> NIL){
            h_right++;
        }
        Tree_height(T, cur -> left);
        Tree_height(T, cur -> right);
    }

    if(h_left > h_right){
        return h_left;
    }else{
        return h_right;
    }
}

/**
 * height(depth) of the given node
 * return the height
 * height of root = 0
 */
int height(RBT *T, RBT_NODE *node){
    int h = 0;
    int key = node -> key;
    RBT_NODE *cur = T -> root;
    if(node == T -> root){
        h = 0;
    }else{
        while(cur != T -> NIL){
            if(key < cur -> key){
                cur = cur -> left;
                h++;
            }else if(key > cur -> key){
                cur = cur -> right;
                h++;
            }else{
                break;
            }
        }
    }
    return h;
}
