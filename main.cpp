/*
#include "test.h"
#include <iostream>
#include <string>
using namespace std;
 
int main(int argc, char *argv[])
{
    string filename;

    switch (argc)
    {
      case 1:
        cout << "Enter test script file name: ";
        getline(cin, filename);
        break;
      case 2:
        filename = argv[1];
        break;
      default:
        cout << "Usage: " << argv[0] << " scriptname" << endl;
        return 1;
    }
    
    Test t;
    string problem;

    if (!t.loadAndRunTestScript(filename, problem))
    {
        cout << "Problem running script: " << problem << endl;
        return 1;
    }
}
*/

#include "Database.h"
#include "MultiMap.h"
#include <iostream>
#include <string>
#include <assert.h>
using namespace std;


bool setSchema(Database& db)
{
	Database::FieldDescriptor fd1, fd2, fd3; 

	fd1.name = "username"; 
	fd1.index = Database::it_indexed; // username is an indexed field

	fd2.name = "phonenum"; 
	fd2.index = Database::it_indexed; // phone # is an indexed field 

	fd3.name = "age"; 
	fd3.index = Database::it_none; // age is NOT an indexed field 
	std::vector<Database::FieldDescriptor>  schema;
	schema.push_back(fd1); 
	schema.push_back(fd2); 
	schema.push_back(fd3); 

	return db.specifySchema(schema); 
}

void addARow(Database& db)  // assumes schema has already been specified
{
	std::vector<std::string> row; 

	row.push_back("ednatodd");         // field 0: username 
	row.push_back("424-676-0202"); // field 1: phone number 
	row.push_back("0035");             // field 2: age 

	db.addRow(row);                    // add the new row to the Database 
}

void addFromInternet(Database& db)
{
	bool ok = db.loadFromURL("http://cs.ucla.edu/classes/winter14/cs32/Projects/4/Data/census.csv"); 
	if (ok) 
		cout << "Schema and data loaded from the Internet successfully!" << endl;
	else  
		cout << "Error loading schema and data from the Internet" << endl;  
}

void addFromFile(Database &db)
{
	bool ok = db.loadFromFile("E:\\Software\\Dropbox\\10th winter\\CS32(Dong Ziyi)\\proj4\\data.txt"); 

	if (ok) 
		cout << "Data loaded from file data.dat successfully!\n";
	else  
		cout << "Error loading data from file data.dat\n"; 
}

void getAndPrintRowNum(Database& db, unsigned int rowNum)
{
	vector<string> targetRow; 
	bool ok = db.getRow(rowNum, targetRow); 
	if (ok) 
	{ 
		// print each field value followed by a space
		for (size_t i = 0; i < targetRow.size(); i++) 
			cout << targetRow[i] << " "; 
		cout << endl; 
	} 
	else
		cout << "Error getting row number: " << rowNum << endl;
}

void doAQuery(Database& db)
{
	std::vector<Database::SearchCriterion> searchCrit;
	Database::SearchCriterion s1; 
	s1.fieldName = "name"; 
	s1.minValue = "Albert"; 
	s1.maxValue = "Sherry"; 

	Database::SearchCriterion s2; 
	s2.fieldName = "lifeExp"; 
	s2.minValue = "";  // no minimum specified 
	s2.maxValue = "00058.450";  

	searchCrit.push_back(s1); 
	searchCrit.push_back(s2);  

	  // We¡¯ll leave our sort criteria empty for now, which means 
	  // the results may be returned to us in any order 
	std::vector<Database::SortCriterion> sortCrit; 
	Database::SortCriterion sc1;
	sc1.fieldName = "name";
	sc1.ordering = Database::ot_ascending;

	Database::SortCriterion sc2;
	sc2.fieldName = "lifeExp";
	sc2.ordering = Database::ot_ascending;

	sortCrit.push_back(sc1);
	sortCrit.push_back(sc2);

	std::vector<int> results; 
	int numFound = db.search(searchCrit, sortCrit, results); 
	if (numFound == Database::ERROR_RESULT) 
	   cout << "Error querying the database!" << endl; 
	else 
	{
		cout << numFound << " rows matched the criteria; here they are:" << endl;
		for (size_t i = 0; i < results.size(); i++) 
		{
			// print the row number out where we had a match
			cout << "Row #" << results[i] << ": "; 

			// get and print the field values out from that row 
			std::vector<std::string> rowData; 
			if (db.getRow(results[i], rowData)) 
			{
				for (size_t j = 0; j < rowData.size(); j++)
					cout << rowData[j] << " "; 
				cout << endl;
			} 
			else  
				cout << "Error retrieving row¡¯s data!" << endl;
		}
	}
}




 
int main(int argc, char *argv[])
{
/*	MultiMap myMultiMap;
	myMultiMap.insert("Andrea", 6); 
	myMultiMap.insert("Bill", 2);
	myMultiMap.insert("Carey", 5); 
	myMultiMap.insert("Bill", 8);
	myMultiMap.insert("Batia", 4); 
	myMultiMap.insert("Larry", 7); 
	myMultiMap.insert("Larry", 9);
	myMultiMap.insert("Bill", 3);
	
	MultiMap::Iterator it = myMultiMap.findEqualOrSuccessor("Bz"); //MultiMap::Iterator it = myMultiMap.findEqual("Batia");
	while (it.valid())
	{
		cout << it.getKey() << " " << it.getValue() << endl;
		it.next();   // advance to the next associaton
	}
*/

	Database db;
//	setSchema(db);
//	addARow(db);
	addFromInternet(db);
//	addFromFile(db);
//	getAndPrintRowNum(db, 3);
	
	for (int i = 0; i < 5; i++)
		getAndPrintRowNum(db, i);



	doAQuery(db);






	system("pause");
	return 0;
}


