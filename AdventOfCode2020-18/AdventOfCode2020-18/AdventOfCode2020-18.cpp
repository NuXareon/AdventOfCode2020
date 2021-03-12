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
    virtual int Process() = 0;
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

    int Process()
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

    std::optional<int> m_resultValue;
};

class ValueNode : public BaseNode
{
public:
    ValueNode(int value)
        : m_value(value)
    {

    }

    int Process() final
    {
        return m_value;
    }

    int m_value;
};

class OperationGraph
{
public:
    BaseNode* m_start = nullptr;
};

std::pair<BaseNode*, std::string_view::const_iterator> ProcessInput(std::string_view inputLine)
{
    BaseNode* startingNode = nullptr;
    ValueNode* leftValueNode = nullptr;
    OperationNode* currentOperationNode = nullptr;

    for (auto it = inputLine.begin(); it != inputLine.end(); it++)
    {
        const auto c = *it;

        if (c == ' ')
        {
            continue;
        }
        if (c == '+' || c == '*')
        {
            // auto operationNode = std::make_unique<OperationNode>(c);
            auto operationNode = new OperationNode(c);

            // Insert left part of the operation if it exists
            if (leftValueNode)
            {
                operationNode->m_leftOperand = leftValueNode;
                leftValueNode = nullptr;
            }

            // This should be exclusive of leftValueNode?
            if (currentOperationNode)
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
        else if (c == '(')
        {
            // Process parentesis subtree and asign it as right hand operand
            // TODO: This breask if there are more than 1 parentheses next to each other
            assert(currentOperationNode);
            auto parenthesisOpStart = it;
            parenthesisOpStart++;
            std::string_view newView(parenthesisOpStart, inputLine.end());
            auto [rightNode, endIt] = ProcessInput(newView);
            currentOperationNode->m_rightOperand = rightNode;
            it += (endIt-newView.begin()+1);
            // Open parenthesis
        }
        else if (c == ')')
        {
            // #TODO Might want to check for parenthesis missmatch here
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