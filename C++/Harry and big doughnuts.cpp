using namespace std;

int main() {
	std::string yes ="yes";
	std::string no = "no";
	
	int numCats, dNutWeight, sackWeight, numRounds;
	
	std::cin >> numRounds;
	while(numRounds-- > 0){
		std::cin >> numCats >> sackWeight >> dNutWeight;
		if(numCats * dNutWeight <= sackWeight)
			std::cout << yes << std::endl;
		else
			std::cout << no << std::endl;
	}
	return 0;
}
