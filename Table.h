#include <vector>
#include <list>
#include <string>

#ifndef Table_H
#define Table_H

class Table
    {
      public:
        Table(std::string keyColumn, const std::vector<std::string>& columns);
        ~Table();
        bool good() const;
        bool insert(const std::string& recordString);
        void find(std::string key, std::vector<std::vector<std::string>>& records) const;
        int select(std::string query, std::vector<std::vector<std::string>>& records) const;

          // We prevent a Table object from being copied or assigned by
          // making the copy constructor and assignment operator unavailable.
        Table(const Table&) = delete;
        Table& operator=(const Table&) = delete;
      private:
        std::string keyValue;
        std::vector<std::string> columns;
        int keyIndex = -1;
        bool isValid;
        
        std::vector<std::list<std::vector<std::string>>> hash_table[1000];
    
        
        int hashF(std::string hashKey) const; 
    };






#endif /* Table_h */
