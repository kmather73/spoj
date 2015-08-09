#include <iostream>
using namespace std;

int main() {
	int numTestsLeft, currNum, sum;
	std::cin >>  numTestsLeft;
	
	while(numTestsLeft-- > 0){
		sum = 1;
		std::cin >> currNum;
		
		for(int i = 2; i*i <= currNum; ++i){
			if(currNum % i == 0){
				sum += i;
				sum += currNum / i;
			}
			if(i * i == currNum)
				sum -= i;
		}
		if(currNum == 1) //special case
	      sum = 0;
		std::cout << sum << std::endl;
	}
	return 0;
}
