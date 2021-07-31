#include "Table.h"
#include <string>
#include <iostream>
#include <cstdlib>

//=====================start of helper functions=====================
class StringParser
{
  public:
    StringParser(std::string text = "")
    {
        setString(text);
    }

    void setString(std::string text)
    {
        m_text = text;
        m_start = 0;
    }

    bool getNextField(std::string& field);

  private:
    std::string m_text;
    size_t m_start;
};

bool StringParser::getNextField(std::string& fieldText)
{
    m_start = m_text.find_first_not_of(" \t\r\n", m_start);
    if (m_start == std::string::npos)
    {
        m_start = m_text.size();
        fieldText = "";
        return false;
    }
    if (m_text[m_start] != '\'')
    {
        size_t end = m_text.find_first_of(" \t\r\n", m_start+1);
        fieldText = m_text.substr(m_start, end-m_start);
        m_start = end;
        return true;
    }
    fieldText = "";
    for (;;)
    {
        m_start++;
        size_t end = m_text.find('\'', m_start);
        fieldText += m_text.substr(m_start, end-m_start);
        m_start = (end != std::string::npos ? end+1 : m_text.size());
        if (m_start == m_text.size()  ||  m_text[m_start] != '\'')
            break;
        fieldText += '\'';
    }
    return true;
}


bool stringToDouble(std::string s, double& d)
{
        char* end;
        d = std::strtof(s.c_str(), &end);
        return end == s.c_str() + s.size()  &&  !s.empty();
}



int Table::hashF(std::string hashKey)  const {
    unsigned int h = std::hash<std::string>()(hashKey);
    return h % 1000;
}


//=====================end of helper functions=====================

//constructor for table
Table::Table(std::string keyColumn, const std::vector<std::string>& columns)
{
    keyValue = keyColumn;
    this->columns = columns;
    
}


Table::~Table() {}      //destructor for table object

//function checking if table is good
bool Table::good() const   {
        //if key is not empty
        //if key in columns
        //if columns does not contain duplicates
        //if one column is empty string
    bool keyColumnInColumns = false;
    if(keyValue == "")   //if key is empty, invalid table
    {
        return false;
    }
    
    if(columns.size() == 0) //if columns vector is empty, invalid table
    {
        return false;
    }

    
    for(int i=0; i < columns.size(); i++)
    {

        if(columns[i] == "") {  //if one column is empty string, table invalid
            return false;
        }
        
        if(keyValue == columns[i])
            keyColumnInColumns = true;          //if key in column, valid table
        
        for(int j =1+i; j < columns.size(); j++) {     //if columns contain duplicates
            if(columns[i] == columns[j])
                return false;
        }
        
    }
    
    if(keyColumnInColumns == false) //if key is not within column, return false; table badly constructed
    {
        return false;
    }
    return true;        //otherwise return true
     
}

//function inserting record into table
bool Table::insert(const std::string &recordString)
{
    this->isValid = this->good();   //check if table if valid before proceeding
    if(!isValid)
        return false;
    
    
    StringParser parser(recordString);
    std::string str;
    std::vector<std::string> record;
    
    while(parser.getNextField(str) == true)
    {
        //TO DO: should you check if recordString contains inner 2 numbers
        record.push_back(str);
    }
    
    if(record.size() != columns.size()) {   //if recordstring invalid number of records
        return false;
    }
    
    for (int i = 0; i < columns.size();i++) //find key index within columns to find it within record's vector
    {                                                                           //index
        if (columns[i] == keyValue)
        {
            keyIndex = i;
        }
    }
    //to do, should you check if record[keyIndex] is appropriate type?
    
    std::list<std::vector<std::string>> tempList;
    tempList.push_back(record); //create temporary list of vector of strings with record info
    //to push into hash_index
    
    unsigned int hash_index = this->hashF(record[keyIndex]);
    hash_table[hash_index].push_back(tempList); //add new list with vector/info to hash table at that index
    return true;

}


void Table::find(std::string key, std::vector<std::vector<std::string>>& records) const
{
    //to do: what if random key hashes to value in vector??
        //should there be a checker to ensure everything at that list
        //has same as given key
    bool is_valid = this->good();   //checking if table is valid before proceeding
    if(!is_valid)
        return;
    
    //hash the given key to find index
    unsigned int hash_index = this->hashF(key);
    
    int currKeyIndex = -1;
    for(int i=0; i < columns.size(); i++) //get the key index within vector of columns
    {
        if(columns[i] == keyValue)
        {
            currKeyIndex = i;
        }
    }
    
    
    //create a temporary records vector in case records has things in it
    std::vector<std::vector<std::string>> tempRecords;
    records.clear(); //clear the records vector of vectors
    
    //iterate overlist in hash_table[hash_index] and add data from nodes to records
    for(auto it = hash_table[hash_index].begin(); it != hash_table[hash_index].end(); it++)
    {
        
        for(auto j = (*it).begin(); j != (*it).end(); j++)  //access the list's inner vector
        {
            std::string targetField = (*j).at(currKeyIndex);    //get the field corresponding to matching key
            if(targetField == key)                              //check in case of collisions with diff keys
                tempRecords.push_back((*j));
        }
    }
 
    records = tempRecords;  //set records equal to temp vector of vector of strings
        
}

int Table::select(std::string query, std::vector<std::vector<std::string>>& records) const
{
    bool is_valid = this->good();   //checking if table is valid before proceeding
    if(!is_valid)
        return -1;
    
    StringParser p(query);  //create stringparser object
    std::string s;
    std::vector<std::string> queryVector;   //vector to store query
    std::vector<std::vector<std::string>> tempRecordsV; //temp vector of vector of strings to store matching records
    records.clear();    //clear the records vector in case not empty TO DO SHOULD I DO THIS
    int tokenOneColIndex = -1;     //store the column index of column wanted
    std::string searchToken;
    std::string operatorToken;
    std::string valueToken;
    
    while(p.getNextField(s) == true)
    {
        queryVector.push_back(s);   //push back query
    }
    
    if(queryVector.size() != 3)
    {
        return -1;  //make sure token size is appropriate
    }
    
    bool isValidQuery = false;
    for(int i=0; i < columns.size(); i++)   //make sure first token is valid column name
    {
        if(columns[i] == queryVector[0])
        {
            isValidQuery = true;
            tokenOneColIndex = i;   //get the first's token (what looking for) index within vector of columns
        }
    }
    
    if(!isValidQuery)       //if first token not a valid column name, return unsuccessful
    {
        return -1;  //if first token not valid column name return -1
    }
    
   
    searchToken = queryVector[0];           //define variables for the three query search tokens
    operatorToken = queryVector[1];
    valueToken = queryVector[2];
    
    //check if second token valid
    //convert operator token into all uppercase for easier processing; check less cases and check standard ones instead
    if (!(operatorToken == ">" || operatorToken == ">=" || operatorToken == "<=" || operatorToken == "==" || operatorToken == "<" || operatorToken == "!=" || operatorToken == "="))
    {
    
        if(operatorToken.size() != 2) //if operator token is not a symbol but not size two; which is needed for non symbol
        {                               //operations, return not successful
            return -1;
        }
        
        double d;
        if (!stringToDouble(valueToken, d)) //if numerical comparison, query/third token must be valid number
        {
            return -1;
        }
        
        //convert to uppercase using stl toupper function to standardize
        char one = queryVector[1][0];
        char two = queryVector[1][1];
        one = std::toupper(one);
        two = std::toupper(two);
        operatorToken = one;
        operatorToken += two;
        
        std::cerr << operatorToken << std::endl;
 
    }
    
    //iterate through hashtable to find indicated thing
        
    //TO DO: CHECK IF FIRST TOKEN IS NUMERICAL
    int badRecords = 0;         //create variable to add to if records are not in proper form
    
        for(int i=0; i < 1000; i++)
        {
            if(!hash_table[i].empty())
            {
                for(auto it = hash_table[i].begin(); it != hash_table[i].end(); it++)
                {
                    for(auto j = (*it).begin(); j != (*it).end(); j++)  //access the list's inner vector
                    {
                        //get the record info searching for from index found in columns at the current vector index
                        std::string tokenOne = (*j).at(tokenOneColIndex);
                        
                        //if non-numerical comparisons:
                        if(operatorToken == "<" || operatorToken == "<=" || operatorToken == ">" || operatorToken == ">=" ||  operatorToken == "!=" || operatorToken == "==" || operatorToken == "=")
                        {
                            //if is certain operator and satfies condition, push back vector to records
                            if(operatorToken == "<" && (tokenOne < valueToken))
                                tempRecordsV.push_back((*j));
                            
                            else if(operatorToken == "<=" && (tokenOne <= valueToken))
                                tempRecordsV.push_back((*j));
                            
                            else if(operatorToken == ">" && (tokenOne > valueToken))
                                tempRecordsV.push_back((*j));
                            
                            else if(operatorToken == ">=" && (tokenOne >= valueToken))
                                tempRecordsV.push_back((*j));
                            
                            else if(operatorToken == "==" && (tokenOne == valueToken))
                                tempRecordsV.push_back((*j));
                            
                            else if(operatorToken == "=" && (tokenOne == valueToken))
                                tempRecordsV.push_back((*j));
                            
                            else {
                                if(operatorToken == "!=" && (tokenOne != valueToken))
                                    tempRecordsV.push_back((*j));
                            }
                        }
                        //else make a numerical comparison
                        else if (operatorToken == "NE" || operatorToken == "GE" || operatorToken == "GT" || operatorToken == "LE" || operatorToken == "LT" || operatorToken == "EQ")
                        {
                            double currTokenOne, valueTokenThree;
                            
                            //if can convert current string's info into number, enter comparisons
                            if(stringToDouble(tokenOne, currTokenOne) && stringToDouble(valueToken, valueTokenThree))
                            {
                                //if certain operator and satisfies the conditions, push back vector
                                if(operatorToken == "NE")
                                {
                                    if(currTokenOne != valueTokenThree)
                                        tempRecordsV.push_back((*j));
                                }
                                else if(operatorToken == "GE")
                                {
                                    if(currTokenOne >= valueTokenThree)
                                        tempRecordsV.push_back((*j));
                                }
                                else if(operatorToken == "GT")
                                {
                                    if(currTokenOne > valueTokenThree)
                                        tempRecordsV.push_back((*j));
                                }
                                else if(operatorToken == "LE")
                                {
                                    if(currTokenOne <= valueTokenThree)
                                        tempRecordsV.push_back((*j));
                                }
                                else if(operatorToken == "EQ")
                                {
                                    if(currTokenOne == valueTokenThree)
                                        tempRecordsV.push_back((*j));
                                }
                                else //operatorToken == "LT"
                                {
                                    if(currTokenOne < valueTokenThree)
                                        tempRecordsV.push_back((*j));
                                }
                            }
                            else {  //invalid record
                                ++badRecords;
                            }
                        }
                        else {      //else invalid operator token in query
                            return -1;  //beause not non-numerical comparison but not satisfying numerical comparison operators
                        }
                    }                                            //end of for loop iterating over vector indeces
                }                                           //end of for loop iterating over list
            }                                    //end of if statement checking if bucket not empty
        }                    //end of outer for loop checking every hash table index/bucket
        
    
    records = tempRecordsV;     //set records vector to well formed queries
    return badRecords;      //returns 0 if no bad records, indicating success; if bad records returns this number
}   //end of function scope
 


//test cases:
    //key is formatted weird "   scores" rather than "scores"
    //columns is formatted weird "josh    " rather than "josh"
    //check with commas and shit

 
