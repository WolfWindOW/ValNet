#include "json.hpp"
#include <fstream>
#include <iostream>
#include <string>
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <algorithm>
using json = nlohmann::json;

void stripData(std::ifstream, std::string);

struct jsonParse {
	std::string playerCard = "\"playerCard\""; 
	std::string playerTitle = "\"playerTitle\"";
	std::string roundResult = "\"roundResult\"";
	std::string roundCeremony = "\"roundCeremony\"";
	std::string bombPlanter = "\"bombPlanter\"";
	std::string plantRoundTime = "\"plantRoundTime\"";
	std::string viewRadians = "\"viewRadians\"";
	std::string location = "\"location\"";
	std::string x = "\"x\"";
	std::string y = "\"y\"";
	std::string plantLocation = "\"plantLocation\"";
	std::string plantSite = "\"plantSite\"";
	std::string defuseRoundTime = "\"defuseRoundTime\"";
	std::string defuseLocation = "\"defuseLocation\"";
	std::string ability = "\"ability\"";
	std::string timeSince = "\"timeSince\"";
	std::string roundResultCode = "\"roundResultCode\"";
	std::string killer = "\"killer\"";
	std::string victim = "\"victim\"";
	std::string receiver = "\"receiver\"";
	std::string legshots = "\"legshots\"";
	std::string bodyshots = "\"bodyshots\"";
	std::string headshots = "\"headshots\"";
};

struct charactersToId {
	std::string zero = "5f8d3a7f-467b-97f3-062c-13acf203c006";
	std::string one = "f94c3b30-42be-e959-889c-5aa313dba261";
	std::string two = "117ed9e3-49f3-6512-3ccf-0cada7e3823b";
	std::string three = "320b2a48-4d9b-a075-30f1-1f93a9b638fa";
	std::string four = "707eab51-4836-f488-046a-cda6bf494859";
	std::string five = "eb93336a-449b-9c1b-0a54-a891f7921d69";
	std::string six = "9f0d8ba9-4140-b941-57d3-a7ad57c6b417";
	std::string seven = "569fdd95-4d10-43ab-ca70-79becc718b46";
	std::string eight = "8e253930-4c05-31dd-1b6c-968525494517";
	std::string nine = "add6443a-41bd-e414-f6ad-e58d267f4e95";
	std::string ten = "a3bfb853-43b2-7238-a4f1-ad90e9e46bcc";
	std::string eleven = "22697a3d-45bf-8dd7-4fec-84a9e28c69d7";
	std::string twelve = "601dbbe7-43ce-be57-2a40-4abd24953621";
	std::string minusOne = "36FB82AF-409D-C0ED-4B49-57B1EB08FBD5";

	int toPos(std::string charID) {
		if (charID == zero) {
			return 0;
		}
		else if (charID == one) {
			return 1;
		}
		else if (charID == two) {
			return 2;
		}
		else if (charID == three) {
			return 3;
		}
		else if (charID == four) {
			return 4;
		}
		else if (charID == five) {
			return 5;
		}
		else if (charID == six) {
			return 6;
		}
		else if (charID == seven) {
			return 7;
		}
		else if (charID == eight) {
			return 8;
		}
		else if (charID == nine) {
			return 9;
		}
		else if (charID == ten) {
			return 10;
		}
		else if (charID == eleven) {
			return 11;
		}
		else if (charID == twelve) {
			return 12;
		}
		else return 13;
	}
};


void stripData(std::ifstream dataFile, std::string tempFileName) {
	static int tempNumber = 0;
	std::string tempNumberToString = std::to_string(tempNumber);
	tempNumber++;
	std::ofstream tempFile(tempFileName+tempNumberToString);
	std::string s;
	jsonParse j;
	while (getline(dataFile, s)) {
		if (s.find(j.ability) == std::string::npos) {
			tempFile << s;
			tempFile << std::endl;
		}
	}
}

void oneHotCharacters(json data, std::ofstream& out) {
	/*
	* Parses through the json, looks at each player in a match and assigns a 1 to the team that player is on in the corresponding bit
	* Additionally stores the number of wins, paired with the one hot data
	*/
	charactersToId cToI;
	std::vector<std::pair<std::pair<int*, int>, std::pair<int*, int>>> wholeFileData;
	for (auto i = 0; i < data["matches"].size(); ++i) {
		int red[14] = { 0 };
		int blue[14] = { 0 };
		
		//10 total players
		for (auto j = 0; j < 10; ++j) {
			if (data["matches"][i]["players"][j]["teamId"] == "BLUE") {
				blue[cToI.toPos(data["matches"][i]["players"][j]["characterId"])] = 1;
			}
			else red[cToI.toPos(data["matches"][i]["players"][j]["characterId"])] = 1;
		}
		/*
		* Tried to use pairs to keep the data in memory, but it freed the addresses after sending up the call stack
		auto redPair = std::make_pair(red, data["matches"][i]["teams"][0]["roundsWon"]);
		auto bluePair = std::make_pair(blue, data["matches"][i]["teams"][1]["roundsWon"]);
		auto matchPair = std::make_pair(redPair, bluePair);
		wholeFileData.push_back(matchPair);
		*/
		//First 14 bits are red, next 14 are blue, the bit after that is the winner 29 input nodes total
		for (auto k = 0; k < 14; ++k) {
			out << red[k];
		}
		for (auto k = 0; k < 14; ++k) {
			out << blue[k];
		}
		if (data["matches"][i]["teams"][0]["won"] == true) {
			out << 0;
		}
		else out << 1;
		out << ",";
		/*
		out << "," << data["matches"][i]["teams"][0]["roundsWon"] << ",";
		
		out << "," << data["matches"][i]["teams"][1]["roundsWon"] << ",\n\r";
		*/
	}
	return ;
}


void main() {
	//Stores onehot data from each file about the characters
	std::vector<std::vector<std::pair<std::pair<int*, int>, std::pair<int*, int>>>> oneHotData;
	//Opens each of the data files (11 in total)
	for (int i = 1; i < 12; ++i) {
		std::string sin = std::to_string(i) + ".json";
		std::string sout = std::to_string(i) + ".csv";
		std::ifstream in(sin);
		if (!in) {
			std::cerr << "Could not open " + sin + " , exiting" << std::endl;
			exit(1);
		}
		json fileData;
		//Drops the raw json
		in >> fileData;
		in.close();
		std::ofstream out(sout);
	}
	
	int s = 0;
}