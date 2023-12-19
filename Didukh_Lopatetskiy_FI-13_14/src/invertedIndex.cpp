#include "invertedIndex.hpp"

void Collections::parse(const std::string &inputString) {
    parser.lexer(inputString);
    auto tokens = parser.getTokens(); 
    
    if(tokens.size()) {
        std::string collectionName = tokens.at(1);

        if(tokens.at(0) == "INSERT") 
            insertSet(collectionName, getSetFromTokens(tokens));

        if(tokens.at(0) == "CREATE")
            createCollection(collectionName);

        if(tokens.at(0) == "PRINT_INDEX") 
            printCollectionIndex(tokens.at(1));

        if(tokens.at(0) == "SEARCH") 
            searchInCollection(collectionName);
        
        if(tokens.at(0) == "SEARCH_INTERSECTS")
            intersectsSearch(collectionName, getSetFromTokens(tokens));
        
        if(tokens.at(0) == "SEARCH_CONTAINS")
            intersectsSearch(collectionName, getSetFromTokens(tokens));
        
        if(tokens.at(0) == "SEARCH_CONTAINED_BY")
            intersectsSearch(collectionName, getSetFromTokens(tokens));

        if(tokens.at(0) == "CONTAINS") 
            containsCollection(collectionName, getSetFromTokens(tokens));
    } else {
        std::cout << "Error: Invalid command syntax, try again.\n";
    }
}

void Collections::createCollection(const std::string &collectionName) {
    if (collections.find(collectionName) != collections.end()) {
        std::cout << "Error: Creating collection with existing name '" << collectionName << "'" << std::endl;
    } else {
        collections[collectionName] = Collection();
        std::cout << "Collection '" << collectionName << "' has been created." << std::endl;
    }
}

void Collections::insertSet(const std::string &collecntionName, const std::set<int> &set) {
    collections[collecntionName].insert(set);
    std::cout << "Set has been added to " << collecntionName << std::endl;
}

void Collections::printCollectionIndex(const std::string &collectionName) {
    std::cout << "Printing " << collectionName << "collection...\n";
    collections[collectionName].print_index();
}

std::vector<std::set<int>> Collections::searchInCollection(const std::string &collectionName) {
    std::vector<std::set<int>> resultSets;

    if (collections.find(collectionName) != collections.end()) {
        std::cout << "Collection '" << collectionName << "' exists in collections\n";
        resultSets = collections[collectionName].getSets();
    } else 
        std::cout << "Collection '" << collectionName << "' doesn't exist in collections\n";

    printsSets(resultSets);
    return resultSets;
}

void Collections::printsSets(const std::vector<std::set<int>> &sets) {
    for(const auto& set : sets) {
        for(auto it = set.begin(); it != set.end(); ++it) 
            std::cout << *it << " ";
        std::cout << std::endl;
    }
}

bool Collections::containsCollection(const std::string &collectionName, const std::set<int> &set) {
    if (collections.find(collectionName) == collections.end()) {
        std::cout << "Error: Collection '" << collectionName << "' doesn't exist." << std::endl;
        return false;
    }

    Collection& currentCollection = collections[collectionName];

    if (currentCollection.contains(set)) {
        std::cout << "Collection '" << collectionName << "' contains the set." << std::endl;
        return true;
    } else {
        std::cout << "Collection '" << collectionName << "' does not contain the set." << std::endl;
        return false;
    }
}

void Collection::insert(const std::set<int> &set) {
    sets.push_back(set);
    
    for(auto it = set.begin(); it != set.end(); ++it) 
        invertedIndex[*it].insert(sets.size());
}

bool Collection::contains(const std::set<int> &set) {
    std::set<int> intersection;
    bool first = true;

    for (int num : set) {
        auto it = invertedIndex.find(num);
        if (it != invertedIndex.end()) {
            if (first) {
                intersection = it->second;
                first = false;
            } else {
                std::set<int> temp_intersection;
                std::set_intersection(
                    intersection.begin(), intersection.end(),
                    it->second.begin(), it->second.end(),
                    std::inserter(temp_intersection, temp_intersection.begin())
                );
                intersection = temp_intersection;
            }
        } else {
            intersection.clear();
            break;
        }
    }

    for(auto it = intersection.begin(); it != intersection.end(); ++it) {
        if(sets.at(*it - 1).size() != set.size())
            return false;
    }

    return !intersection.empty();
}

void Collection::print_index() {
    for(const auto& pair : invertedIndex) {
        std::cout << pair.first << ": ";
        
        for(const auto& name : pair.second)
            std::cout << name << " ";
        
        std::cout << std::endl;
    }
}

std::vector<std::set<int>> Collections::intersectsSearch(const std::string &collectionName, const std::set<int> &set) {
    std::vector<std::set<int>> resultSets;

    if (collections.find(collectionName) == collections.end()) {
        std::cout << "Error: Collection '" << collectionName << "' doesn't exist." << std::endl;
        return resultSets;
    }

    Collection& currentCollection = collections[collectionName];

    for (const auto& existingSet : currentCollection.getSets()) {
        std::set<int> intersection;
        std::set_intersection(
            set.begin(), set.end(),
            existingSet.begin(), existingSet.end(),
            std::inserter(intersection, intersection.begin())
        );

        if (!intersection.empty()) 
            resultSets.push_back(existingSet);
    }

    printsSets(resultSets);
    return resultSets;
}

std::vector<std::set<int>> Collections::containsSearch(const std::string &collectionName, const std::set<int> &set) {
    std::vector<std::set<int>> resultSets;

    if (collections.find(collectionName) == collections.end()) {
        std::cout << "Error: Collection '" << collectionName << "' doesn't exist." << std::endl;
        return resultSets;
    }

    /*
        u should change Collection::contains method, so it will return std::vector<std::set<int>>
        
        than in Collections:containsCollection
            return intersection.empty();
        
        and in continsSearch just print those sets :))

    */

    printsSets(resultSets);
    return resultSets;
}

std::vector<std::set<int>> Collections::containedBySearch(const std::string &collectionName, const std::set<int> &set) {
    std::vector<std::set<int>> resultSets;

    if (collections.find(collectionName) == collections.end()) {
        std::cout << "Error: Collection '" << collectionName << "' doesn't exist." << std::endl;
        return resultSets;
    }

    Collection& currentCollection = collections[collectionName];

    // i guess this could be done using sinilar idea to collection::contains
    // but this implementation works fine 
    // mb will change if there will be some time 

    for (const auto& existingSet : currentCollection.getSets()) {
        if (std::includes(set.begin(), set.end(), existingSet.begin(), existingSet.end())) {
            resultSets.push_back(existingSet);
        }
    }

    printsSets(resultSets);
    return resultSets;
}