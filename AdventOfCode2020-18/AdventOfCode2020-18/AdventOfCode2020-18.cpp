// AdventOfCode2020-18.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <memory>
#include <cassert>
#include <optional>

enum class OperationType
{
    Sum,
    Mult
};

class BaseNode
{
public:
    virtual long long Process() = 0;
};

class OperationNode : public BaseNode
{
public:
    OperationNode(char c)
    {
        if (c == '+')
        {
            m_opType = OperationType::Sum;
        }
        else if (c == '*')
        {
            m_opType = OperationType::Mult;
        }
        else
        {
            assert(("Invalid operator", false));
        }
    }

    long long Process() final
    {
        if (m_resultValue)
        {
            // Return result value if parents try to recalculate node
            return *m_resultValue;
        }

        assert(m_leftOperand && m_rightOperand);
        const auto leftResult = m_leftOperand->Process();
        const auto rightResult = m_rightOperand->Process();

        if (m_opType == OperationType::Sum)
        {
            m_resultValue = leftResult + rightResult;
        }
        else if (m_opType == OperationType::Mult)
        {
            m_resultValue = leftResult * rightResult;
        }
        else 
        {
            assert(false);
        }

        if (m_nextOp)
        {
            return m_nextOp->Process();
        }

        return *m_resultValue;
    }

    OperationType m_opType = OperationType::Sum;
    // #TODO make share ptrs
    BaseNode* m_leftOperand = nullptr;
    BaseNode* m_rightOperand = nullptr;
    BaseNode* m_nextOp = nullptr;

    std::optional<long long> m_resultValue;
};

class ValueNode : public BaseNode
{
public:
    ValueNode(long long value)
        : m_value(value)
    {

    }

    long long Process() final
    {
        return m_value;
    }

    long long m_value;
};

class OperationGraph
{
public:
    BaseNode* m_start = nullptr;
};

std::pair<BaseNode*, std::string_view::const_iterator> ProcessInput(std::string_view inputLine)
{
    BaseNode* startingNode = nullptr;
    BaseNode* leftValueNode = nullptr;
    OperationNode* currentOperationNode = nullptr;
    OperationNode* currentOperationNodeBackup = nullptr;

    for (auto it = inputLine.begin(); it != inputLine.end(); it++)
    {
        const auto c = *it;

        if (c == ' ')
        {
            continue;
        }
        if (c == '+')
        {
            auto operationNode = new OperationNode(c);

            // Insert left part of the operation if it exists, either a value or the previous operation
            if (leftValueNode)
            {
                assert(currentOperationNode == nullptr);
                operationNode->m_leftOperand = leftValueNode;
                leftValueNode = nullptr;
            }
            else if (currentOperationNode)
            {
                // sum has priority over multiplication, so swap the right nodes to process the sum first
                if (currentOperationNode->m_opType == OperationType::Mult)
                {
                    currentOperationNodeBackup = currentOperationNode;
                    operationNode->m_leftOperand = currentOperationNodeBackup->m_rightOperand;
                    /*
                    if (auto operationRight = dynamic_cast<OperationNode*>(currentOperationNodeBackup->m_rightOperand))
                    {
                        operationRight->m_nextOp = operationNode;
                    }
                    */
                    currentOperationNodeBackup->m_rightOperand = operationNode;
                }
                else
                {
                    currentOperationNode->m_nextOp = operationNode;
                    operationNode->m_leftOperand = currentOperationNode;
                }
            }

            currentOperationNode = operationNode;

            if (startingNode == nullptr)
            {
                startingNode = operationNode;
            }
        }
        else if (c == '*')
        {
            auto operationNode = new OperationNode(c);

            // Insert left part of the operation if it exists, either a value or the previous operation
            if (leftValueNode)
            {
                assert(currentOperationNode == nullptr);
                operationNode->m_leftOperand = leftValueNode;
                leftValueNode = nullptr;
            }
            else if (currentOperationNode)
            {
                if (currentOperationNode->m_opType == OperationType::Sum && currentOperationNodeBackup)
                {
                    currentOperationNode = currentOperationNodeBackup;
                    currentOperationNodeBackup = nullptr;
                }

                currentOperationNode->m_nextOp = operationNode;
                operationNode->m_leftOperand = currentOperationNode;
            }

            currentOperationNode = operationNode;

            if (startingNode == nullptr)
            {
                startingNode = operationNode;
            }
        }
        /*
        if (c == '+' || c == '*')
        {
            // auto operationNode = std::make_unique<OperationNode>(c);
            auto operationNode = new OperationNode(c);

            // Insert left part of the operation if it exists, either a value or the previous operation
            if (leftValueNode)
            {
                assert(currentOperationNode == nullptr);
                operationNode->m_leftOperand = leftValueNode;
                leftValueNode = nullptr;
            }
            else if (currentOperationNode)
            {
                currentOperationNode->m_nextOp = operationNode;
                operationNode->m_leftOperand = currentOperationNode;
            }

            currentOperationNode = operationNode;

            // Start with an operation
            if (startingNode == nullptr)
            {
                startingNode = operationNode;
            }
            // Insert Operation
        }
        */
        else if (c == '(')
        {
            // Process parenthesis subtree recursively
            auto parenthesisOpStart = it;
            parenthesisOpStart++;
            std::string_view newView(parenthesisOpStart, inputLine.end());
            auto [subOperationStart, endIt] = ProcessInput(newView);

            if (currentOperationNode)
            {
                currentOperationNode->m_rightOperand = subOperationStart;
            }
            else
            {
                leftValueNode = subOperationStart;
            }

            it += (endIt-newView.begin()+1);
        }
        else if (c == ')')
        {
            return { startingNode, it };
        }
        else if (std::isdigit(static_cast<unsigned char>(c)))
        {
            // need to insert to right of operation if it exists
            // check firstOperandValue == nullptr
            //auto valueNode = std::make_unique<ValueNode>(std::atoi(&c));

            auto valueNode = new ValueNode(std::atoi(&c));

            if (currentOperationNode && currentOperationNode->m_rightOperand == nullptr)
            {
                assert(currentOperationNode->m_leftOperand != nullptr);
                currentOperationNode->m_rightOperand = valueNode;
            }
            else
            {
                // Store the value until we find an operand for it
                assert(leftValueNode == nullptr);
                leftValueNode = valueNode;
            }
        }
        else
        {
            assert(("Invalid input", false));
        }
    }

    if (startingNode == nullptr)
    {
        // Case where we have a single operations surrounded by parentheses: (a + b)
        startingNode = leftValueNode;
    }

    return { startingNode, inputLine.end() };
}

int main()
{
    long long result = 0;
    std::string inputLine;
    for (; std::getline(std::cin, inputLine);)
    {
        const auto [startingNode, endIt] = ProcessInput(inputLine);
        result += startingNode->Process();
    }

    std::cout << "Sum total = " << result << "\n";
}