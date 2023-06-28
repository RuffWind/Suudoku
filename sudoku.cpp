#include<iostream>
using namespace std;

int sudoku[11][11];  // Sudoku, a 2d rray
int ans_num = 0;  // The number of answers

/* Generating */
void gen_sudoku(int** sudoku, int* seed) {
	// Step 1: Generate the very middle cell of the sudoku with a 9-length seed (1D array)
	int si = 0;
	for (int i = 4; i <= 6; i++) {
		for (int j = 4; j <= 6; j++) {
			sudoku[i][j] = seed[si++];
		}
	}
	// Step 2: Generate surrounding four cells (top, bottom, left and right) by rolling the middle cell 
	for (int i = 4; i <= 6; i++) {
		int bias = 0;
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
	// Step 3: Generate the four corner cells the same way as Step 2

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
			cout << "解" << ans_num << endl;
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

int main()
{
	cout << "请输入9×9的数独" << endl;
	for (int i = 1; i <= 9; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			cin >> sudoku[i][j];
		}
	}
	dfs(1, 1);
	if (ans_num == 0)
	{
		cout << "本数独无解" << endl;
	}
	else
	{
		cout << "共有" << ans_num << "个解" << endl;
	}
	return 0;
}