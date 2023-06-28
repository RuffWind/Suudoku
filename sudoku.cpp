#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;

//#define DEBUG

ifstream  fin;
ofstream fout;

int sudoku_global[11][11];  // Sudoku, a 2d rray
int seed[9];  // A seed used for generating a sudoku (Actually it is the order of the 9 numbers in the very middle cell of a sudoku. )
const int sudoku_n = 9;  // The sudoku size is 9 * 9
/* config */
const string sudoku_path = "D:\\Codes\\vs2019\\Sudoku\\Sudoku\\data\\sudoku.txt";  // Final sudokus
const string puzzle_path = "D:\\Codes\\vs2019\\Sudoku\\Sudoku\\data\\puzzle.txt";  // Puzzles
const string solution_path = "D:\\Codes\\vs2019\\Sudoku\\Sudoku\\data\\sudoku.txt";  // Answers of the puzzles
int seed_num = 3;  // The number of seeds, and sudokus at the same time, to be generated

/* Utilities */
void inline display_seed(int* seed) {
	cout << "Seed: ";
	for (int i = 0; i < 9; i++) {
		cout << seed[i] << " ";
	}
	cout << endl;
}

inline void display_sudoku(int sudoku[][11]) {
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

void save(int sudoku[][11], string path, bool is_first = false) {
	if (is_first) {
		fout.open(path, ios::out);
	}
	else {
		fout.open(path, ios::app);
	}
	if (!fout.is_open()) {
		std::cerr << "Cannot open the file!\n";
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
}

/* Generating */
// Generat a sudoku with a seed
void gen_sudoku(int sudoku[][11], int* seed) {
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
void gen_puzzle(int sudoku[][11], int hole_num) {
	for (int i = 0; i < hole_num; i++) {
		int rand_r, rand_c;
		do {
			rand_r = rand() % sudoku_n + 1;
			rand_c = rand() % sudoku_n + 1;
		} while (sudoku[rand_r][rand_c] == 0);  // 0 means blank
		sudoku[rand_r][rand_c] = 0;
	}
}

// Generate seeds with backtracking algorithm, and generate sudokus with these seeds
int res[9];  // Record the result
bool line[10] = { false };  // Record whether a number has been included in array "res"
int cur_seed_num = 0;  // The number of seeds generated for now
bool has_printed = false;
void gen_seed_sudoku(int seed_num, bool is_puzzle = false, int hole_num = 0, int max_n = sudoku_n, int index = 0) {
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
		gen_sudoku(sudoku_global, seed);
		if (is_puzzle) {
			srand(time(0) + cur_seed_num);
			gen_puzzle(sudoku_global, hole_num);
#ifdef DEBUG
			cout << "Puzzle " << cur_seed_num + 1 << ": \n";
			display_sudoku(sudoku);
#endif
			save(sudoku_global, puzzle_path, cur_seed_num == 0 ? true : false);
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
			gen_seed_sudoku(seed_num, is_puzzle, hole_num, max_n, index + 1);
			line[i] = false;
		}
	}
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
}//判断九宫格是否有重复

// Deep-First-Search
int cur_puzzle_no = 0;
int ans_num = 0;  // The number of solutions
void dfs(int sudoku[][11], int index = 0, int na = 1, int nb = 1) {
	if (na == sudoku_n + 1) {
		if (judge() == true) {
			ans_num++;
			save(sudoku_global, solution_path, index == 1 ? true : false);
#ifdef DEBUG
			cout << "Solution " << cur_puzzle_no + 1 << ": \n";
			display_sudoku(sudoku);
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
		dfs(sudoku, index, na, nb);
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
		dfs(sudoku, index, na, nb);
		na = nna, nb = nnb;
		sudoku[nna][nnb] = 0;
	}
}

// Application
void solve_puzzle(int sudoku1[][11], string puzzle_path, string solution_path) {
	fin.open(puzzle_path, ios::in);
	if (!fin.is_open()) {
		std::cerr << "Cannot open the file!\n";
	}
	int index = 0;  // Record where we are
	char buf[1024] = { 0 };
	while (fin >> buf) {
		int row = (index / sudoku_n) % sudoku_n + 1;
		int col = index % sudoku_n + 1;  // row and col start from 1 ...
		sudoku_global[row][col] = atoi(buf);  // '$' will be automatically converted to number 0 by "atoi"
		index++;
		if (index % (sudoku_n * sudoku_n) == 0) {
			ans_num = 0;
			dfs(sudoku_global, index / (sudoku_n * sudoku_n));
#ifdef DEBUG
			cout << "ANS_NUM: " << ans_num << endl << endl;
#endif
			cur_puzzle_no++;
		}
	}
	fin.close();
}


int main(int argc, char* argv[])
{
	/* Generating */
	gen_seed_sudoku(seed_num, true, 30);
	solve_puzzle(sudoku_global, puzzle_path, solution_path);
	return 0;
}
