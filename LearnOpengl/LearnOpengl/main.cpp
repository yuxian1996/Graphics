#include "Program.h"

int main()
{
	Program program;
	if (program.Init())
	{
		program.Run();
	}
	program.Destroy();

	getchar();

	return 0;
}