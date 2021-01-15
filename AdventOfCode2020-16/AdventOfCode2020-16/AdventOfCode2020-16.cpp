// AdventOfCode2020-16.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <atomic>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <cassert> 

// Data Structure Declaration
enum class InputStage
{
    RuleDefinition,
    YourTicket,
    NearbyTickets
};

struct csv_rulename : std::ctype<char> {
    csv_rulename(std::size_t refs = 0) : ctype(make_table(), false, refs) {}

    static const mask* make_table()
    {
        // make a copy of the "C" locale table
        static std::vector<mask> v(classic_table(), classic_table() + table_size);
        v[':'] |= space;      // : will be classified as whitespace
        v['-'] |= space;
        v[' '] &= ~space;      // space will not be classified as whitespace
        return &v[0];
    }
};

struct csv_range : std::ctype<char> {
    csv_range(std::size_t refs = 0) : ctype(make_table(), false, refs) {}

    static const mask* make_table()
    {
        // make a copy of the "C" locale table
        static std::vector<mask> v(classic_table(), classic_table() + table_size);
        v[':'] |= space;      // : will be classified as whitespace
        v['-'] |= space;
        return &v[0];
    }
};

struct csv_ticket : std::ctype<char> {
    csv_ticket(std::size_t refs = 0) : ctype(make_table(), false, refs) {}

    static const mask* make_table()
    {
        // make a copy of the "C" locale table
        static std::vector<mask> v(classic_table(), classic_table() + table_size);
        v[','] |= space;      // : will be classified as whitespace
        return &v[0];
    }
};

struct Rule
{
    Rule(std::string&& name) : ruleName(std::move(name)) {}

    std::string ruleName;
    std::vector<std::pair<unsigned int, unsigned int>> ranges;
};

struct Ticket
{
    Ticket() = default;
    Ticket(std::vector<unsigned int>&& initDigits) : digits(std::move(initDigits)) {}
    std::vector<unsigned int> digits;
};

constexpr unsigned int numberOfThreads = 8;

// Logic
std::pair<unsigned int, unsigned int> ReadRange(std::istringstream& inputStream)
{
    unsigned int min, max;
    inputStream >> min >> max;
    return { min, max };
}

void ReadRule(std::istringstream& inputStream, std::vector<Rule>& rules)
{
    // Get whole word until the semicolon
    inputStream.imbue(std::locale(std::cin.getloc(), new csv_rulename));

    std::string ruleName;
    inputStream >> ruleName;

    rules.emplace_back(std::move(ruleName));
    auto& newRule = rules.back();

    inputStream.imbue(std::locale(std::cin.getloc(), new csv_range));

    // Keep reading ranges until we run out
    for (;;)
    {
        std::pair<unsigned int, unsigned int> range = ReadRange(inputStream);
        newRule.ranges.push_back(std::move(range));

        std::string orKeyword;
        if (!(inputStream >> orKeyword))
        {
            break;
        }
    }
}

std::vector<unsigned int> ReadTicketDigits(std::istringstream& inputStream)
{
    std::vector<unsigned int> digits;
    inputStream.imbue(std::locale(std::cin.getloc(), new csv_ticket));

    for (unsigned int inputDigit; inputStream >> inputDigit;)
    {
        digits.push_back(inputDigit);
    }

    return digits;
}

void ProcessInput(std::vector<Rule>& rules, Ticket& myTicket, std::vector<Ticket>& nearbyTickets)
{
    InputStage inputStage = InputStage::RuleDefinition;
    for (std::string inputLine; std::getline(std::cin, inputLine);)
    {
        if (inputLine == "")
        {
            continue;
        }

        std::istringstream lineStringstream(std::move(inputLine));

        if (inputStage == InputStage::RuleDefinition)
        {
            if (inputLine == "your ticket:")
            {
                inputStage = InputStage::YourTicket;
                continue;
            }

            ReadRule(lineStringstream, rules);
        }
        else if (inputStage == InputStage::YourTicket)
        {
            if (inputLine == "nearby tickets:")
            {
                inputStage = InputStage::NearbyTickets;
                continue;
            }

            myTicket = ReadTicketDigits(lineStringstream);
        }
        else // Nearby tickets
        {
            auto ticketData = ReadTicketDigits(lineStringstream);
            nearbyTickets.emplace_back(std::move(ticketData));
        }
    }
}


void ProcessValidation(std::vector<bool>& validTickets, const std::vector<Ticket>& nearbyTickets, const std::vector<Rule>& rules)
{
    std::atomic<unsigned int> nextTicket = 0;

    auto validateTicketsFunction = [&validTickets, &nextTicket, &nearbyTickets, &rules]()
    {
        for (;;)
        {
            const unsigned int ticketId = nextTicket.fetch_add(1);      // Memory order is sequential
            if (ticketId >= nearbyTickets.size())
            {
                return;
            }

            const Ticket& ticket = nearbyTickets[ticketId];
            for (const auto& digit : ticket.digits)
            {
                bool validDigit = false;
                for (const Rule& rule : rules)
                {
                    for (const auto& range : rule.ranges)
                    {
                        if (digit >= range.first && digit <= range.second)
                        {
                            validDigit = true;
                            break;
                        }
                    }

                    if (validDigit)
                    {
                        break;
                    }
                }

                if (!validDigit)
                {
                    validTickets[ticketId] = false;
                    break;
                }
            }
        }
    };

    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < numberOfThreads; ++i)
    {
        threads.emplace_back(validateTicketsFunction);
    }

    for (std::thread& thread : threads)
    {
        thread.join();
    }
}

void DeleteUniqueRuleFromOtherFields(std::vector<std::set<size_t>>& validRulesPerField, size_t index)
{
    std::vector<size_t> cleanUpIndices;
    const auto& foundValue = *(validRulesPerField[index].begin());

    for (size_t j = 0; j < validRulesPerField.size(); ++j)
    {
        if (index != j)
        {
            auto numElemsErased = validRulesPerField[j].erase(foundValue);

            // Only check if it's unique if we removed a value from this set
            if (numElemsErased > 0 && validRulesPerField[j].size() == 1)
            {
                cleanUpIndices.push_back(j);
            }
        }
    }

    // Recursively clean up other indexes if after this operation a rule is unique for a any other field
    for (auto nextIndex : cleanUpIndices)
    {
        DeleteUniqueRuleFromOtherFields(validRulesPerField, nextIndex);
    }
}

void ProcessRules(std::vector<std::set<size_t>>& validRulesPerField, const std::vector<bool>& validTickets, const std::vector<Ticket>& nearbyTickets, const std::vector<Rule>& rules)
{
    std::shared_mutex validRulesFieldMutex;
    std::atomic<unsigned int> nextTicket = 0;

    auto findTicketFieldFunction = [&validTickets, &nextTicket, &nearbyTickets, &rules, &validRulesPerField, &validRulesFieldMutex]()
    {
        for (;;)
        {
            const unsigned int ticketId = nextTicket.fetch_add(1);      // Memory order is sequential
            if (ticketId >= nearbyTickets.size())
            {
                return;
            }

            if (!validTickets[ticketId])
            {
                continue;
            }

            std::vector<std::vector<unsigned int>> valuesToDelete(validRulesPerField.size());
            bool allRulesResolved = true;
            const Ticket& ticket = nearbyTickets[ticketId];
            for (size_t i = 0; i < ticket.digits.size(); ++i)
            {
                const auto& validRules = validRulesPerField[i];
                if (validRules.size() == 1)
                {
                    continue;
                }
                allRulesResolved = false;

                // Read lock (multiple threads can read at the same time)
                std::shared_lock readLock(validRulesFieldMutex);

                const auto digitToCheck = ticket.digits[i];
                for (const auto& ruleId : validRules)
                {
                    bool validRule = false;
                    for (const auto& range : rules[ruleId].ranges)
                    {
                        if (digitToCheck >= range.first && digitToCheck <= range.second)
                        {
                            validRule = true;
                            break;
                        }
                    }

                    if (!validRule)
                    {
                        valuesToDelete[i].push_back(ruleId);
                    }
                }
            }

            if (allRulesResolved)
            {
                return;
            }

            for (size_t i = 0; i < valuesToDelete.size(); ++i)
            {
                if (!valuesToDelete[i].empty())
                {
                    // Write lock (only one thread can write)
                    std::unique_lock writeLock(validRulesFieldMutex);

                    for (auto value : valuesToDelete[i])
                    {
                        validRulesPerField[i].erase(value);
                    }

                    // If after deletion this value is unique for the field, remove it from all the others
                    if (validRulesPerField[i].size() == 1)
                    {
                        DeleteUniqueRuleFromOtherFields(validRulesPerField, i);
                    }
                }
            }
        }
    };

    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < numberOfThreads; ++i)
    {
        threads.emplace_back(findTicketFieldFunction);
    }

    for (std::thread& thread : threads)
    {
        thread.join();
    }
}

int main()
{
    // Read Input
    std::vector<Rule> rules;
    Ticket myTicket;
    std::vector<Ticket> nearbyTickets;
    ProcessInput(rules, myTicket, nearbyTickets);

    // Ticket Validation
    std::vector<bool> validTickets(nearbyTickets.size(), true);
    ProcessValidation(validTickets, nearbyTickets, rules);

    // Ticket Process
    std::set<size_t> rulesPool;
    for (size_t i = 0; i < rules.size(); ++i)
    {
        rulesPool.insert(i);
    }
    std::vector<std::set<size_t>> validRulesPerField(nearbyTickets.begin()->digits.size(), rulesPool);
    ProcessRules(validRulesPerField, validTickets, nearbyTickets, rules);
    
    // Result Process
    unsigned long long result = 0;
    for (size_t i = 0; i < validRulesPerField.size(); ++i)
    {
        assert(validRulesPerField[i].size() == 1);

        const auto ruleId = *(validRulesPerField[i].begin());
        const Rule& rule = rules[ruleId];

        std::istringstream ruleNameStream(rule.ruleName);

        std::string firstWord;
        ruleNameStream >> firstWord;

        if (firstWord == "departure")
        {
            const auto ticketValue = myTicket.digits[i];
            result = (result == 0) ? ticketValue : result * ticketValue;
        }
    }

    std::cout << result << "\n";
}