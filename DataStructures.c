struct Customer{					//customer_db
	char username[1024];
	char password[1024];
	char account_num[1024];
	bool status;	// 1 active and 0 inactive
	bool type;		// 1 admin and 0 normal
};

struct Account{						// account_db
	char account_num[1024];
	double balance;
	bool status;	// 1 active and 0 inactive
};

struct Transaction{					//transaction_db
	char date[20]; // 8 Nov 2020; 23:10:34
	char account_num[1024];
	char type; // C credit and D debit
	double balance; // remaining balance
	double amount; // transaction amount
};
