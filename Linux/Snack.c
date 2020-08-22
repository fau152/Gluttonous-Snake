#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define time 250000

enum DIRR {UP,DOWN,LEFT,RIGHT};

typedef struct node
{
	int hang;
	int lie;
	struct node* pNext;
}Snack;


static Snack* head = NULL;
static Snack* tail = NULL; 
Snack food;
enum DIRR dir;
int key;

int IsSnack(int hang,int lie)
{
	Snack* p = head;
	while(p!=NULL)
	{
		if(hang == p->hang && lie == p->lie)
		{
			return 1;
		}
		p = p->pNext;
	}
	return 0;
}

int IsFood(int hang,int lie)
{
	if(food.hang == hang && food.lie == lie)
	{
		return 1;
	}
	return 0;
}

void ShowWindow()
{
	int hang,lie;
	move(0,0);

	for(hang=0;hang<=21;hang++)
	{
		if(hang==0 || hang==21)
		{
			printw("-");
			for(lie=1;lie<=20;lie++)
			{
				printw("--");
			}
			printw("-");
		}
		else
		{
			printw("|");
			for(lie=1;lie<=20;lie++)
			{
				if(IsSnack(hang,lie))
				{
					printw("[]");
				}
				else if(IsFood(hang,lie))
				{
					printw("()");
				}
				else
				{
					printw("  ");
				}
			}
			printw("|");
		}
		printw("\n");
	}
	printw("by wanglong\n");
	printw("food.hang=%d,food.lie=%d\n",food.hang,food.lie);
	switch(key)
	{
		case KEY_UP:
			printw("UP\n");
			break;
		case KEY_DOWN:
			printw("DOWN\n");
			break;
		case KEY_LEFT:
			printw("LEFT\n");
			break;
		case KEY_RIGHT:
			printw("RIGHT\n");
			break;
		default:
			break;
	}

}

void InitGame()
{
	initscr();
	keypad(stdscr,1);
	noecho();
}

void EndGame()
{
	endwin();
}

void InitFood()
{
	int x = (rand()%20)+1;
	int y = (rand()%20)+1;
	food.hang = x;
	food.lie = y;
}

void InitSnack(enum DIRR* dir)
{
	*dir = RIGHT;

	Snack* s1 = (Snack*)malloc(sizeof(Snack));
	s1->hang = 2;
	s1->lie = 5;
	Snack* s2 = (Snack*)malloc(sizeof(Snack));
	s2->hang = 2;
	s2->lie = 4;
	Snack* s3 = (Snack*)malloc(sizeof(Snack));
	s3->hang = 2;
	s3->lie = 3;

	s1->pNext = s2;
	s2->pNext = s3;
	s3->pNext = NULL;

	head = s1;
	tail = s3;
}

int EatFood()
{
	if(food.hang == head->hang && food.lie == head->lie)
	{
		return 1;
	}
	return 0;
}

int FoodInSnack()
{
	Snack* p = head->pNext;
	while(p!=NULL)
	{
		if(p->hang == food.hang && p->lie == food.lie)
		{
			return 1;
		}
		p = p->pNext;
	}
	return 0;
}

void MoveSnack(enum DIRR dir)
{
	//添加头结点
	Snack* s = (Snack*)malloc(sizeof(Snack));
	switch(dir)
	{
		case UP:
			s->hang = head->hang-1;
			s->lie = head->lie;
			break;
		case DOWN:
			s->hang = head->hang+1;
			s->lie = head->lie;
			break;
		case LEFT:
			s->hang = head->hang;
			s->lie = head->lie-1;
			break;
		case RIGHT:
			s->hang = head->hang;
			s->lie = head->lie+1;
			break;
	}
	s->pNext = head;
	head = s;

	if(EatFood())
	{
		do
		{
			InitFood();
		}while(FoodInSnack());
	}
	else
	{
		//删除尾结点
		Snack* t = head;
		while(t->pNext->pNext!=NULL)
		{
			t = t->pNext;
		}
		tail = t;
		t = t->pNext;
		free(t);
		t = NULL;
		tail->pNext = NULL;
	}
}

void* func1()
{
	while(1)
	{
		MoveSnack(dir);
		refresh();
		ShowWindow();
		usleep(time);
	}
}

void* func2()
{
	while(1)
	{
		key = getch();
		switch(key)
		{
			case KEY_UP:
				if(dir!=DOWN)
				{
					dir = UP;
				}
				break;
			case KEY_DOWN:
				if(dir!=UP)
				{
					dir = DOWN;
				}
				break;
			case KEY_LEFT:
				if(dir!=RIGHT)
				{
					dir = LEFT;
				}
				break;
			case KEY_RIGHT:
				if(dir!=LEFT)
				{
					dir = RIGHT;
				}
				break;
		}
	}
}

int SnackSelf()
{
	Snack* p = head->pNext->pNext;
	while(p != NULL)
	{
		if(p->hang == head->hang && p->lie == head->lie)
		{
			return 1;	
		}
		p = p->pNext;
	}
	return 0;
}

int IsDie()
{
	if(head->hang<1 || head->hang>20 || head->lie<1 || head->lie>20 || SnackSelf())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int main(void)
{
	pthread_t th1;
	pthread_t th2;

	InitGame();
	InitFood();
	InitSnack(&dir); 

	pthread_create(&th1,NULL,func1,NULL);
	pthread_create(&th2,NULL,func2,NULL);

	while(1)
	{
		if(IsDie())
		{
			beep();
			InitFood();
			InitSnack(&dir); 
		}
	}

	EndGame();
	return 0;
}