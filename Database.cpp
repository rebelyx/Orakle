#include "Database.h"
#include "http.h"
#include "tokenizer.h"
#include <fstream>
#include <set>
#include <algorithm>
using namespace std;

Database::Database() {
	
}

void Database::clear() {
	m_schema.clear();

	for (vector<vector<string>>::iterator it = m_rows.begin(); it != m_rows.end(); it++) {
		it->clear();
	}
	m_rows.clear();

	for (vector<MultiMap*>::iterator it = m_fieldIndex.begin(); it != m_fieldIndex.end(); it++) {
		delete (*it);
	}
	m_fieldIndex.clear();
	
}

Database::~Database() {
	clear();
}

bool Database::specifySchema(const vector<FieldDescriptor>& schema) {
	clear();
	vector<FieldDescriptor>::const_iterator it;
	for (it = schema.begin(); it != schema.end() && (it->index != Database::it_indexed); it++) {}

	if (it == schema.end())
		return false;

	for (unsigned int i = 0; i <schema.size(); i++) {
		m_schema.push_back(schema.at(i));
	}
	m_fieldIndex = vector<MultiMap*>(m_schema.size(), NULL);
	return true;
}

bool Database::addRow(const vector<string>& rowOfData) {
	if (m_schema.size() != rowOfData.size())
		return false;

	vector<string> temp;
	for (unsigned int i = 0; i <rowOfData.size(); i++) {
		temp.push_back(rowOfData.at(i));
		if (m_schema.at(i).index == Database::it_indexed) {
			if (m_fieldIndex.at(i) == NULL) {
				m_fieldIndex.at(i) = new MultiMap();
			}
			m_fieldIndex.at(i)->insert(rowOfData.at(i), m_rows.size());
		}
	}
	m_rows.push_back(temp);
	return true;
}

bool Database::loadFromURL(string url) {
	string page;
	if (!HTTP().get(url, page))
		return false;

	return loadFromPage(page);
}

bool Database::loadFromFile(string filename) {
	ifstream ifs(filename);
	string page((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	return loadFromPage(page);
}

bool Database::loadFromPage(string page) {
	string row_delimiters = "\n";
	Tokenizer t1(page, row_delimiters);
	string a_row;
	vector<FieldDescriptor> new_schema;
	if(t1.getNextToken(a_row)) { // schema line
		Tokenizer t2(a_row, ",");
		string a_field;
		while (t2.getNextToken(a_field)) {
			FieldDescriptor fd;
			if (a_field.find_last_of("*") == a_field.size() - 1) { // this field should be indexed
				fd.name = a_field.substr(0, a_field.size() - 1);
				fd.index = Database::it_indexed;
			} else {
				fd.name = a_field;
				fd.index = Database::it_none;
			}
			new_schema.push_back(fd);
		}
		specifySchema(new_schema);
	} else {
		return false;
	}

	while (t1.getNextToken(a_row)) {
		Tokenizer t2(a_row, ",");
		vector<string> rowOfData;
		string a_field_data;
		while (t2.getNextToken(a_field_data)) {
			rowOfData.push_back(a_field_data);
		}
		if (rowOfData.size() == m_schema.size())
			addRow(rowOfData);
		else
			return false;
	}
	return true;
}

int Database::getNumRows() const {
	return m_rows.size();
}

bool Database::getRow(int rowNum, vector<string>& row) const {
	if (rowNum < 0 || rowNum >= (int)m_rows.size())
		return false;

	row = m_rows.at(rowNum);
	return true;
}

int Database::search(const vector<SearchCriterion>& searchCriteria,
	const vector<SortCriterion>& sortCriteria,
	vector<int>& results) {
		results.clear();
		if (searchCriteria.empty())
			return ERROR_RESULT;

		for (unsigned int i = 0; i < searchCriteria.size(); i++) {
			if (schemaContainsField(searchCriteria.at(i).fieldName) == -1 || 
				(searchCriteria.at(i).minValue == "" && searchCriteria.at(i).maxValue == ""))
				return false;
		}
		
		set<unsigned int> rowsSatisfySearchCriteria = initializeSet();
		for (unsigned int i = 0; i < searchCriteria.size(); i++) {
			set<unsigned int> rowsGreaterThanMin = initializeSet();
			set<unsigned int> rowsSmallerThanMax = initializeSet();

			int fieldNum = schemaContainsField(searchCriteria.at(i).fieldName);
			if (searchCriteria.at(i).minValue != "") {
				rowsGreaterThanMin.clear();
				MultiMap::Iterator it = m_fieldIndex.at(fieldNum)->findEqualOrSuccessor(searchCriteria.at(i).minValue);
				for (; it.valid(); it.next()) {
					rowsGreaterThanMin.insert(it.getValue());
				}
				set<unsigned int>::iterator it1 = rowsSatisfySearchCriteria.begin();
				set<unsigned int>::iterator it2 = rowsGreaterThanMin.begin();
				while ( (it1 != rowsSatisfySearchCriteria.end()) && (it2 != rowsGreaterThanMin.end()) ) {
					if (*it1 < *it2) {
    					rowsSatisfySearchCriteria.erase(it1++);
					} else if (*it2 < *it1) {
						++it2;
					} else { // *it1 == *it2
						++it1;
						++it2;
					}
				}
				rowsSatisfySearchCriteria.erase(it1, rowsSatisfySearchCriteria.end());
			}
			if (searchCriteria.at(i).maxValue != "") {
				rowsSmallerThanMax.clear();
				MultiMap::Iterator it = m_fieldIndex.at(fieldNum)->findEqualOrPredecessor(searchCriteria.at(i).maxValue);
				for (; it.valid(); it.prev()) {
					rowsSmallerThanMax.insert(it.getValue());
				}
				set<unsigned int>::iterator it1 = rowsSatisfySearchCriteria.begin();
				set<unsigned int>::iterator it2 = rowsSmallerThanMax.begin();
				while ( (it1 != rowsSatisfySearchCriteria.end()) && (it2 != rowsSmallerThanMax.end()) ) {
					if (*it1 < *it2) {
    					rowsSatisfySearchCriteria.erase(it1++);
					} else if (*it2 < *it1) {
						++it2;
					} else { // *it1 == *it2
						++it1;
						++it2;
					}
				}
				rowsSatisfySearchCriteria.erase(it1, rowsSatisfySearchCriteria.end());
			}
		}
		for (set<unsigned int>::iterator it = rowsSatisfySearchCriteria.begin(); it != rowsSatisfySearchCriteria.end(); it++) {
			results.push_back(int(*it));
		}

		m_sortCriteria = sortCriteria;
		sort(results.begin(), results.end(), [&](int x, int y) {return compareTwoRows(x,y);});
		return results.size();
}

int Database::schemaContainsField(string fieldName) {
	for (unsigned int i = 0; i < m_schema.size(); i++) {
		if (m_schema.at(i).name == fieldName)
			return i;
	}
	return -1;
}

set<unsigned int> Database::initializeSet() {
	set<unsigned int> all;
	for (unsigned int i = 0; i < m_rows.size(); i++) {
		all.insert(i);
	}
	return all;
}

bool Database::compareTwoRows(const int& a, const int& b) {
	if (m_sortCriteria.empty())
		return false;

	for (unsigned int i = 0; i < m_sortCriteria.size(); i++) {
		int fieldNum = schemaContainsField(m_sortCriteria.at(i).fieldName);
		if (m_sortCriteria.at(i).ordering == Database::ot_ascending) {			
			if (m_rows.at(a).at(fieldNum) < m_rows.at(b).at(fieldNum)) {
				return true;
			}
			if (m_rows.at(a).at(fieldNum) > m_rows.at(b).at(fieldNum)) {
				return false;
			}
		}
		if (m_sortCriteria.at(i).ordering == Database::ot_descending) {
			if (m_rows.at(a).at(fieldNum) > m_rows.at(b).at(fieldNum)) {
				return true;
			}
			if (m_rows.at(a).at(fieldNum) < m_rows.at(b).at(fieldNum)) {
				return false;
			}
		}
	}
	return true;
}