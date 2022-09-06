#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>

#define PATH "D:/Data.txt"
#define PATH_OUT "D:/Data2.txt"
#define SIZE_BUFFER 256
#define THREADS_COUNT 5
#define ARR_SIZE 1000

CHAR *unsort_buff[ARR_SIZE] = { 0 };
CHAR *sort_buff[ARR_SIZE] = { 0 };
DWORD size_unsort_buffer = 0;
BOOL isProcessed = TRUE;
//CHAR* task_array_to_sort[50] = { 0 };
DWORD sortCount = 0;
int count = 0;

CRITICAL_SECTION section = { 0 };
HANDLE Thread_pool[THREADS_COUNT];

struct Task
{
	CHAR* task[ARR_SIZE];
	DWORD size_arr;
};

struct TaskQueue
{
	struct Task task_arr[ARR_SIZE];
	int frst, end;
};

struct TaskQueue tasksQueue;

void ReadFromDataFile();
void GetUNsortBuffer();
void CreateTaskQueue();
DWORD WINAPI TaskEnqueue(LPVOID lParam);
void SortArr(int count);
void WriteToFile();
void CreateThreadPool();
void FinalSort();

int main() 
{
	ReadFromDataFile();
	GetUNsortBuffer();

	tasksQueue.end = 0;
	tasksQueue.frst = 1;
	
	CreateTaskQueue();

	for (int i = 0; i < count; i++) 
	{
		int size = tasksQueue.task_arr[i].size_arr;
		printf("\n %d \n", size);

		for (int j = 0; j < size; j++) 
		{
			printf("%s\n", tasksQueue.task_arr[i].task[j]);
		}
	}

	InitializeCriticalSection(&section);
	CreateThreadPool();

	//FinalSort();
	
	FinalSort();

	for (int i = 0; i < size_unsort_buffer; i++)
	{
		printf("%s\n", sort_buff[i]);
	}

	WriteToFile();

	system("pause");
	return 0;
}

void ReadFromDataFile()
{
	HANDLE inFile = CreateFile(PATH, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	OVERLAPPED olf = { 0 };
	LARGE_INTEGER li = { 0 };

	li.QuadPart = 0;
	olf.Offset = li.LowPart;
	olf.OffsetHigh = li.HighPart;

	LPSTR buffer = (CHAR*)calloc(SIZE_BUFFER + 1, sizeof(CHAR));
	DWORD i = 0;
	DWORD iNumRead = 1;

	while (&iNumRead != 0 || (olf.Internal != -1 && !GetLastError))
	{
		if (!ReadFile(inFile, buffer, SIZE_BUFFER, &iNumRead, &olf))
		{
			break;
		}

		LPSTR currStr = strtok(buffer, "\n");

		while (currStr != NULL)
		{
			unsort_buff[i++] = currStr;
			currStr = strtok(NULL, "\n");
		}

		olf.Offset += iNumRead;
	}

	size_unsort_buffer = i;
	CloseHandle(inFile);
	return;
}

void GetUNsortBuffer()
{
	for (int i = 0; i < size_unsort_buffer; i++) 
	{
		printf("%s\n", unsort_buff[i]);
	}
	return;
}

void CreateTaskQueue()
{
	const int del = (size_unsort_buffer % THREADS_COUNT == 0) ? (size_unsort_buffer / THREADS_COUNT) : (size_unsort_buffer / THREADS_COUNT) + 1;
	DWORD size_arr = 0;
	CHAR* task_arr[ARR_SIZE] = { 0 };

	for (int i = 0; i < size_unsort_buffer; i++) 
	{
		task_arr[size_arr++] = unsort_buff[i];

		if (size_arr == del || i == size_unsort_buffer - 1) 
		{
			tasksQueue.end++;
			tasksQueue.task_arr[count].size_arr = size_arr;
			for (int j = 0; j < size_arr; j++)
			{
				tasksQueue.task_arr[count].task[j] = task_arr[j];
			}

			count++;
			size_arr = 0;
		}

		
	}
	return;
}

DWORD WINAPI TaskEnqueue(LPVOID lParam)
{
	EnterCriticalSection(&section);

	BOOL isPushed = tasksQueue.end >= tasksQueue.frst ? TRUE : FALSE;
	DWORD taskNumber = tasksQueue.end--;
		
	LeaveCriticalSection(&section);

	if (isPushed) 
	{
		SortArr(taskNumber - 1);
	}
	return 0;
}

	

void SortArr(int count)
{
	DWORD size = tasksQueue.task_arr[count].size_arr;

	CHAR* temp;
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			if (strcmp(tasksQueue.task_arr[count].task[i], tasksQueue.task_arr[count].task[j]) > 0)
			{
				temp = tasksQueue.task_arr[count].task[i];
				tasksQueue.task_arr[count].task[i] = tasksQueue.task_arr[count].task[j];
				tasksQueue.task_arr[count].task[j] = temp;
			}
		}
	}

	for (int i = 0; i < size; i ++) 
	{
		printf(" i = %d\t\t cout = %d\t\t arr_:%s\n", i, count, tasksQueue.task_arr[count].task[i]);
	}

	return;
}

void WriteToFile()
{
	FILE *hFile = NULL;

	hFile = fopen(PATH_OUT, "w");
	
	if (hFile != NULL)
	{
		for (size_t i = 0; i < size_unsort_buffer; i++)
		{
			//for (int j = 0; j < tasksQueue.task_arr[i].size_arr; j++)
			//{
			fprintf(hFile, "%s", sort_buff[i]);
			//}
		}
	}

	fclose(hFile);
	return;
}

void CreateThreadPool()
{
	for (int i = 0; i < THREADS_COUNT; i++) 
	{
		Thread_pool[i] = CreateThread(NULL, 0, TaskEnqueue, NULL, NULL, 0);
	}

	WaitForMultipleObjects(THREADS_COUNT, Thread_pool, TRUE, INFINITE);

	return;
}

void FinalSort()
{
	CHAR* help_arr[ARR_SIZE] = { 0 };

	for (int i = 0; i < tasksQueue.task_arr[0].size_arr; i++)
	{
		help_arr[i] = tasksQueue.task_arr[0].task[i];
	}

	int countSortArr = 0;
	int sizeStruct = tasksQueue.task_arr[0].size_arr;
	int sizeArr = sizeStruct;

	for (int i = 1; i < count; i++) 
	{
		sizeStruct = tasksQueue.task_arr[i].size_arr;
		

		int index_struct = 0, index_array = 0;

		while (TRUE)
		{
			if (index_struct == sizeStruct)
			{
				while (index_array < sizeArr)
				{
					sort_buff[countSortArr++] = help_arr[index_array];
					index_array++;
				}
				break;
			}

			if (index_array == sizeArr)
			{
				while (index_struct < sizeStruct)
				{
					sort_buff[countSortArr++] = tasksQueue.task_arr[i].task[index_struct];
					index_struct++;
				}
				break;
			}

			if (strcmp(help_arr[index_array], tasksQueue.task_arr[i].task[index_struct])> 0)
			{
				sort_buff[countSortArr++] = tasksQueue.task_arr[i].task[index_struct];
				index_struct++;
			}
			else 
			{
				sort_buff[countSortArr++] = help_arr[index_array];
				index_array++;
			}

		}

		sizeArr += sizeStruct;

		for (int i = 0; i < countSortArr; i++)
		{
			help_arr[i] = sort_buff[i];
		}

		countSortArr = 0;
	}
}
