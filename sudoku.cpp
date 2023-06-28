#include <iostream>
#include <fstream>
#include <string>
using namespace std;
ifstream  fin;
ofstream fout;

int sudoku[11][11];  // Sudoku, a 2d rray
int ans_num = 0;  // The number of answers
int seed[9];  // A seed used for generating a sudoku (Actually it is the order of the 9 numbers in the very middle cell of a sudoku. )
/* config */
const string puzzle_path = "D:\\Codes\\vs2019\\Sudoku\\Sudoku\\data\\puzzles.txt";
const string solution_path = "D:\\Codes\\vs2019\\Sudoku\\Sudoku\\data\\solutions.txt";
int seed_num = 0;  // The number of seeds, and sudokus at the same time, to be generated

/* Utilities */
void inline display_seed(int* seed) {
	cout << "Seed: ";
	for (int i = 0; i < 9; i++) {
		cout << seed[i] << " ";
	}
	cout << endl;
}

void inline display_sudoku(int sudoku[][11]) {
	for (int i = 1; i <= 9; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			if ((i == 4 && j == 1) || (i == 7 && j == 1))
			{
				cout << "---------------------" << endl;
			}
			else if (j == 4 || j == 7)
			{
				cout << "| ";
			}
			cout << sudoku[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void save_sudoku(int sudoku[][11], bool is_first = false, bool display = false, string path = puzzle_path) {
	if (is_first) {
		fout.open(path, ios::out);
	}
	else {
		fout.open(path, ios::app);
	}
	if (!fout.is_open()) {
		std::cerr << "Cannot open the file!\n";
	}
	for (int i = 1; i <= 9; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			if ((i == 4 && j == 1) || (i == 7 && j == 1))
			{
				fout << "---------------------" << endl;
			}
			else if (j == 4 || j == 7)
			{
				fout << "| ";
			}
			fout << sudoku[i][j] << " ";
		}
		fout << endl;
	}
	fout << endl;
	fout.close();
	if (display) {
		display_sudoku(sudoku);
	}
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
// Generate seeds with backtracking algorithm, and generate sudokus with these seeds
const int sudoku_n = 9;
int res[9];  // Record the result
bool line[10] = { false };  // Record whether a number has been included in array "res"
int cur_seed_num = 0;  // The number of seeds generated for now
bool has_printed = false;
void gen_seed_sudoku(int seed_num, int max_n = sudoku_n, int index = 0) {
	// Param "index" should be 0 while using this function from outside scope.  
	if (cur_seed_num >= seed_num) {
		if (!has_printed) {
			cout << cur_seed_num << " seeds (and sudokus at the same time) have been generated.\n";
			has_printed = true;
		}
		return;
	}
	if (index == max_n) {
		for (int i = 0; i < max_n; i++) {
			seed[i] = res[i];
		}
		gen_sudoku(sudoku, seed);
		save_sudoku(sudoku, cur_seed_num == 0 ? true : false);
		cur_seed_num++;
		return;
	}
	for (int i = 1; i <= max_n; i++) {
		if (line[i] == false) {
			res[index] = i;
			line[i] = true;
			gen_seed_sudoku(seed_num, max_n, index + 1);
			line[i] = false;
		}
	}
}

/* Solving */
bool checkX(int x)
{
	for (int i = 1; i <= 9; i++)
	{
		if (sudoku[x][i] == 0)
		{
			continue;
		}
		for (int j = 1; j < i; j++)
		{
			if (sudoku[x][i] == sudoku[x][j])
			{
				return false;
			}
		}
	}
	return true;
}//判断每一行是否有重复

bool checkY(int y)
{
	for (int i = 1; i <= 9; i++)
	{
		if (sudoku[i][y] == 0)
		{
			continue;
		}
		for (int j = 1; j < i; j++)
		{
			if (sudoku[i][y] == sudoku[j][y])
			{
				return false;
			}
		}
	}
	return true;
}//判断每一列是否有重复

bool judgeNine(int x, int y)
{
	for (int i = x; i < x + 3; i++)
	{
		for (int j = y; j < y + 3; j++)
		{
			if (sudoku[i][j] == 0)
			{
				continue;
			}
			for (int ii = x; ii < i; ii++)
			{
				for (int jj = y; jj < y + 3; jj++)
				{
					if (sudoku[i][j] == sudoku[ii][jj])
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}//判断九宫格是否有重复（辅助判断）

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

void dfs(int na, int nb)
{
	if (na == 10)
	{
		if (judge() == true)
		{
			ans_num++;
			display_sudoku(sudoku);
			return;
		}
	}
	if (sudoku[na][nb] != 0)
	{
		int nna = na, nnb = nb;
		if (nb == 9)
		{
			na++, nb = 1;
		}
		else
		{
			nb++;
		}
		dfs(na, nb);
		na = nna, nb = nnb;
		return;
	}
	for (int i = 1; i <= 9; i++)
	{
		sudoku[na][nb] = i;
		if (checkX(na) == false || checkY(nb) == false || judge() == false)
		{
			sudoku[na][nb] = 0;
			continue;
		}
		int nna = na, nnb = nb;
		if (nb == 9)
		{
			na++, nb = 1;
		}
		else
		{
			nb++;
		}
		dfs(na, nb);
		na = nna, nb = nnb;
		sudoku[nna][nnb] = 0;
	}
}//搜索与回溯


int main(int argc, char* argv[])
{
	/* Generating */
	gen_seed_sudoku(seed_num);
	/* Solving */
	/*
	dfs(1, 1);
	if (ans_num == 0) {
		cout << "本数独无解" << endl;
	}
	else {
		cout << "共有" << ans_num << "个解" << endl;
	}
	*/
	return 0;
}
