//99

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;
const int MAX_NUM_RECORDS = 30,
SETW_ID = 15, SETW_MODEL = 9, SETW_QUANTITY = 12, SETW_PRICE = 14, SETPRECISION = 2,
TABLE_LENGTH = SETW_ID + SETW_MODEL + SETW_PRICE + SETW_QUANTITY,
MIN_PRICE = 5000, MIN_QUANTITY = 0, MIN_MODEL_LEN = 3,
ID_CRIT_1_NUM_LETTERS = 2, ID_CRIT_2_NUM_ALPHANUMERIC = 4, ID_CRIT_3_NUM_DIGITS = 3,
CORRECT_ID_LEN = ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC + ID_CRIT_3_NUM_DIGITS,
LETTER_OFFSET_IN_ASCII = 32;
const double SEARCH_PRICE_RANGE = 0.15;

class CarRecord {
public:
	CarRecord() { SetRecord("", "", 0, 0); }

	void SetRecord(string ID, string modelName, int quantityOnHand, double cost);

	string ToString() const; //returns formatted output as a string

	double GetPrice() const { return price; }

	string GetModel() const { return model; }

	string GetID() const { return carID; }

private:
	string carID, model;
	int quantity;
	double price;
};

enum Menu {
	PRINT_VALID = 1, PRINT_INVALID = 2, RETRIEVE_RECORD = 3, QUIT = 4
};

enum SearchMenu {
	CAR_ID_OR_NAME = 1, PRICE = 2, GO_BACK = 3
};

//Gets the data from the input file and writes all invalid data to error file
void GetData(CarRecord validRec[], int& numValidRec, int& numInvalidRec, string errorFileName, string inputFileName);

//prints valid records to screen in table format
void PrintValidRecords(const CarRecord validRec[], int numValidRec, string border, string header);

//prints invalid records to screen in table format
void PrintInvalidRecords(string border, string header, string errorFileName, int numInvalid);

//Creates the submenu for searching for a record
void EnterSearchSubMenu(const CarRecord validRec[], int numValidRec, int& userChoiceMainMenu, string border, string header);

bool IsValidID(string carID, string& errorMessage); //checks if the Car ID is valid

bool IsValidModel(string model, string& errorMessage); //checks if the Model Name is valid

bool IsValidQuantity(int quantity, string& errorMessage); //checks if the quantity is valid

bool IsValidPrice(double price, string& errorMessage); //checks if the price is valid

bool IsValidRecord(string id, string model, int quantity, double price, string& errMsg); //checks if the record is valid

string ToUpper(string str); //returns string converted to upper case

void ClearInvalidInput(string errMsg); //clears cin, clears the keyboard buffer, prints an error message

//searches the records by Car ID or Model Name
string SearchByIdOrModel(const CarRecord validRec[], int numValidRec, string keyWord);

//searches the records by price
string SearchByPrice(const CarRecord validRec[], int numValidRec, double lowerBound, double upperBound);

//Outputs any records that matches the search
void PrintSearchResultsIdOrModel(const CarRecord validRec[], int numValidRec, string border, string header);

//Outputs any record that matches the search
void PrintSearchResultsPrice(const CarRecord validRec[], int numValidRec, string border, string header);

int main() {
	int userChoiceMainMenu, numValidRec = 0, numInvalidRec = 0;
	stringstream borderSS, headerSS;
	CarRecord validRecords[MAX_NUM_RECORDS];
	string inputFileName = "test.txt", errorFileName = "InvalidRecords.txt", border, header;
	GetData(validRecords, numValidRec, numInvalidRec, errorFileName, inputFileName);

	cout << fixed << setprecision(SETPRECISION);

	borderSS << setfill('-') << setw(TABLE_LENGTH) << "";
	headerSS << setw(SETW_ID) << left << "Car ID" << setw(SETW_MODEL) << "Model" << setw(SETW_QUANTITY) << right << "Quantity" << setw(SETW_PRICE + 1) << "Price\n" << endl;

	border = borderSS.str();
	header = headerSS.str();

	do {
		cout << "\n\nMENU: \n"
			"1. PRINT VALID RECORDS\n"
			"2. PRINT INVALID RECORDS\n"
			"3. SEARCH FOR CAR RECORD\n"
			"4. QUIT\n\n";
		cin >> userChoiceMainMenu;
		switch (userChoiceMainMenu) {
		case PRINT_VALID:
			PrintValidRecords(validRecords, numValidRec, border, header);
			break;
		case PRINT_INVALID:
			PrintInvalidRecords(border, header, errorFileName, numInvalidRec);
			break;
		case RETRIEVE_RECORD:
			EnterSearchSubMenu(validRecords, numValidRec, userChoiceMainMenu, border, header);
			break;
		case QUIT:
			break;
		default:
			ClearInvalidInput("INVALID MENU SELECTION");
		}
	} while (userChoiceMainMenu != QUIT);
	cout << "\nQUITTING..." << endl;
	return 0;
}

void CarRecord::SetRecord(string ID, string modelName, int quantityOnHand, double cost) {
	carID = ID;
	model = modelName;
	quantity = quantityOnHand;
	price = cost;
}

string CarRecord::ToString() const {
	stringstream ss;
	ss << setw(SETW_ID) << left << carID
		<< setw(SETW_MODEL) << model
		<< setw(SETW_QUANTITY) << right << quantity
		<< setw(SETW_PRICE) << fixed << setprecision(SETPRECISION) << price;
	return ss.str();
}

void GetData(CarRecord validRec[], int& numValidRec, int& numInvalidRec, string errorFileName, string inputFileName) {
	string tempCarID, tempModel, errMsg;
	int tempQuantity;
	double tempPrice;
	ifstream inFile(inputFileName);
	ofstream errorFile(errorFileName);
	if (!inFile) {
		cout << "Input file not found. Exiting the program." << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	if (inFile.peek() == EOF) {
		cout << "The input file is empty. Quitting the program." << endl;
		inFile.close();
		system("pause");
		exit(EXIT_FAILURE);
	}
	while (!inFile.eof() && numValidRec < MAX_NUM_RECORDS) {
		inFile >> tempCarID >> tempModel >> tempQuantity >> tempPrice;
		tempCarID = ToUpper(tempCarID);
		tempModel = ToUpper(tempModel);
		errMsg = "";
		if (IsValidRecord(tempCarID, tempModel, tempQuantity, tempPrice, errMsg)) { //separates valid and invalid records
			validRec[numValidRec].SetRecord(tempCarID, tempModel, tempQuantity, tempPrice);
			numValidRec++;
		}
		else {
			errorFile << setw(SETW_ID) << left << tempCarID
				<< setw(SETW_MODEL) << tempModel
				<< setw(SETW_QUANTITY) << right << tempQuantity
				<< setw(SETW_PRICE) << fixed << setprecision(SETPRECISION) << tempPrice << " " << errMsg << "\n";
			numInvalidRec++;
		}
	}
	if (numValidRec == MAX_NUM_RECORDS && !inFile.eof()) {
		cout << "Not all Records have been stored; only the first " << MAX_NUM_RECORDS << " were processed." << "\n";
	}
	inFile.close();
	errorFile.close();
}

void PrintValidRecords(const CarRecord validRec[], int numValidRec, string border, string header) {
	if (numValidRec == 0) {
		cout << "\nNO VALID RECORDS FOUND.\n";
	}
	else {
		cout << "\nDISPLAYING " << numValidRec << " VALID RECORDS(UNSORTED)...\n"
			<< border << "\n"
			<< header;
		for (int i = 0; i < numValidRec; i++) {
			cout << validRec[i].ToString() << "\n";
		}
		cout << border << "\n";
	}
}

void PrintInvalidRecords(string border, string header, string errorFileName, int numInvalidRec) {
	ifstream invalidRecFile(errorFileName);
	if (invalidRecFile.peek() == EOF) {
		cout << "\nNO INVALID RECORDS FOUND\n";
	}
	else {
		cout << "\nDISPLAYING " << numInvalidRec << " INVALID RECORDS...\n"
			<< border << "\n"
			<< header;
		string line;
		while (getline(invalidRecFile, line)) {
			cout << line << "\n";
		}
		cout << border << "\n";
	}
	invalidRecFile.close();
}

bool IsValidID(string carID, string& errorMessage) {
	bool meetsIDCriteria = true;
	string idErrorMessage = "";
	if (carID.length() != CORRECT_ID_LEN)
	{
		meetsIDCriteria = false;
		idErrorMessage += " Must be " + to_string(CORRECT_ID_LEN) + " characters long.";
	}
	else {
		for (int i = 0; i < ID_CRIT_1_NUM_LETTERS; i++) {
			char ch = carID[i];
			if (!(((ch >= 'A') && (ch <= 'Z'))) || ch == 'O' || ch == 'o') { //checks if ID meets criteria 1
				meetsIDCriteria = false;
				idErrorMessage += " First " + to_string(ID_CRIT_1_NUM_LETTERS) + " characters must be Alpha(A-Z, letter O is not allowed).";
				i = ID_CRIT_1_NUM_LETTERS;
			}
		}
		for (int i = ID_CRIT_1_NUM_LETTERS; i < (ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC); i++) { //checks if ID meets criteria 2
			char ch = carID[i];
			if (!((((ch >= 'A') && (ch <= 'Z'))) || ((ch >= '0') && (ch <= '9'))) || ch == 'O') {
				meetsIDCriteria = false;
				idErrorMessage += " Characters " + to_string(ID_CRIT_1_NUM_LETTERS + 1) +
					"-" + to_string(ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC) +
					" must be alphanumeric(A-Z, letter O is not allowed).";
				i = ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC;
			}
		}
		for (int i = (ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC); i < CORRECT_ID_LEN; i++) { //checks if ID meets criteria 3
			char ch = carID[i];
			if (!(ch >= '0' && ch <= '9')) {
				meetsIDCriteria = false;
				idErrorMessage += " Characters " + to_string(ID_CRIT_1_NUM_LETTERS + ID_CRIT_2_NUM_ALPHANUMERIC + 1) +
					"-" + to_string(CORRECT_ID_LEN) + " must be numeric (0-9).";
				i = CORRECT_ID_LEN;
			}
		}
	}

	if (!meetsIDCriteria) {
		errorMessage += "INVALID ID [" + idErrorMessage + "] ";
	}
	return meetsIDCriteria;
}

bool IsValidModel(string model, string& errorMessage) {
	bool meetsModelCriteria = true;
	string modelErrorMessage = "";
	if (model.length() < MIN_MODEL_LEN) {
		meetsModelCriteria = false;
		modelErrorMessage += "Must be at least " + to_string(MIN_MODEL_LEN) + " characters long.";
	}
	if (model[0] < 'A' || model[0] > 'Z') { //checks if Model meets criteria 1
		meetsModelCriteria = false;
		modelErrorMessage += " Must start with a letter.";
	}
	int len = model.length();
	for (int i = 1; i < len; i++) { //checks if Model meets criteria 2
		char ch = model[i];
		if (!((((ch >= 'A') && (ch <= 'Z'))) || ((ch >= '0') && (ch <= '9')))) {
			meetsModelCriteria = false;
			modelErrorMessage += " Must consist of only alphanumeric characters.";
			i = len;
		}
	}
	if (!meetsModelCriteria) {
		errorMessage += "INVALID MODEL [" + modelErrorMessage + "] ";
	}
	return meetsModelCriteria;
}

bool IsValidQuantity(int quantity, string& errorMessage) {
	if (quantity < MIN_QUANTITY) {
		errorMessage += "INVALID QUANTITY [Can't be less than " + to_string(MIN_QUANTITY) + ".] ";
	}
	return quantity >= MIN_QUANTITY;
}

bool IsValidPrice(double price, string& errorMessage) {
	if (price <= MIN_PRICE) {
		errorMessage += "INVALID PRICE [Must be greater than $" + to_string(MIN_PRICE) + ".] ";
	}
	return price > MIN_PRICE;
}

bool IsValidRecord(string id, string model, int quantity, double price, string& errMsg) {
	bool isValidID, isValidModel, isValidQuantity, isValidPrice;

	isValidID = IsValidID(id, errMsg);
	isValidModel = IsValidModel(model, errMsg);
	isValidQuantity = IsValidQuantity(quantity, errMsg);
	isValidPrice = IsValidPrice(price, errMsg);

	return isValidID && isValidModel && isValidQuantity && isValidPrice;
}

string ToUpper(string str) {
	string upperCaseStr = "";
	for (int i = 0; i < str.length(); i++) {
		char ch = str[i];
		if (((ch >= 'a') && (ch <= 'z'))) {
			ch -= LETTER_OFFSET_IN_ASCII; //converts char to upper case
		}
		upperCaseStr += ch;
	}
	str = upperCaseStr;
	return upperCaseStr;
}

void ClearInvalidInput(string errMsg) {
	cout << "\n" << errMsg << "\n";
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string SearchByIdOrModel(const CarRecord validRec[], int numValidRec, string keyWord) {
	string recordsFound = "";
	for (int i = 0; i < numValidRec; i++) {
		string tempModel = validRec[i].GetModel(),
			tempID = validRec[i].GetID();
		if (tempModel.find(keyWord) != std::string::npos || tempID.find(keyWord) != std::string::npos) {
			recordsFound += validRec[i].ToString() + "\n";
		}
	}
	return recordsFound;
}

string SearchByPrice(const CarRecord validRec[], int numValidRec, double lowerBound, double upperBound) {
	string recordsFound = "";
	for (int i = 0; i < numValidRec; i++) {
		if (validRec[i].GetPrice() <= upperBound && validRec[i].GetPrice() >= lowerBound) {
			recordsFound += validRec[i].ToString() + "\n";
		}
	}

	return recordsFound;
}

void PrintSearchResultsPrice(const CarRecord validRec[], int numValidRec, string border, string header) {
	double searchPrice;
	string recordsFound = "";
	cout << "\nEnter the price you are looking for(Range +/- " << SEARCH_PRICE_RANGE * 100 << " %): \n";
	while (!(cin >> searchPrice)) {
		ClearInvalidInput("INVALID INPUT: You must enter a number.\nRE-ENTER:");
	}
	double lowerBound = searchPrice * (1 - SEARCH_PRICE_RANGE), upperBound = searchPrice * (1 + SEARCH_PRICE_RANGE);

	recordsFound += SearchByPrice(validRec, numValidRec, lowerBound, upperBound);

	if (recordsFound != "") {
		cout << "\nRecords within the price range of " << "$ " << lowerBound << " to $ " << upperBound << " found: \n"
			<< border << "\n"
			<< header
			<< recordsFound
			<< border;
	}
	else {
		cout << "\nNo Records within the price range of " << "$ " << lowerBound << " to $ " << upperBound << " found\n";
	}
}

void PrintSearchResultsIdOrModel(const CarRecord validRec[], int numValidRec, string border, string header) {
	string keyWord, recordsFound = "";
	cout << "\nEnter the keyword you want to search by: \n";
	cin >> keyWord;
	keyWord = ToUpper(keyWord);

	recordsFound += SearchByIdOrModel(validRec, numValidRec, keyWord);

	if (recordsFound != "") {
		cout << "\nRecords containing \"" << keyWord << "\" found: \n"
			<< border << "\n"
			<< header
			<< recordsFound
			<< border;
	}
	else {
		cout << "\nNo Records containing \"" << keyWord << "\" found\n";
	}
}

void EnterSearchSubMenu(const CarRecord validRec[], int numValidRec, int& userChoiceMainMenu, string border, string header) {
	int userChoiceSearchMenu;
	do {
		cout << "\n\nSEARCH BY: \n"
			"1. CAR ID OR MODEL NAME\n"
			"2. PRICE\n"
			"3. GO BACK\n"
			"4. QUIT\n\n";
		cin >> userChoiceSearchMenu;
		switch (userChoiceSearchMenu) {
		case CAR_ID_OR_NAME:
			PrintSearchResultsIdOrModel(validRec, numValidRec, border, header);
			break;
		case PRICE:
			PrintSearchResultsPrice(validRec, numValidRec, border, header);
			break;
		case GO_BACK:
			cout << "GOING BACK TO MAIN MENU\n";
			break;
		case QUIT:
			userChoiceMainMenu = QUIT;
			break;
		default:
			ClearInvalidInput("INVALID MENU SELECTION");
		}
	} while (!(userChoiceSearchMenu == GO_BACK || userChoiceMainMenu == QUIT));
}

//TEST 1 (Search found records)
/*
MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. QUIT

3


SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

1

Enter the keyword you want to search by:
fus

Records containing "FUS" found:
--------------------------------------------------
Car ID         Model        Quantity         Price

KFUSG6765      FUSION5            22      19975.12
--------------------------------------------------

SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

2

Enter the price you are looking for:
23000

Records within the price range of $ 19550.00 to $ 26450.00 found:
--------------------------------------------------
Car ID         Model        Quantity         Price

ALY13M398      BEATLE             90      19750.11
KDTYGA123      COROLLA6            0      22490.99
JGR5YG258      PRIUS              15      20090.54
IAT67T964      SENTRA            110      25840.99
KFUSG6765      FUSION5            22      19975.12
--------------------------------------------------

SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

3
GOING BACK TO MAIN MENU


MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. QUIT

4

QUITTING...

*/

//TEST #2 (Search did not find any records)
/*

MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. QUIT

3


SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

1

Enter the keyword you want to search by:
sdfasddfsa

No Records containing the key word "SDFASDDFSA" found


SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

2

Enter the price you are looking for:
1000

No Records within the price range of $ 850.00 to $ 1150.00 found


SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

4

QUITTING...
*/

//TEST #3 (go back and quit from the sub menu)
/*

MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. QUIT

3


SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

sdf

INVALID MENU SELECTION


SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

3
GOING BACK TO MAIN MENU


MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. QUIT

3


SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

4

QUITTING...
*/

//TEST #4 (invalid input for price)
/*
MENU:
1. PRINT VALID RECORDS
2. PRINT INVALID RECORDS
3. SEARCH FOR CAR RECORD
4. QUIT

3


SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

2

Enter the price you are looking for(Range +/- 15.00 %):
price

INVALID INPUT: You must enter a number.
RE-ENTER:
j2

INVALID INPUT: You must enter a number.
RE-ENTER:
5001

No Records within the price range of $ 4250.85 to $ 5751.15 found


SEARCH BY:
1. CAR ID OR MODEL NAME
2. PRICE
3. GO BACK
4. QUIT

4

QUITTING...*/