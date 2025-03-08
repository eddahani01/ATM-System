
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

const string ClientstFileName = "ClientsData.txt";

struct stClient
{
	string AccountNumber;
	string PinCode;
	string ClientName;
	string Phone;
	double AccountBalance = 0;
	bool MarkForDelete = false;
};

stClient CurrentClient;

enum enATMMainMenuOptions {
	eQuickWithdraw = 1, eNormalWithdraw = 2, eDeposit = 3,
	eCheckBalance = 4, eExit = 5
};

void ShowMainMenu();
void Login();
void GoBackToMainMenu();
void ShowQuickWithdrawScreen();
void ShowNormalWithdrawScreen();

vector <string> SplitString(string S1, string delimiter)
{
	vector <string> vString;

	short pos = 0;
	string sWord; // define a string variable

	// use find() function to get the position of the delimiter

	while ((pos = S1.find(delimiter)) != std::string::npos)
	{
		sWord = S1.substr(0, pos); //store the word
		if (sWord != "")
			vString.push_back(sWord);


		S1.erase(0, pos + delimiter.length()); // erase() until position and move to the next word
		// we use delimiter.length() because delimiter can also be many character or symbols like (#,,#) (,,) ...

	}
	if (S1 != "")
		vString.push_back(S1); // it add last word ofthe string

	return vString;
}

stClient ConvertLineToRecord(string Line, string Seperator = "#//#")
{
	stClient Client;
	vector <string> vClientData;

	vClientData = SplitString(Line, Seperator);
	Client.AccountNumber = vClientData[0];
	Client.PinCode = vClientData[1];
	Client.ClientName = vClientData[2];
	Client.Phone = vClientData[3];
	Client.AccountBalance = stod(vClientData[4]);

	return Client;
}

vector <stClient> LoadClientDataFromFile(string FileName)
{
	vector <stClient> vClients;

	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string line;
		stClient Client;

		while (getline(MyFile, line))
		{
			Client = ConvertLineToRecord(line);
			vClients.push_back(Client);
		}
		MyFile.close();
	}
	return vClients;
}

bool FindClientByAccountNumberAndPinCode(string AccountNumber, string PinCode, stClient& Client)
{
	vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);

	for (stClient C : vClients)
	{
		if (C.AccountNumber == AccountNumber && C.PinCode == PinCode)
		{
			Client = C;
			return true;
		}
	}
	return false;
}

string ConvertRecordToLine(stClient Client, string Seperator = "#//#")
{
	string stClientRecord = "";

	stClientRecord += Client.AccountNumber + Seperator;
	stClientRecord += Client.PinCode + Seperator;
	stClientRecord += Client.ClientName + Seperator;
	stClientRecord += Client.Phone + Seperator;
	stClientRecord += to_string(Client.AccountBalance);

	return stClientRecord;

}

vector <stClient> SaveClientsDataToFile(string FileName, vector<stClient> vClients) {

	fstream MyFile;
	MyFile.open(FileName, ios::out); //overwrite

	string DataLine;
	if (MyFile.is_open()) {

		for (stClient C : vClients)
		{
			if (C.MarkForDelete == false)
			{
				//we only write record that are not marked for delete.

				DataLine = ConvertRecordToLine(C);
				MyFile << DataLine << endl;
			}
		}
		MyFile.close();
	}
	return vClients;
}

bool DepositBalanceToClientByAccountNumber(string AccountNumber, double Amount, vector <stClient>& vClients)
{
	char Answer = 'n';

	cout << "\n\n Are you sure you want to perform this Transaction? y/n ? ";
	cin >> Answer;

	if (Answer == 'y' || Answer == 'Y')
	{
		for (stClient& C : vClients)
		{
			if (C.AccountNumber == AccountNumber)
			{
				C.AccountBalance += Amount;
				SaveClientsDataToFile(ClientstFileName, vClients);
				cout << "\n\nDone Successfully. New Balance is: " << C.AccountBalance;
				return true;
			}
			break;
		}

	}
	else
	{
		cout << "\n Transaction Canceled." << endl;
	}


	return false;
}

short getQuickWithdrawAmount(short QuickWithdrawMenuOption)
{
	switch (QuickWithdrawMenuOption)
	{
	case 1:
		return 20;
	case 2:
		return 50;
	case 3:
		return 100;
	case 4:
		return 200;
	case 5:
		return 400;
	case 6:
		return 600;
	case 7:
		return 800;
	case 8:
		return 1000;
	default:
		return 0;
	}
}

void PerformQuickWithdrawMenuOptions(short QuickWithdrawMenuOption)
{
	if (QuickWithdrawMenuOption == 9)//Exit
		return;
	short WithdrawBalance = getQuickWithdrawAmount(QuickWithdrawMenuOption);

	if (WithdrawBalance > CurrentClient.AccountBalance)
	{
		cout << "\nThe Amount Exceeds your balance, Make another choice. " << endl;
		cout << "\n\nPress any key to Continue ...";
		system("pause>0");
		ShowQuickWithdrawScreen();
		return;
	}
	vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);
	DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber, WithdrawBalance * -1, vClients);
	CurrentClient.AccountBalance -= WithdrawBalance;
}

short ReadQuickWithdrawMenuOption()
{
	short Choice = 0;

	while (Choice < 1 || Choice>9)
	{
		cout << "Choose what to withdraw from [1] to [8]? ";
		cin >> Choice;
	}


	return Choice;
}

int ReadWithdrawAmount()
{
	int Amount = 0;
	do
	{
		cout << "Enter an amount multiple of 5's ? ";
		cin >> Amount;
	} while (Amount % 5 != 0);

	return Amount;
}

double ReadDepositAmount()
{
	double Amount = 0;

	do
	{
		cout << "\nEnter a positive Deposit Amount? ";
		cin >> Amount;

	} while (Amount <= 0);

	return Amount;
}

void PerformDepositOption()
{
	double DepositAmount = ReadDepositAmount();

	vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);
	DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber, DepositAmount, vClients);
	CurrentClient.AccountBalance += DepositAmount;

}

void ShowDepositScreen()
{
	system("cls");
	cout << string(40, '=') << endl;
	cout << "\tDeposit Screen \n";
	cout << string(40, '=') << endl << endl;
	PerformDepositOption();
}

void PerformNormalWithdrawOption()
{
	int WithdrawBalance = ReadWithdrawAmount();

	if (WithdrawBalance > CurrentClient.AccountBalance)
	{
		cout << "\nThe Amount Exceeds your balance, Make another choice. " << endl;
		cout << "\n\nPress any key to Continue ...";
		system("pause>0");
		ShowNormalWithdrawScreen();
		return;
	}
	vector <stClient> vClients = LoadClientDataFromFile(ClientstFileName);
	DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber, WithdrawBalance * -1, vClients);
	CurrentClient.AccountBalance -= WithdrawBalance;
}

void ShowNormalWithdrawScreen()
{
	system("cls");
	cout << "==============================================\n";
	cout << "\t\tNormal Withdraw\n";
	cout << "==============================================\n";
	PerformNormalWithdrawOption();
}

void ShowQuickWithdrawScreen()
{
	system("cls");
	cout << "==============================================\n";
	cout << "\t\tQuick Withdraw\n";
	cout << "==============================================\n";
	cout << "\t[1] 20\t\t[2] 50\n";
	cout << "\t[3] 100\t\t[4] 200\n";
	cout << "\t[5] 400\t\t[6] 600\n";
	cout << "\t[7] 800\t\t[8] 1000\n";
	cout << "\t[9] Exit\n";
	cout << "==============================================\n";

	cout << "\nYour Balance is " << CurrentClient.AccountBalance << endl << endl;


	PerformQuickWithdrawMenuOptions(ReadQuickWithdrawMenuOption());


}

void ShowCheckBalanceScreen()
{
	system("cls");

	cout << string(40, '=') << endl;
	cout << "\tCheck Balance Screen \n";
	cout << string(40, '=') << endl << endl;

	cout << "\nYour Balance is " << CurrentClient.AccountBalance << endl << endl;
}

short ReadATMMainMenuOptions()
{
	cout << "Choose what do you want to do? [1 to 5]? ";
	short Choice = 0;
	cin >> Choice;

	return Choice;
}

void GoBackToMainMenu()
{
	cout << "\n\nPress any key to go back to Main Menu ...";
	system("pause>0");
	ShowMainMenu();
}

void PerformATMMainMenuOptions(enATMMainMenuOptions ATMMainMenuOption)
{
	switch (ATMMainMenuOption)
	{
	case eQuickWithdraw:
		system("cls");
		ShowQuickWithdrawScreen();
		GoBackToMainMenu();
		break;
	case eNormalWithdraw:
		system("cls");
		ShowNormalWithdrawScreen();
		GoBackToMainMenu();
		break;
	case eDeposit:
		system("cls");
		ShowDepositScreen();
		GoBackToMainMenu();
		break;
	case eCheckBalance:
		system("cls");
		ShowCheckBalanceScreen();
		GoBackToMainMenu();
		break;
	case eExit:
		system("cls");
		Login();

		break;
	}
}

void ShowMainMenu()
{
	system("cls");
	cout << "==============================================\n";
	cout << "\t\tATM Main Menu Screen\n";
	cout << "==============================================\n";
	cout << "\t[1] Quick Withdraw.\n";
	cout << "\t[2] Normal Withdraw.\n";
	cout << "\t[3] Deposit.\n";
	cout << "\t[4] Check Balance.\n";
	cout << "\t[5] Logout.\n";
	cout << "==============================================\n";
	PerformATMMainMenuOptions((enATMMainMenuOptions)ReadATMMainMenuOptions());

}

bool LoadClientInfo(string AccountNumber, string PinCode)
{
	if (FindClientByAccountNumberAndPinCode(AccountNumber, PinCode, CurrentClient))
		return true;
	else
		return false;
}

void Login()
{
	bool LoginFaild = false;

	string AccountNumber, PinCode;

	do
	{
		system("cls");
		cout << "==============================================\n";
		cout << "\t\tLogin Screen\n";
		cout << "==============================================\n";

		if (LoginFaild)
		{
			cout << "Invalid Account Number/PinCode!" << endl;
		}
		cout << "\nEnter Account Number? ";
		cin >> AccountNumber;
		cout << "\nEnter PinCode? ";
		cin >> PinCode;

		LoginFaild = !LoadClientInfo(AccountNumber, PinCode);

	} while (LoginFaild);

	ShowMainMenu();

}

int main()
{
	Login();

	system("pause>0");

	return 0;
}