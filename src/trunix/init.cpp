#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
	// Before
	std::cout << "----------------------" << std::endl;
	std::cout << "Begin Bash" << std::endl;
	std::cout << "----------------------" << std::endl;

	// Execute
	int ret = execve("/bin/bash", argv, envp);

	// After
	std::cout << "----------------------" << std::endl;
	std::cout << "End Bash (" << ret << ")" << std::endl;
	std::cout << "----------------------" << std::endl;

	// Success
	return 0;
}
