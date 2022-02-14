/**
 * Copyright (C) 2020 by Dongwon Kim, All rights reserved.
 *
 * Assignment 3 of Algorithms (booking movie ticket)
 *
 * File name : report3_2018312292_DongwonKim.c
 *
 * Summery of this code:
 *    UI: get mode (1: reservation 2: cancel 3: terminate)
 *        mode 1: get date, movie and time from user
 *                print reservation data and reservation id
 *        mode 2: get reservation id and delete the reservation
 *                print canceled data
 *        mode 3: terminate the program
 *
 *    Data Structure:
 *        Red Black Tree(RBT) T: contains all reservation information
 *            RB_INSERT, RB_DELETE, Tree_height, key value of leaves, no. of node
 *        3D int array movie: save if the seat is reserved or not
 *            modified by seat_array, printed by seat_select
 *
 * Written by Dongwon Kim on November 28, 2020
 *            Student ID number : 2018312292
 *            School of Electrical Engineering
 *            Sungkyunkwan University
 *
 *  Modification History :
 *    1. Written by Dongwon Kim on Nov 28, 2020
 *
 *
 *  Compiler Used : gcc version 7.3.0, 32-bit
 *  Encoding : UTF - 8
 *  IDE used :Code Block 17.12
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ID_MAX 10000   //  range of reservation ID
#define RAND_INPUT 30  //  no. of random reservation

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
void random_seat(RBT *T, int ***movie);
void create_list(int *A, int size, int range);
int *start_time(int *show_time);
char movie_name(int movie_no);
// UI
void get_user_input(int ***movie, RBT *T, int *show_time);
int seat_select(int **movie);
void print_info(int *show_time, int date);
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
char color(RBT_NODE *node);
int Tree_height(RBT *T, RBT_NODE *cur);
int findMax(int a, int b);

/**
 * main function of the program
 * call srand to make random number
 * create and initialize 3D int array movie
 * create and initialize RBT T
 * create and fill int array show_time
 * start UI
 * free dynamic memory
 *
 * output:
 *    return 0 (success), -1(dynamic memory allocation failed)
 */
int main(){
    srand(time(NULL));
    //  create 3D array of seat reservation data
    int ***movie = movie_array();
    //  initialize RBT T
    RBT T = Tree_init();
    //  get random start time
    int *show_time;
    show_time = start_time(show_time);
    //  randomly select RAND_INPUT of seat per movie
    random_seat(&T, movie);

    // UI start
    get_user_input(movie, &T, show_time);

    //  free dynamic memory
    free(show_time);
    Tree_free(T.root);
    free_movie(movie);

    return 0;
}

/*******************************sub module**********************************/
/**
 * get unique reservation id (range: 0 ~ ID_MAX - 1)
 * canceled id can be used again
 *
 * input:
 *    RBT *T: RBT that contains all reservation information
 *
 * output:
 *    return int id
 */
int make_reserve_id(RBT *T){
    int id = rand() % ID_MAX;
    //  get random id that is not in the Tree
    while(rb_search(T, id) != NULL){
        id = rand() %ID_MAX;
    }
    return id;
}

/**
 * change no. of movie(int) to movie name(A, B, C)
 *
 * input:
 *    int movie_no: no. of movie (range 1 ~ 3)
 *
 * output:
 *    return char 'A'(1), 'B'(2), 'C'(3)
 */
char movie_name(int movie_no){
    if(movie_no == 1){
        return 'A';
    }else if(movie_no == 2){
        return 'B';
    }else{
        return 'C';
    }
}
/**
 * mark 1 for reserved seat, 0 for available seat to 2D array movie
 *
 * input
 *    int seat: seat number (range: 0 ~ 99)
 *    int reserve: reserve(when 1), cancel(when 0)
 *
 * in/output
 *    int **movie: pointer array of 2D int array for seat
 *           mark 0 when seat is not reserved, 1 when reserved
 */
void seat_array(int **movie, int seat, int reserve){
    int row = seat / 10;  // 0 ~ 9
    int column = seat % 10; // 0 ~ 9

    if(reserve == 0){
        movie[row][column] = 0;
    }else{
        movie[row][column] = 1;
    }
}

/**
 * randomly select RAND_INPUT of seat
 *
 * input/output
 *    RBT *T: RBT that contains all reservation information
 *    int ***movie: array of 2D pointer array for the seat(modified by seat_array())
 *                  hierarchy: date(movie(time)))
 */
void random_seat(RBT *T, int ***movie){
    int no_key = RAND_INPUT;  // no. of random input
    int seats[no_key];
    //  create 30 of random keys (0 ~ 99)
    create_list(seats, no_key, 100);
    int id, date, movie_no, time_no, seat;

    //  for 3 days, 9 movies are played per day
    for(int i = 0; i < 27; i++){
        for(int j = 0; j < no_key; j++){
            //  get random reservation id
            id = make_reserve_id(T);
            date = (int)i / 9 + 1;
            movie_no = (i / 3) % 3 + 1;
            time_no = i % 9 + 1;
            seat = seats[j];

            //  insert new node to the tree
            RB_INSERT(T, id, movie_no, date, time_no, seat);
            //  modify array that has reservation information
            seat_array(movie[i], seat, 1);
        }
        //  random seat for next movie
        create_list(seats, no_key, 100);
    }
}


/**
 * create random number to fill in the array size (size)
 * if same value is in the array, pick the number again.
 * range: 0 ~ range - 1
 *
 * input:
 *    int size: size of the array
 *    int range: range of random values
 *
 * in/output:
 *    int *A: integer array that contains random values (size: size)
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

/**
 * randomly choose start time of movies
 * the show time of a movie in the same time don't collapse
 *
 * input/output
 *    show_time: 1D dynamic allocated array to save random start time
 */
int *start_time(int *show_time){
    show_time = (int *)malloc(sizeof(int) * 27);
    if(show_time == NULL){
        printf("dynamic allocation failed\n");
        exit(-1);
    }

    int rand_array[3];
    // create random start time (range: 0 ~ 23) for a movie per day
    create_list(rand_array, 3, 24);
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 3; j++){
            show_time[i * 3 + j] = rand_array[j];
        }
        create_list(rand_array, 3, 24);
    }
    return show_time;
}

/**********************************UI****************************************/
/**
 * get mode and reservation data from user
 * insert/delete the node to the tree, mark seat
 *
 * input:
 *    show_time: 1D dynamic allocated array that have start time information
 *
 * in/output:
 *    movie: 3D array of 2D pointer array for the seat(modified by seat_array())
 *    T: RBT Tree that contains all reservation information
 *    (modified by RB_INSERT(), RB_DELETE())
 */
void get_user_input(int ***movie, RBT *T, int *show_time){
    int mode;
    int id;
    int movie_no, time_no;
    int seat, date, time;

    do{
        printf("Press 1(for booking) 2(for canceling) 0(for termination)\n");
        scanf("%d", &mode);
        /*
         * reservation mode
         *
         * can choose date, movie and time
         * if the input is out of range, get input again
         * insert the information to tree T
         * modify array movie
         * print reservation information
         */
        if(mode == 1){
            printf("Choose date(1 ~ 3): ");
            scanf("%d", &date);
            //  input out of range
            while((date < 1) || (date > 3)){
                printf("improper input (Choose 1 ~ 3)\n");
                printf("Choose date: ");
                scanf("%d", &date);
            }
            printf("Choose movie and time\n");
            print_info(show_time, date);
            printf("Choose the movie(1 ~ 9): ");
            scanf("%d", &movie_no);
            //  input out of range
            while((movie_no < 1) || (movie_no > 9)){
                printf("improper input. (choose 1 ~ 9)\n");
                printf("Choose the movie(1 ~ 9): ");
                scanf("%d", &movie_no);
            }

            //  select seat
            seat = seat_select(movie[movie_no - 1 + (date - 1) * 9]);
            seat_array(movie[movie_no - 1 + (date - 1) * 9], seat, 1);

            //  get reservation id, movie_no, time
            id = make_reserve_id(T);
            time = show_time[(movie_no - 1) + 9 * (date - 1)];
            time_no = movie_no;
            movie_no = (int)(movie_no - 1) / 3 + 1;

            //  insert to tree
            RB_INSERT(T, id, movie_no, date, time_no, seat);
            printf("Height: %d Node no.: %d\n", Tree_height(T, T -> root), T -> num_node);
            printf("Root: %d\n", T -> root -> key);
            printf("leaf: ");
            Tree_print(T -> root, T);
            printf("\n");

            //  print reservation data
            printf("Reservation Completed\n");
            printf("Reservation ID: %d\n", id);
            printf("Date: %d Movie: %c\nTime %d:00 Seat %d\n", date, movie_name(movie_no), time, seat);

        /*
         * cancel
         *
         * get reservation id
         * if reservation id is not in the tree, get the input again
         * delete the node from the tree T
         * print reservation information
         */
        }else if(mode == 2){
            printf("Enter reservation ID: ");
            scanf("%d", &id);
            RBT_NODE *del = rb_search(T, id);

            //  if the id is not in the tree
            while(del == NULL){
                printf("Reservation ID not found Try again\n");
                printf("Enter reservation ID: ");
                scanf("%d", &id);
                del = rb_search(T, id);
            }
            time_no = del -> time;
            time = show_time[time_no - 1 + 9 * (del -> date - 1)];

            //  print reservation data
            printf("Cancel Completed\n");
            printf("Reservation ID: %d", id);
            printf("Date: %d Movie: %c\n", del -> date, movie_name(del -> movie));
            printf("Time %d:00 Seat %d\n", time, del -> seat);

            //  delete from T, modify array movie
            seat_array(movie[time_no - 1 + 9 * (del -> date - 1)], del -> seat, 0);
            RB_DELETE(T, id);
            printf("Height: %d Node no.: %d\n", Tree_height(T, T -> root), T -> num_node);
            printf("Root: %d\n", T -> root -> key);
            printf("leaf: ");
            Tree_print(T -> root, T);
            printf("\n");

        /*
         * termination
         */
        }else if(mode == 0){
            break;
        /*
         * if input is not 0 or 1 or 2, get 'mode' again
         */
        }else{
            printf("improper input.\n");
        }
    }while(mode != 0);
}

/**
 * let user select seat(range: 0 ~ 99) by keyboard
 * print the seat (if reserved, print X else, print seat no.)
 * if the input from user is inappropriate, get the input again
 *
 * input:
 *    movie: 2D array of movie_no. that has seat reservation data
 *
 * output:
 *    return int seat: proper user's keyboard input
 */
int seat_select(int **movie){
    //  print the reservation data of seat
    int reserved = 0;
    printf("seat no. (can reserve) X (already reserved)\n");
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            if(movie[i][j] == 0){
                printf("%d\t", (i * 10 + j));
            }else{
                printf("X\t");
                reserved += 1;
            }
        }
        printf("\n");
    }
    //  print the no. of reserved seat
    printf("reserved seat: %d/100 available: %d/100\n", reserved, 100 - reserved);

    //  get seat no. from user(keyboard input)
    int seat, row, column;
    printf("Choose seat no.: ");
    scanf("%d", &seat);

    //  if the input is out of range, get the input again
    while((seat < 0) || (seat > 99)){
        printf("improper input. (choose 0 ~ 99)\n");
        printf("Choose seat no.: ");
        scanf("%d", &seat);
    }
    row = seat / 10;
    column = seat % 10;

    //  if the seat is already reserved, get the input again
    while(movie[row][column] == 1){
        printf("The seat is already reserved.\n");
        printf("Choose seat no.: ");
        scanf("%d", &seat);
        //  if the input is out of range, get the input again
        while((seat < 0) || (seat > 99)){
            printf("improper input. (choose 0 ~ 99)\n");
            printf("Choose seat no.: ");
            scanf("%d", &seat);
        }
        row = seat / 10;
        column = seat % 10;
    }

    return seat;
}

/**
 * print the show time and movie name of the date
 *
 * input:
 *    show_time: int array that save the start time of all movies
 *    date: the date that user selected
 */
void print_info(int *show_time, int date){
    // print the information
    printf("\t\tMovie\t\t\n");
    printf("\t\tA\t\tB\t\tC\n");
    printf("time\n");
    printf("\t 1. %d:00\t 4. %d:00\t 7. %d:00\n", show_time[0 + 9* (date - 1)], show_time[3 + 9* (date - 1)], show_time[6+ 9* (date - 1)]);
    printf("\t 2. %d:00\t 5. %d:00\t 8. %d:00\n", show_time[1+ 9* (date - 1)], show_time[4+ 9* (date - 1)], show_time[7+ 9* (date - 1)]);
    printf("\t 3. %d:00\t 6. %d:00\t 9. %d:00\n", show_time[2+ 9* (date - 1)], show_time[5+ 9* (date - 1)], show_time[8+ 9* (date - 1)]);
}

/***************data structure/primitive function***************************/

/**
 * dynamic memory allocation and initialization to 0
 * movie[i]: pointer to movie[j][k]
 * movie[j][k]: 10 * 10 array for seat
 *
 * output:
 *    return int ***movie: pointer array that points 10*10 2D array
 */
int ***movie_array(void){
    //  dynamic memory allocation
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

    //  initialization to 0 (b/c at first, no reservation)
    for(int i = 0; i < 27; i++){
        for(int j = 0; j < 10; j++){
            for(int k = 0; k < 10; k++){
                movie[i][j][k] = 0;
            }
        }
    }
    return movie;
}

/**
 * free dynamic memory of int ***movie
 *
 * input:
 *    movie: pointer array that points 10*10 2D array
 */
void free_movie(int ***movie){
    for(int i = 0 ; i < 27; i++){
        for(int j = 0; j < 10; j++){
            free(movie[i][j]);
        }
        free(movie[i]);
    }
    free(movie);
}

/**
 * initialize Red Black Tree
 * data value of nil is all zero.
 * color of nil is black
 * left, right, parent of nil is NULL at the first time.
 *
 * output:
 *   return RBT T
 */
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

    //  at first, root == NIL
    T.root = T.NIL;

    return T;
}

/**
 * free dynamic memory of RBT
 */
void Tree_free(RBT_NODE *node){
    if(node != NULL){
        Tree_free(node -> right);
        Tree_free(node -> left);
        free(node);
    }
}


/**
 * red black tree insert
 * insert the node as done in binary search tree
 * set the color of node red
 * call RB_INSERT_FIXUP to maintain the rule of RBT
 *
 * input:
 *    int key: reservation id
 *    int movie: movie number (range: 1 ~ 3)
 *    int date: the date that movie shows (range: 1 ~ 3)
 *    int time: the movie no that user choose (range: 1 ~ 9)
 *    int seat: seat number (range: 0 ~ 99)
 *
 * in/output:
 *    RBT *T: Red Black Tree(RBT) which the node that have key value 'key' is inserted
 */
void RB_INSERT(RBT *T, int key, int movie, int date, int time, int seat){
    // if there is no node that has the key value(no duplicating key in the tree)
    if(rb_search(T, key) == NULL){
        T -> num_node += 1;  //  increase no. of node in the Tree
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
 *
 * input:
 *    RBT_NODE *newnode: new node to be inserted in the tree
 *
 * in/output:
 *    RBT *T: RBT that have to be fixed
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
 *
 * input:
 *    RBT_NODE *x: the node be left rotated
 *
 * in/output:
 *    RBT *T: RBT that contains all reservation information
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
 *
 * input:
 *    RBT_NODE *x: node be right rotated
 *
 * in/output:
 *    RBT *T: RBT that contains all reservation information
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
 *
 * input:
 *    int z: key value to search
 *    RBT *T: RBT that contains all reservation information
 *
 * output:
 *    return RBT_NODE *cur: the node address when the node exists
 *    return  NULL: when the node doesn't exists
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
 *
 * input:
 *    RBT_NODE *u: node1 to be swapped
 *    RBT_NODE *v: node2 to be swapped
 *
 * in/output:
 *    RBT *T: RBT that contains all reservation information
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
 *
 * input:
 *    int key: key value of the node to be deleted
 *
 * in/output:
 *    RBT *T: RBT that contains all reservation information
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

/**
 * to maintain RBT rules
 *
 * input:
 *    RBT_NODE *x: address of the node to be fixed
 *
 * in/output:
 *    RBT *T: RBT that contains all reservation information
 */
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
 *
 * input:
 *    RBT_NODE *x: address of the node which is the start point of the search
 *    RBT *T: RBT that contains all reservation information
 *
 * output:
 *    return RBT_NODE *x: the address of node that has minimum key value
 */
RBT_NODE *T_MIN(RBT *T, RBT_NODE *x){
    while(x -> left != T -> NIL){
        x = x -> left;
    }
    return x;
}

/**
 * print the leaf value of the RBT
 * order: left to right / b: black, r: red
 *
 * input:
 *    RBT_NODE *node: the address of node to start printing
 *    RBT *T: RBT that contains all reservation information
 *
 * output:
 *    print the key value of leaves
 */
void Tree_print(RBT_NODE *node, RBT *T){
    if(node != T -> NIL){

        if((node -> left == T -> NIL) &&(node -> right == T -> NIL)){
            printf("%d (%c)", node -> key, color(node));
        }
        Tree_print(node -> left, T);
        Tree_print(node -> right, T);
    }
}

/**
 * return char (color of the node)
 *
 * input:
 *    RBT_NODE *node: address of node whose color be interpreted
 *
 * output:
 *    return char 'r'(red) or 'b'(black)
 */
char color(RBT_NODE *node){
    if(node -> color == RED){
        return 'r';
    }else{
        return 'b';
    }
}

/**
 * find height of the given tree(height of root = 0)
 *
 * input:
 *    RBT_NODE *cur: address of the node which is the start point
 *    RBT *T: RBT that contains all reservation information
 *
 * output:
 *     return (int)height
 */
int Tree_height(RBT *T, RBT_NODE *cur){
    //  when cur is root of the subtree, return 1
    if(cur == T -> NIL){
        return 0;
    }

    //  if cur node has child
    return findMax(Tree_height(T, cur -> left), Tree_height(T, cur -> right)) + 1;
}

/**
 * return bigger value
 *
 * input:
 *    int a: integer1 to be compared
 *    int b: integer2 to be compared
 *
 * output:
 *    return (int)bigger integer between integer1 and integer2
 */
int findMax(int a, int b){
    if(a >= b){
        return a;
    }else{
        return b;
    }
}
