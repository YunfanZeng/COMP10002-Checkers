/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: [Enter your full name and student number here before submission]
  Dated:     [Enter the date that you "signed" the declaration]

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions

/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type

typedef struct {
	int x;
	int y;
} cell_t;

typedef struct {
	cell_t origin;
	cell_t target;
	char bw;
} move_t;

int possible_moves(board_t, char, move_t*, move_t*);
void

/* populates a given board into its starting state */
init_board(board_t board) {

	/* hard-coded positions of the initial pieces */
	int wpos[12][2] = {{0,1}, {0,3}, {0,5}, {0,7}, {1,0}, {1,2}, {1,4}, {1,6}, {2,1}, {2,3}, {2,5}, {2,7}};
	int bpos[12][2] = {{5,0}, {5,2}, {5,4}, {5,6}, {6,1}, {6,3}, {6,5}, {6,7}, {7,0}, {7,2}, {7,4}, {7,6}};

	printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
	printf("#BLACK PIECES: 12\n#WHITE PIECES: 12\n");

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = '.';
		}
	}
	for (int i = 0; i < 12; i++) {
		board[wpos[i][0]][wpos[i][1]] = CELL_WPIECE;
		board[bpos[i][0]][bpos[i][1]] = CELL_BPIECE;
	}
}


/* prints a given board into a formated state */
void
print_board(board_t board) {
	printf("     A   B   C   D   E   F   G   H\n");
	for (int i = 0; i < BOARD_SIZE; i++) {
		printf("   +---+---+---+---+---+---+---+---+\n");
		printf(" %d |", i + 1);
		for (int j = 0; j < BOARD_SIZE; j++) {
			printf(" %c |", board[i][j]);
		}
		printf("\n");
	}
	printf("   +---+---+---+---+---+---+---+---+\n");
}

/* Calculates the board cost of a given board */
int
board_cost(board_t board) {
	int b, B, w, W;
	b = 0; B = 0; w = 0; W = 0;

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			int cell = board[i][j];
			if (cell != CELL_EMPTY) {
				if (cell == CELL_BPIECE) { b++; }
				else if (cell == CELL_WPIECE) { w++; }
				else if (cell == CELL_BTOWER) { B++; }
				else if (cell == CELL_WTOWER) { W++; }
			}
		}
	}
	return b + (COST_TOWER * B) - w - (COST_TOWER * W);
}

/* compares two move_t structs, where 'o' cmp origin cell, 't' cmp target cell, 'a' cmp both 
 * returns 1 if same and 0 otherwise
 */
int 
cmp_move(move_t move1, move_t move2, char type) {
	int same = 1;
	if (type == 'o' || type == 'a') {
		if (move1.origin.x != move2.origin.x) {
			same = 0;
			return same;
		}
		else if (move1.origin.y != move2.origin.y) {
			same = 0;
			return same;
		}
	} 
	if (type == 't' || type == 'a') {
		if (move1.target.x != move2.target.x) {
			same = 0;
			return same;
		}
		else if (move1.target.y != move2.target.y) {
			same = 0;
			return same;
		}
	}
	return same;
}


/* Called when given a move to check if it is valid*/
int
move_valid(board_t board, move_t* turns, int turn_no, char bw) {
	int origin_x = turns[turn_no].origin.x;
	int origin_y = turns[turn_no].origin.y;
	char origin_cell_piece = board[origin_y][origin_x];
	int target_x = turns[turn_no].target.x;
	int target_y = turns[turn_no].target.y;
	char target_cell_piece = board[target_y][target_y];
	int b_origin_valid = (origin_cell_piece == CELL_BPIECE || origin_cell_piece == CELL_BTOWER);
	int w_origin_valid = (origin_cell_piece == CELL_WPIECE || origin_cell_piece == CELL_WTOWER);
	move_t move_list[4];
	move_t all_moves[96];
	if (origin_x < 0 || origin_x > 7 || origin_y < 0 || origin_y > 7) {
		return 1;
	}
	if (target_x < 0 || target_x > 7 || target_y < 0 || target_y > 7) {
		return 2;
	}
	if (origin_cell_piece == CELL_EMPTY) {
		return 3;
	}
	if (target_cell_piece != CELL_EMPTY) {
		return 4;
	}
	if ((bw = 'b' && w_origin_valid) || (bw = 'w' && b_origin_valid)) {
		return 5;
	} 
	
	int no_of_moves = possible_moves(board, bw, move_list, all_moves);
	
	if (no_of_moves == 0) {
		return 6;
	} else {
		for (int i = 0; i < no_of_moves; i++) {
			if (cmp_move(all_moves[i], turns[turn_no], 'o')) {
				if (cmp_move(all_moves[i], turns[turn_no], 't')) {
					return 0;
				}
			}
		}
		return 6;
	}

	return 0;
}


void
is_tower(board_t board, move_t* turns, int turn_no, char bw) {
	if (bw == 'b') {
		if (turns[turn_no].target.y == 0) {
			board[turns[turn_no].target.y][turns[turn_no].target.x] = CELL_BTOWER;
		}
	}
	else {
		if (turns[turn_no].target.y == 7) {
			board[turns[turn_no].target.y][turns[turn_no].target.x] = CELL_WTOWER;
		}
	}
}


void
update_board(board_t board, char bw, int turn_no, move_t* turns, char type) {
	char* str_bw;
	if (bw == 'b') {
		str_bw = "BLACK";
	}
	else {
		str_bw = "WHITE";
	}
	printf("=====================================\n");
	printf("%s ACTION #%d:", str_bw, turn_no + 1);
	char o_x = turns[turn_no].origin.x + 65;
	char t_x = turns[turn_no].target.x + 65;
	printf(" %c%d-%c%d\n", o_x, turns[turn_no].origin.y + 1, t_x, turns[turn_no].target.y + 1);
	printf("BOARD COST: %d\n", board_cost(board));

	if (type == 'm') {
		board[turns[turn_no].target.y][turns[turn_no].target.x] = board[turns[turn_no].origin.y][turns[turn_no].origin.x];
		board[turns[turn_no].origin.y][turns[turn_no].origin.x] = CELL_EMPTY;

	}

	if (type == 'c') {
		int cap_cell_y;
		int cap_cell_x;
		board[turns[turn_no].target.y][turns[turn_no].target.x] = board[turns[turn_no].origin.y][turns[turn_no].origin.x];
		board[turns[turn_no].origin.y][turns[turn_no].origin.x] = CELL_EMPTY;

		if (turns[turn_no].origin.y > turns[turn_no].target.y) {
			cap_cell_y = turns[turn_no].origin.y - 1;
		}
		else {
			cap_cell_y = turns[turn_no].origin.y + 1;
		}

		if (turns[turn_no].origin.x > turns[turn_no].target.x) {
			cap_cell_x = turns[turn_no].origin.x - 1;
		}
		else {
			cap_cell_x = turns[turn_no].origin.x + 1;
		}

		board[cap_cell_y][cap_cell_x] = CELL_EMPTY;

	}

	if (type == 'f') {
		return 0;
	}
	is_tower(board, turns, turn_no, bw);
}

char turn_type(move_t* turns, int turn_no) {
	if (turns[turn_no].origin.y > turns[turn_no].target.y) {
		if (turns[turn_no].origin.y - turns[turn_no].target.y == 2) {
			return 'c';
		}
		else {
			return 'm';
		}
	}
	else {
		if (turns[turn_no].target.y - turns[turn_no].origin.y == 2) {
			return 'c';
		}
		else {
			return 'm';
		}
	}
}


/* resets the target cell values in the struct move_t */
void
reset_move(move_t* move_list, int num_of_moves) {
	for (int i = 0; i < num_of_moves; i++) {
		move_list[i].target.y = -1;
		move_list[i].target.x = -1;
	}
}

/* sets the origin of move_t to a specific cell */
void
set_origin(move_t* move_list, int num_of_moves, int y, int x) {
	for (int i = 0; i < num_of_moves; i++) {
		move_list[i].origin.y = y;
		move_list[i].origin.x = x;
	}
}
char
switch_colour(char);

int
possible_moves(board_t board, char bw, move_t* move_list, move_t* all_moves) {
	char bw_tower;
	int origin_x, origin_y;
	char tl_cell_piece;
	char tr_cell_piece;
	char bl_cell_piece;
	char br_cell_piece;
	char origin_piece;
	int num_of_moves = 4;
	int move_no = 0;

	if (bw == 'b') {
		bw_tower = 'B';
	}
	else {
		bw_tower = 'W';
	}
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			origin_y = i;
			origin_x = j;
			origin_piece = board[origin_y][origin_x];
			if (board[i][j] == bw || board[i][j] == bw_tower) {
				reset_move(move_list, num_of_moves);
				set_origin(move_list, num_of_moves, origin_y, origin_x);

				if (origin_piece != 'b') {
					if (origin_y + 1 <= 7) {
						if (origin_x + 1 <= 7) {
							br_cell_piece = board[origin_y + 1][origin_x + 1];
							if (br_cell_piece == '.') {
								move_list[1].target.y = origin_y + 1;
								move_list[1].target.x = origin_x + 1;
							}
							else if (br_cell_piece == bw || br_cell_piece == bw_tower) {
							}
							else if (origin_x + 2 <= 7 && origin_y + 2 <= 7)  {
								move_list[1].target.y = origin_y + 2;
								move_list[1].target.x = origin_x + 2;
							}
						}
						if (origin_x - 1 >= 0) {
							bl_cell_piece = board[origin_y + 1][origin_x - 1];
							if (bl_cell_piece == '.') {
								move_list[2].target.y = origin_y + 1;
								move_list[2].target.x = origin_x - 1;
							}
							else if (bl_cell_piece == bw || bl_cell_piece == bw_tower) {
							}
							else {
								move_list[2].target.y = origin_y + 2;
								move_list[2].target.x = origin_x - 2;
							}
						}
					}
				}
				if (origin_piece != 'w') {
					if (origin_y - 1 >= 0) {
						if (origin_x + 1 <= 7) {
							tr_cell_piece = board[origin_y - 1][origin_x + 1];
							if (tr_cell_piece == '.') {
								move_list[0].target.y = origin_y - 1;
								move_list[0].target.x = origin_x + 1;
							}
							else if (tr_cell_piece == bw || tr_cell_piece == bw_tower) {
							}
							else {
								move_list[0].target.y = origin_y - 2;
								move_list[0].target.x = origin_x + 2;
							}
						}

						if (origin_x - 1 >= 0) {
							tl_cell_piece = board[origin_y - 1][origin_x - 1];
							if (tl_cell_piece == '.') {
								move_list[3].target.y = origin_y - 1;
								move_list[3].target.x = origin_x - 1;
							}
							else if (tl_cell_piece == bw || tl_cell_piece == bw_tower) {
							}
							else {
								move_list[3].target.y = origin_y - 2;
								move_list[3].target.x = origin_x - 2;
							}
						}
					}
				}

				for (int k = 0; k < num_of_moves; k++) {
					int x = move_list[k].target.x;
					int y = move_list[k].target.y;
					if (x != -1) {
						if (x > 7 || x < 0) {
							move_list[k].target.x = -1;
							move_list[k].target.y = -1;
						}
						else if (y > 7 || y < 0) {
							move_list[k].target.x = -1;
							move_list[k].target.y = -1;
						}
						else if (board[y][x] != '.') {
							move_list[k].target.x = -1;
							move_list[k].target.y = -1;
						}
					}
				}
				for (int n = 0; n < num_of_moves; n++) {
					if (move_list[n].target.x != -1) {
						all_moves[move_no] = move_list[n];

						move_no++;
					}
				}
			}
		}
	}
	return move_no;
}
char
switch_colour(char bw) {
	if (bw == 'b') {
		return 'w';
	}
	else {
		return 'b';
	}
}
typedef struct node node_t;
struct node {
	board_t board_state;
	move_t move, child_move;
	int board_cost;
	int child_no;
	node_t** children;
};

typedef struct {
	node_t* root;
} tree_t;

void
play_move(board_t board, board_t child_board, move_t move) {
	int origin_x = move.origin.x;
	int origin_y = move.origin.y;
	int target_y = move.target.y;
	int target_x = move.target.x;
	int cap_cell_y;
	int cap_cell_x;

	child_board[target_y][target_x] = board[origin_y][origin_x];
	child_board[origin_y][origin_x] = CELL_EMPTY;

	if (origin_y > target_y) {
		cap_cell_y = origin_y - 1;
	}
	else {
		cap_cell_y = origin_y + 1;
	}

	if (origin_x > target_x) {
		cap_cell_x = origin_x - 1;
	}
	else {
		cap_cell_x = origin_x + 1;
	}
	if (target_y - origin_y == 2 || origin_y - target_y == 2) {
		child_board[cap_cell_y][cap_cell_x] = CELL_EMPTY;
	}
}
void
create_towers(board_t board) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (board[0][i] == 'b') {
			board[0][i] = 'B';
		}
		if (board[7][i] == 'w') {
			board[7][i] = 'W';
		}
	}
}
/* Copy board1 to board2 */
void
board_copy(board_t board1, board_t board2) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board2[i][j] = board1[i][j];
		}
	}
}


node_t* maximise(node_t* root);
node_t* minimise(node_t* root);

node_t*
maximise(node_t* root) {
	if (root->child_no == 0) {
		return root;
	}
	node_t* max_node;
	node_t* node;
	max_node = malloc(sizeof(node_t));
	node = malloc(sizeof(node_t));

	int max_cost = INT_MIN;
	int cost;
	for (int i = 0; i < root->child_no; i++) {
		node = minimise(root->children[i]);
		cost = node->board_cost;

		if (cost > max_cost) {
			max_node->move = root->move;
			max_node->child_move = node->move;
			max_cost = cost;
			max_node->board_cost = max_cost;
		}
	}
	return max_node;
}

node_t*
minimise(node_t* root) {
	if (root->child_no == 0) {
		return root;
	}
	node_t* min_node;
	node_t* node;
	min_node = malloc(sizeof(node_t));
	assert(min_node);
	node = malloc(sizeof(node_t));
	assert(node);
	int min_cost = INT_MAX;
	int cost;
	for (int i = 0; i < root->child_no; i++) {
		node = maximise(root->children[i]);
		cost = node->board_cost;

		if (cost < min_cost) {
			min_node->move = root->move;
			min_node->child_move = node->move;

			min_cost = cost;
			min_node->board_cost = min_cost;
		}
	}
	return min_node;
}


void
add_child(board_t board, node_t* root, char bw, int depth) {
	/* Base Case */
	move_t move_list[4];
	move_t all_moves[94];
	int no_of_moves = possible_moves(board, bw, move_list, all_moves);
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			root->board_state[i][j] = board[i][j];
		}
	}
	root->board_cost = board_cost(board);
	root->child_no = no_of_moves;
	if (no_of_moves == 0 || depth == 0) {
		root->child_no = 0;
		return;
	}

	/* Can make more children */
	root->children = malloc(no_of_moves * sizeof(node_t*));
	assert(root->children);
	node_t* new;
	for (int i = 0; i < no_of_moves; i++) {
		new = malloc(sizeof(node_t));
		root->children[i] = new;
		root->children[i]->move = all_moves[i];
		root->children[i]->child_move = all_moves[i];
	}

	for (int i = 0; i < no_of_moves; i++) {
		board_t child_board;
		board_copy(board, child_board);
		play_move(board, child_board, all_moves[i]);
		create_towers(child_board);
		add_child(child_board, root->children[i], switch_colour(bw), depth - 1);
	}
}

int
next_move(board_t board, char bw) {
	tree_t* tree;
	tree = malloc(sizeof(*tree));
	assert(tree);

	tree->root = NULL;
	node_t* new = malloc(sizeof(node_t));
	tree->root = new;
	add_child(board, tree->root, bw, TREE_DEPTH);

	node_t* state = malloc(sizeof(node_t));
	if (bw == 'b') {
		state = maximise(tree->root);
	}
	else {
		state = minimise(tree->root);
	}
	move_t move_list[4];
	move_t all_moves[50];

	play_move(board, board, state->child_move);
	create_towers(board);
	print_board(board);
	if (possible_moves(board, switch_colour(bw), move_list, all_moves) == 0) {
		//win 
		return 0;
	}
	return 1;
}



int
main(int argc, char* argv[]) {
	int n, cur_size;
	n = 2;
	cur_size = 0;
	move_t* turns = (move_t*)malloc(n * sizeof(*turns));
	assert(turns);

	char ox, tx, bw;
	char play;
	cell_t o_cell;
	cell_t t_cell;

	bw = 'b';
	while (scanf("%c%d%*c%c%d\n", &ox, &o_cell.y, &tx, &t_cell.y) == 4) {
		if (cur_size == n) {
			n *= 2;
			turns = (move_t*)realloc(turns, n * sizeof(*turns));
			assert(turns);
		}


		turns[cur_size] = *(move_t*)malloc(sizeof(*turns));
		o_cell.x = ox - 65;
		o_cell.y -= 1;
		t_cell.x = tx - 65;
		t_cell.y -= 1;
		turns[cur_size].origin = o_cell;
		turns[cur_size].target = t_cell;
		turns[cur_size].bw = bw;
		cur_size++;

		if (bw == 'b') {
			bw = 'w';
		}
		else {
			bw = 'b';
		}
	}
	// YOUR IMPLEMENTATION OF STAGES 0-2    
	board_t board;
	init_board(board);
	print_board(board);

	for (int i = 0; i < cur_size; i++) {
		if (move_valid(board, turns, i, turns[i].bw) == 0) {
			return;
		}
		update_board(board, turns[i].bw, i, turns, turn_type(turns, i));
		print_board(board);
		bw = turns[i].bw;
	}
	int actions = 0;
	if (ox == 'A') {
		actions = 1;
	}
	else if (ox == 'P') {
		actions = COMP_ACTIONS;
	}
	for (int i = 0; i < actions; i++) {
		bw = switch_colour(bw);
		printf("Turn %d\n", i);
		if (next_move(board, bw) == 0) {
			printf("%c wins", bw);
			return;
		}

	}

		return 0;            // exit program with the success code
}

/* THE END -------------------------------------------------------------------*/cd