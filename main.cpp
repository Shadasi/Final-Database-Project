#include <iostream>
#include <string>
#include <my_global.h>
#include <mysql.h>

using namespace std;
int checkTables(MYSQL* conn, MYSQL&);
int listCars(MYSQL* conn, MYSQL&, string);
int listDealers(MYSQL* conn, MYSQL&, string);
int insertQuery(MYSQL* conn, MYSQL&,  string);
void addCar(string&);
void addMake(string&);
void addDealer(string&);
int findMake(MYSQL* conn, MYSQL&);
int findZip(MYSQL* conn, MYSQL&);

int main()
{

	MYSQL *conn, mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;

	string db_host;
	string db_user;
	string db_password;
	string db_name;
	string myQuery;

	char command = '0';

	cout << "Please enter database host address" << endl;
	cin >> db_host;
		// prompt user for host address and read input

	cout << endl << "Please enter your username" << endl;
	cin >> db_user;
		// prompt user for username and read input

	cout << endl << "Please enter your password." << endl
		<< "NOTE:  THIS WILL NOT BE HIDDEN FROM SIGHT AS YOU TYPE" << endl;
	cin >> db_password;
		// prompt user for password and read input

	cout << endl << "Please enter the name of the database you would like to use" << endl;
	cin >> db_name;
		// prompt user for database name and read input
	int i;

	for (i = 0; i < 50; i++) // "clear" console, by putting a ton of lines in
		cout << "." << endl; // and hoping no one scrolls up

	cout << "Initializing mysql connection... ";
	cout.flush();
		// communicate with user that connection is being attempted

	mysql_init(&mysql);
		// actually initiate the connection

	cout << "Complete!" << endl;
		// communiate with user about initiation status

	cout << "Connecting to database... ";
	cout.flush();
		// tell user that the program is attempting to connect to the database

	conn = mysql_real_connect(&mysql, db_host.c_str(), db_user.c_str(), db_password.c_str(), db_name.c_str(), 0, 0, 0);
		// initialize the connection with user's inputted information

	if (conn == NULL) // check if the conneciton worked
	{
		cout << mysql_error(&mysql) << endl;
		return 1;
			// if it didn't spit an error and get out of the program
	}
	else
		cout << "Connected to database!" << endl << endl;
			// if it worked tell the user everything is good

	if (checkTables(conn, mysql) == 1) // call function to check tables
	{
		cout << "Tables not created... Exiting program.";
		return 1;
		// if the function failed tell the user and terminate program
	}

	while (command != 'q')
		// start loop that runs program, loop terminates when q is entered as a command
	{
		cout << "\nPlease enter a command\na to add\nl to list\nf to find\nq to quit\n\n";
		cin >> command;
		// prompts user to enter a command

		if (command == 'a' || command == 'A')
			// if the command was a, determine what type of addition the user would like to make
		{
			cout << "\nWhat would you like to add?\nc to add car\nd to add dealer\nm to add make\n\n";
			cin >> command;
			// prompt user for sub command	

			if (command == 'c' || command == 'C') 
				// check if command was c or C
			{
				addCar(myQuery); // call function to determine what to add to cars table
				insertQuery(conn, mysql, myQuery);  // send this information to the server
			}
			else if (command == 'd' || command == 'D')
				// check if command was d or D
			{
				addDealer(myQuery); // call function to determine what to add to dealer table
				insertQuery(conn, mysql, myQuery);  // send this information to the server
			}
			else if (command == 'm' || command == 'M')
				// check if the command was m or M
			{
				addMake(myQuery);  // call function to determine what to add to make table
				insertQuery(conn, mysql, myQuery);  // send this information to the server
			}
			else if (command == 'q' || command == 'Q') // if the command is q or Q get out of the program
				return 0;
			else
				cout << "INVALID COMMAND\n";
			// if the command wasn't valid tell the user and ask again
		}
		else if (command == 'l' || command == 'L')
			// if the command was l, determine what type of list the user would like to see
		{
			cout << "\nWhat list would you like to display?\nc to list cars\nd to list dealers\n\n";
			cin >> command;
			// prompt user for sub command

			if (command == 'c' || command == 'C')  // if they chose cars
			{
				myQuery = "SELECT VIN, miles, cDealer, price FROM cars ORDER BY VIN;"; 
					// make a query that views all the cars
				cout << endl << endl;
				listCars(conn, mysql, myQuery); // send it to the server
			}
			else if (command == 'd' || command == 'D')  // if the user chooses dealers
			{
				myQuery = "SELECT* FROM dealer ORDER BY zip, dName";
					// make a query that views the list of dealers
				cout << endl <<  endl;
				listDealers(conn, mysql, myQuery);  // send it to the server
			}
			else if (command == 'q' || command == 'Q')  // if the user wants to quit
				return 0;  // quit
			else
				cout << "INVALID COMMAND\n";
			// if the command wasn't valid tell the user
		}
		else if (command == 'f' || command == 'F')
			// if the command was f, determine what type of car the user wants to find
		{
			cout << "\nWhat would you like to find?\nm to find cars by Make\nz to find cars by Zip Code\n\n";
			cin >> command;
			// prompt user for sub command

			if (command == 'm' || command == 'M')
			{
				findMake(conn, mysql);
				// if the command was m initiate function to query by make
			}
			else if (command == 'z' || command == 'Z')
			{
				findZip(conn, mysql);
				// if the command was z initiate function to query by zipcode
			}
			else if (command == 'q' || command == 'Q')
				return 0;
				// if the command was q quit
			else
				cout << "INVALID COMMAND\n";
			// test for all valid sub commands
		}
		else if (command == 'q' || command == 'Q')
			// double checking for quit command
		{
			return 0;
		}
		else
		{
			cout << "\nInvalid Command";
			// error for invalid command
		}
	}

	mysql_close(conn);
	// close connection to server

    return 0;
}

int checkTables(MYSQL* conn, MYSQL& mysql)
{
	MYSQL_RES *res;
	int status;

	cout << "Checking tables.... "; cout.flush();
		// tell the user we are checking all the tables

	string myQuery = "create table if not exists cars (shortVIN char(3), VIN char(17), miles integer, cDealer char(100), price integer, PRIMARY KEY (VIN))";
	// create query that makes the cars table if it doesn't already exist
	// this table contains a shortened VIN number, vin number, the miles on the car, the dealers name, and a price
	// the primary key is the VIN number

	status = mysql_query(conn, myQuery.c_str()); // send query to create table to the server

	if (status != 0) // check if the query went through correctly
	{
		cout << mysql_error(&mysql) << endl;
		return 1;
			// if it didn't return error 1
	}

	myQuery = "create table if not exists make (mVIN char(3), mName char(100), PRIMARY KEY (mVIN))";
		// create query that makes the make table if it doesn't exist
		// this table contins a three character string that determines the make, and the name of hte make
		// the primary key is the three character VIN string

	status = mysql_query(conn, myQuery.c_str()); // send query to create table on the server

	if (status != 0)  // check if the query went through correctly
	{
		cout << mysql_error(&mysql) << endl;
		return 1;
			// if it didn't return error 1
	}

	myQuery = "create table if not exists dealer (dName char(100), zip integer, pNumber char(10), PRIMARY KEY (dName, zip))";
		// create query that makes a dealer table if it doesn't exist
		// this table contains a dealer name, a zipcode, a phone number, the primary key is the dealer name and zip code
		// the primary key is both the dealer name and the zip code

	status = mysql_query(conn, myQuery.c_str()); // send query to create table on the server

	if (status != 0) // check if the query went through correctly
	{
		cout << mysql_error(&mysql) << endl;
		return 1;
			// if it didn't return error 1
	}

	cout << "Done" << endl;
		// if everything went great tell the user we are done

	return 0;
		// return to main
}

int listCars(MYSQL* conn, MYSQL& mysql, string myQuery)
{
	// NOTE: this function receives the connection and the query already made

	int status;
	MYSQL_RES *res;
	MYSQL_ROW row;

	status = mysql_query(conn, myQuery.c_str()); // send query to the server

	if (status != 0) // check if query goes through okay
	{
		cout << mysql_error(&mysql);
		return 1;
			// if it doesn't send an error
	}
	res = mysql_store_result(conn);
		// grab result from server

	for (row = mysql_fetch_row(res); row != NULL; row = mysql_fetch_row(res))
		// grab all the rows untill there are none left
	{
		cout << row[0] << "  " << row[1] << "  " <<
			row[2] << "  " << row[3] << endl;
			// print out each element in the desired format
	}
	return 0;
		// return to main
}

int listDealers(MYSQL* conn, MYSQL& mysql, string myQuery)
{
		// NOTE:  this function receives a premade query and the sql connection

	int status;
	MYSQL_RES *res;
	MYSQL_ROW row;

	status = mysql_query(conn, myQuery.c_str()); // send query to the server

	if (status != 0) // check if query went through
	{
		cout << mysql_error(&mysql);
		return 1;
			// if it didn't spit and error and get out
	}
	res = mysql_store_result(conn);
		// store the result ino a pointer

	for (row = mysql_fetch_row(res); row != NULL; row = mysql_fetch_row(res))
		// loop to grab all rows in the result, untill rows are empty
	{

		cout << row[0] << "  " << row[1] << "  " <<
			row[2] << endl;
			// print out in desired format
	}

	return 0;
		// return to main
}

void addCar(string& myQuery)
{
		// NOTE: Function returns query by referece

	int status;
	string shortVin, VIN, dealer, miles, price;

	myQuery = "insert into cars (shortVIN, VIN, miles, cDealer, price) ";
	myQuery += "select mVIN, '";
		// start to create the correct query

	cout << "\nPlease enter the VIN number\n";
	cin >> VIN;
		// prompts user for VIN number and stores it
		
	shortVin = VIN.substr(0, 3);
		// sneakily takes three character identifier from VIN to use in queries

	if (VIN.length() != 17)
		// check if the VIN number inputed
		// is of valid length
	{
		cout << "\nInvalid VIN\n";
		return;
			// if the vin wasn't valid length get out
	}

	cout << "\nPlease enter the miles.\n";
	cin >> miles;
		// prompt user for miles and read in input
		
	cout << "\nPlease enter the Dealer Name.\n";
	cin >> dealer;
		// prompt user for dealer name and read in input

	cout << "\nPlease enter the Price.\n";
	cin >> price;
		// prompt user for price and read in input

	myQuery += VIN;
	myQuery += "', ";
	myQuery += miles;
	myQuery += ", dName";
	myQuery += ", ";
	myQuery += price;
	myQuery += " from make, dealer where make.mVIN = '";
	myQuery += shortVin;
	myQuery += "' AND dealer.dName = '";
	myQuery += dealer;
	myQuery += "';";

		// above block of code smashes the inputted info together into the correct query format

	return;
		// return to main, remember we are passing the query by reference
}

void addMake(string& myQuery)
{
		// NOTE:  This function passes the query string back by reference

	string VIN, name;

	myQuery = "insert into make values('";
		// start creating the query

	cout << "\nPlease enter three character ID.\n";
	cin >> VIN;
		// prompt user for VIN, read in input

	cout << "\nPlease enter name of Make.\n";
	cin >> name;
		// prompt user for make, read in input

	myQuery += VIN;
	myQuery += "', '";
	myQuery += name;
	myQuery += "');";
		// add user input to query

	return;
		// return to main
}

void addDealer(string& myQuery)
{
		// NOTE:  This function passes the query string back by reference

	string dName, zip, pNumber;

	myQuery = "insert into dealer values('";
		// start query string

	cout << "\nPlease enter name of dealer.\n";
	cin >> dName;
		// prompt user for dealer name, read input

	cout << "\nPlease enter zip code.\n";
	cin >> zip;
		// prompt user for zip code, read in input

	cout << "\nPlease enter phone number.\n";
	cin >> pNumber;
		// prompt user for phone number, read in input

	myQuery += dName;
	myQuery += "', '";
	myQuery += zip;
	myQuery += "', '";
	myQuery += pNumber;
	myQuery += "');";
		//  add user input to query

	return;
		// return to main
}

int insertQuery(MYSQL* conn, MYSQL& mysql, string myQuery)
{
	int status;

	cout << endl << "Attempting to insert values... ";
		// tell user that we are attempting their query

	status = mysql_query(conn, myQuery.c_str());
		// send query to server

	if (status != 0) // check if query worked
	{
		cout << mysql_error(&mysql);
		return 1;
			// if it didn't return an error
	}
	else
	{
		cout << "Insertion successful!" << endl;
		return 0;
			// otherwise print success and go back to main
	}

}

int findMake(MYSQL* conn, MYSQL& mysql)
{
	string myQuery, input;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int status;

	cout << "\nPlease enter a make to search by.\n";
	cin >> input;
		// prompt user for search terms
	
	myQuery = "SELECT cars.price, cars.miles, dealer.dName, dealer.pNumber FROM cars, make, dealer WHERE(make.mName = '";
	myQuery += input;
	myQuery += "') AND (make.mVIN = cars.shortVIN)AND(cars.cDealer = dealer.dName)ORDER BY cars.price, cars.miles, dealer.dName;";
		// create query, add users search specifications

	status = mysql_query(conn, myQuery.c_str()); // send query to server

	if (status != 0) // check if query sent correctly
	{
		cout << mysql_error(&mysql);
		return 1;
			// if it failed send an error
	}

	res = mysql_store_result(conn); // store results of the query

	for (row = mysql_fetch_row(res); row != NULL; row = mysql_fetch_row(res))
		// loop to get all results in the query table
	{
		string temp = row[3];
			// create a temp variable to help break the phone number apart

		cout << "$" << row[0] << "  " << row[1] << "  " <<
			row[2] << "  (" << temp.substr(0,3) <<
			")" << temp.substr(3,3) << "-" << temp.substr(6,4) << endl;
			// print output in specific format
	}

	return 0;
}

int findZip(MYSQL* conn, MYSQL& mysql)
{
	string myQuery, input;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int status;

	cout << "\nPlease enter a zip code to search by.\n";
	cin >> input;
		// prompt user for zip code

	myQuery = "SELECT make.mName, cars.miles, cars.price, dealer.dName, dealer.pNumber ";
	myQuery += "FROM cars, dealer, make WHERE (dealer.zip = ";
	myQuery += input;
	myQuery += ") AND(dealer.dName = cars.cDealer) AND(cars.shortVIN = make.mVIN) ORDER BY make.mName, cars.price, dealer.dName;";
		// add user input to query

	status = mysql_query(conn, myQuery.c_str());
		// send query to database server

	if (status != 0) // if the query didn't work
	{
		cout << "Invalid entry." << endl; 
		return 1;
			// send an error and get out of the function
	}
	res = mysql_store_result(conn);
		// store the result of the query into a variable
	for (row = mysql_fetch_row(res); 
	row != NULL;
		row = mysql_fetch_row(res)) // while there are values in the row loop to print them out
	{
		string temp = row[4]; 
		// create a string to hold the phone number so it can be split easier

		cout  << row[0] << "  " << row[1] << "  $" <<
			row[2] << "  " << row[3] <<"  (" << temp.substr(0, 3) <<
			")" << temp.substr(3, 3) << "-" << temp.substr(6, 4) << endl;
		//	print out row by row in the correct format
	}

	return 0;
}