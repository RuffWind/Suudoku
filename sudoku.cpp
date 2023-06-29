#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;

#define DEBUG

ifstream  fin;
ofstream fout;

int sudoku_global[11][11];  // Sudoku, a 2d rray
int seed[9];  // A seed used for generating a sudoku (Actually it is the order of the 9 numbers in the very middle cell of a sudoku. )
const int sudoku_n = 9;  // The sudoku size is 9 * 9
/* config */
const string sudoku_path = "data/end_game.txt";  // Final sudokus
const string puzzle_path = "data/puzzle.txt";  // Puzzles
const string solution_path = "data/sudoku.txt";  // Answers of the puzzles

void inline displaySeed(int* seed) {
	cout << "Seed: ";
	for (int i = 0; i < 9; i++) {
		cout << seed[i] << " ";
	}
	cout << endl;
}

inline void displaySudoku(int sudoku[][11]) {
	for (int i = 1; i <= 9; i++) {
		for (int j = 1; j <= 9; j++) {
			if ((i == 4 && j == 1) || (i == 7 && j == 1)) {
				cout << "---------------------" << endl;
			}
			else if (j == 4 || j == 7) {
				cout << "| ";
			}
			if (sudoku[i][j] == 0) {
				cout << '$' << " ";
			}
			else {
				cout << sudoku[i][j] << " ";
			}
		}
		cout << endl;
	}
	cout << endl;
}

bool save(int sudoku[][11], string path, bool is_first = false, int index = 0) {
	if (is_first) {
		fout.open(path, ios::out);
	}
	else {
		fout.open(path, ios::app);
	}
	if (!fout.is_open()) {
		std::cerr << "Cannot open the file!\n";
		return false;
	}
	if (index > 0) {
		fout << "Solution" << index << endl;
	}
	else if (index < 0) {
		fout << "Puzzle" << -index << endl;
	}
	for (int i = 1; i <= 9; i++) {
		for (int j = 1; j <= 9; j++) {
			if (sudoku[i][j] == 0) {
				fout << '$' << " ";
			}
			else {
				fout << sudoku[i][j] << " ";
			}
		}
		fout << endl;
	}
	fout << endl;
	fout.close();
	return true;
}

/* Solving */
// Check repetition in a row
bool checkX(int x)
{
	for (int i = 1; i <= 9; i++)
	{
		if (sudoku_global[x][i] == 0)
		{
			continue;
		}
		for (int j = 1; j < i; j++)
		{
			if (sudoku_global[x][i] == sudoku_global[x][j])
			{
				return false;
			}
		}
	}
	return true;
}
// Check repetition in a column
bool checkY(int y)
{
	for (int i = 1; i <= 9; i++)
	{
		if (sudoku_global[i][y] == 0)
		{
			continue;
		}
		for (int j = 1; j < i; j++)
		{
			if (sudoku_global[i][y] == sudoku_global[j][y])
			{
				return false;
			}
		}
	}
	return true;
}
// Check repetition in a cell
bool judgeNine(int x, int y)
{
	for (int i = x; i < x + 3; i++)
	{
		for (int j = y; j < y + 3; j++)
		{
			if (sudoku_global[i][j] == 0)
			{
				continue;
			}
			for (int ii = x; ii < i; ii++)
			{
				for (int jj = y; jj < y + 3; jj++)
				{
					if (sudoku_global[i][j] == sudoku_global[ii][jj])
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}
bool judge()
{
	if (judgeNine(1, 1) == false)
	{
		return false;
	}
	if (judgeNine(4, 1) == false)
	{
		return false;
	}
	if (judgeNine(7, 1) == false)
	{
		return false;
	}
	if (judgeNine(1, 4) == false)
	{
		return false;
	}
	if (judgeNine(4, 4) == false)
	{
		return false;
	}
	if (judgeNine(7, 4) == false)
	{
		return false;
	}
	if (judgeNine(1, 7) == false)
	{
		return false;
	}
	if (judgeNine(4, 7) == false)
	{
		return false;
	}
	if (judgeNine(7, 7) == false)
	{
		return false;
	}
	return true;
}

// Deep-First-Search
int CUR_PUZZLE_NO = 0;
bool VERY_FIRST = true;  // Do set this boolean to true each time before you use dfs to save the first puzzle!
void dfs(int sudoku[][11], int* ans_num = nullptr, int index = 0, int na = 1, int nb = 1) {
	if (na == sudoku_n + 1) {
		if (judge() == true) {
			(*ans_num)++;
			if (index > 0) {
				save(sudoku_global, solution_path, index == 1 && VERY_FIRST, index);
				VERY_FIRST = false;
			}
#ifdef DEBUG
			if (index > 0) {
				cout << "Solution " << CUR_PUZZLE_NO + 1 << ": \n";
				displaySudoku(sudoku);
			}
#endif
			return;
		}
	}
	if (sudoku[na][nb] != 0) {
		int nna = na, nnb = nb;
		if (nb == sudoku_n) {
			na++, nb = 1;
		}
		else {
			nb++;
		}
		dfs(sudoku, ans_num, index, na, nb);
		na = nna, nb = nnb;
		return;
	}
	for (int i = 1; i <= sudoku_n; i++) {
		sudoku[na][nb] = i;
		if (checkX(na) == false || checkY(nb) == false || judge() == false) {
			sudoku[na][nb] = 0;
			continue;
		}
		int nna = na, nnb = nb;
		if (nb == sudoku_n) {
			na++, nb = 1;
		}
		else {
			nb++;
		}
		dfs(sudoku, ans_num, index, na, nb);
		na = nna, nb = nnb;
		sudoku[nna][nnb] = 0;
	}
}

/* Generating */
// Generat a sudoku with a seed
void genSudoku(int sudoku[][11], int* seed) {
	// Step 1: Generate the very middle cell of the sudoku with a 9-length seed (1D array)
	int si = 0;
	for (int i = 4; i <= 6; i++) {
		for (int j = 4; j <= 6; j++) {
			sudoku[i][j] = seed[si++];
		}
	}
	// Step 2: Generate surrounding four cells (top, bottom, left and right) by rolling the middle cell 
	for (int i = 4; i <= 6; i++) {
		int bias = 1;
		for (int j = 4; j <= 6; j++) {
			if (i == 4) {
				sudoku[i + 1][bias] = sudoku[i][j];
				sudoku[i + 2][bias + 6] = sudoku[i][j];
				bias++;
			}
			else if (i == 5) {
				sudoku[i + 1][bias] = sudoku[i][j];
				sudoku[i - 1][bias + 6] = sudoku[i][j];
				bias++;
			}
			else if (i == 6) {
				sudoku[i - 2][bias] = sudoku[i][j];
				sudoku[i - 1][bias + 6] = sudoku[i][j];
				bias++;
			}
		}
	}
	for (int j = 4; j <= 6; j++) {
		int bias = 1;
		for (int i = 4; i <= 6; i++) {
			if (j == 4) {
				sudoku[bias][j + 1] = sudoku[i][j];
				sudoku[bias + 6][j + 2] = sudoku[i][j];
				bias++;
			}
			else if (j == 5) {
				sudoku[bias][j + 1] = sudoku[i][j];
				sudoku[bias + 6][j - 1] = sudoku[i][j];
				bias++;
			}
			else if (j == 6) {
				sudoku[bias][j - 2] = sudoku[i][j];
				sudoku[bias + 6][j - 1] = sudoku[i][j];
				bias++;
			}
		}
	}
	// Step 3: Generate the four corner cells the same way as Step 2
	for (int i = 1; i <= 3; i++) {
		int bias = 1;
		for (int j = 4; j <= 6; j++) {
			if (i == 1) {
				sudoku[i + 1][bias] = sudoku[i][j];
				sudoku[i + 2][bias + 6] = sudoku[i][j];
				bias++;
			}
			else if (i == 2) {
				sudoku[i + 1][bias] = sudoku[i][j];
				sudoku[i - 1][bias + 6] = sudoku[i][j];
				bias++;
			}
			else if (i == 3) {
				sudoku[i - 2][bias] = sudoku[i][j];
				sudoku[i - 1][bias + 6] = sudoku[i][j];
				bias++;
			}
		}
	}
	for (int i = 7; i <= 9; i++) {
		int bias = 1;
		for (int j = 4; j <= 6; j++) {
			if (i == 7) {
				sudoku[i + 1][bias] = sudoku[i][j];
				sudoku[i + 2][bias + 6] = sudoku[i][j];
				bias++;
			}
			else if (i == 8) {
				sudoku[i + 1][bias] = sudoku[i][j];
				sudoku[i - 1][bias + 6] = sudoku[i][j];
				bias++;
			}
			else if (i == 9) {
				sudoku[i - 2][bias] = sudoku[i][j];
				sudoku[i - 1][bias + 6] = sudoku[i][j];
				bias++;
			}
		}
	}
}

// Generate a puzzle by digging "hole_num" holes in a sudoku generated before
void genPuzzle(int sudoku[][11], int hole_num, bool need_unique=false) {
	for (int i = 0; i < hole_num; i++) {
		int rand_r, rand_c;
		while(true) {
			rand_r = rand() % sudoku_n + 1;
			rand_c = rand() % sudoku_n + 1;
			if (sudoku[rand_r][rand_c] == 0) {
				continue;
			}
			if (need_unique) {
				int original_val = sudoku[rand_r][rand_c];
				sudoku[rand_r][rand_c] = 0;  // 0 means blank
				int ans_num = 0;
				dfs(sudoku, &ans_num);
				if (ans_num > 1) {
					sudoku[rand_r][rand_c] = original_val;
					continue;
				}
				else {
					break;
				}
			}
			else {
				sudoku[rand_r][rand_c] = 0;  // 0 means blank
				break;
			}
		}
	}
}

// Generate seeds with backtracking algorithm, and generate sudokus with these seeds
int res[9];  // Record the result
bool line[10] = { false };  // Record whether a number has been included in array "res"
int cur_seed_num = 0;  // The number of seeds generated for now
bool has_printed = false;
void genSeedSudoku(int seed_num, bool is_puzzle = false, int hole_num = 0, bool need_unique = false, int max_n = sudoku_n, int index = 0) {
	// Param "index" should be 0 while using this function from outside scope.  
	if (cur_seed_num >= seed_num) {
		if (!has_printed) {
			cout << "[INFO] " << cur_seed_num << " seeds (and sudokus/puzzles at the same time) have been generated.\n\n";
			has_printed = true;
		}
		return;
	}
	if (index == max_n) {
		for (int i = 0; i < max_n; i++) {
			seed[i] = res[i];
		}
		genSudoku(sudoku_global, seed);
		if (is_puzzle) {
			srand(time(0) + cur_seed_num);
			genPuzzle(sudoku_global, hole_num, need_unique);
#ifdef DEBUG
			cout << "Puzzle " << cur_seed_num + 1 << ": \n";
			displaySudoku(sudoku_global);
#endif
			save(sudoku_global, puzzle_path, cur_seed_num == 0 ? true : false, -cur_seed_num - 1);
		}
		else {
			save(sudoku_global, sudoku_path, cur_seed_num == 0 ? true : false);
		}
		cur_seed_num++;
		return;
	}
	for (int i = 1; i <= max_n; i++) {
		if (line[i] == false) {
			res[index] = i;
			line[i] = true;
			genSeedSudoku(seed_num, is_puzzle, hole_num, need_unique, max_n, index + 1);
			line[i] = false;
		}
	}
}


// Application
bool solvePuzzle(int sudoku1[][11], string puzzle_path, string solution_path) {
	fin.open(puzzle_path, ios::in);
	if (!fin.is_open()) {
		std::cerr << "Cannot open the file!\n";
		return false;
	}
	int index = 0;  // Record where we are
	char buf[1024] = { 0 };
	while (fin >> buf) {
		if (buf[0] == 'P') {
			continue;
		}
		int row = (index / sudoku_n) % sudoku_n + 1;
		int col = index % sudoku_n + 1;  // row and col start from 1 ...
		sudoku_global[row][col] = atoi(buf);  // '$' will be automatically converted to number 0 by "atoi"
		index++;
		if (index % (sudoku_n * sudoku_n) == 0) {
			int ans_num = 0;
			VERY_FIRST = true;
			dfs(sudoku_global, &ans_num, index / (sudoku_n * sudoku_n));
#ifdef DEBUG
			cout << "ANS_NUM: " << ans_num << endl << endl;
#endif
			CUR_PUZZLE_NO++;
		}
	}
	fin.close();
	return true;
}


int main(int argc, char* argv[])
{
	/* ----------------- */
	/* All possible argument situations: */
	/*
	* --- argc == 3 ---
	* sudoku.exe -c [1-1000000]  # Generate end_game
	* sudoku.exe -s [path]       # Solve puzzles in [path]
	* sudoku.exe -n [1-10000]    # Generate puzzles
	* --- argc == 5 ---
	* sudoku.exe -n [1-10000] -m [1-3]   # Define difficulty
	* sudoku.exe -n [1-10000] -r [20-55] # Define holes to be dug
	* ##### If argc is 4 or 6, there is supposed to be a "-u" #####
	* sudoku.exe -n [1-10000] -u
	* sudoku.exe -n [1-10000] -r [20-55] -u
	* ### Constraints ###
	* "-m" and "-r" must be used together with "-n"
	*/
#ifdef DEBUG
	// Show args
	cout << "argc: " << argc << endl;
	for (int i = 0; i < argc; i++) {
		cout << "argv[" << i << "]: " << argv[i] << endl;
	}
#endif
	// Argument handling
	const string error_msg = "[Error] Bad arguments. For more help, read the instruction.\n";
	string arg_path;  // puzzle path in the argument (-s)
	int seed_num = 0;  // The number of seeds, and sudokus at the same time, to be generated (-c/-n)
	bool need_unique = false;  // Whether each puzzle must have a unique solution (-u)
	int difficulty[4] = { 0,20,35,50 };  // The number of holes in different difficulty levels
	switch (argc) {
	case 4:
		if (strcmp(argv[3], "-u")) {
			cout << error_msg;
			return 1;
		}
		need_unique = true;
	case 3:
		if (!strcmp(argv[1], "-c")) {
			seed_num = atoi(argv[2]);  // Bad char[] will be converted to 0 automatically
			if (seed_num < 1 || seed_num>1000000 || need_unique) {
				cout << error_msg;
				return 1;
			}
			genSeedSudoku(seed_num);
		}
		else if (!strcmp(argv[1], "-s")) {
			arg_path = argv[2];
			if (!solvePuzzle(sudoku_global, arg_path, solution_path)) {
				return 1;
			};
		}
		else if (!strcmp(argv[1], "-n")) {
			seed_num = atoi(argv[2]);
			if (seed_num < 1 || seed_num>10000) {
				cout << error_msg;
				return 1;
			}
			genSeedSudoku(seed_num, true, 20, need_unique);  // Dig 20 holes by default
		}
		else {
			cout << error_msg;
			return 1;
		}
		break;
	case 6:
		if (strcmp(argv[5], "-u")) {
			cout << error_msg;
			return 1;
		}
		need_unique = true;
	case 5:
		if (strcmp(argv[1], "-n")) {
			cout << error_msg;
			return 1;
		}
		seed_num = atoi(argv[2]);
		if (seed_num < 1 || seed_num>10000) {
			cout << error_msg;
			return 1;
		}
		if (!strcmp(argv[3], "-m")) {
			int level = atoi(argv[4]);
			if (level < 1 || level >3) {
				cout << error_msg;
				return 1;
			}
			genSeedSudoku(seed_num, true, difficulty[level], need_unique);
		}
		else if (!strcmp(argv[3], "-r")) {
			int hole_num = atoi(argv[4]);
			if (hole_num < 20 || hole_num >55) {
				cout << error_msg;
				return 1;
			}
			genSeedSudoku(seed_num, true, hole_num, need_unique);
		}
		else {
			cout << error_msg;
			return 1;
		}
		break;
	default:
		cout << error_msg;
		return 1;
	}
	return 0;
}
