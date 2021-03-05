// AdventOfCode2020-18.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <memory>

enum class OperationType
{
    Sum,
    Mult
};

class BaseNode
{

};

class OperationNode : public BaseNode
{
public:
    OperationType m_opType;
    std::unique_ptr<BaseNode> m_leftOperand;
    std::unique_ptr<BaseNode> m_rightOperand;
    std::unique_ptr<BaseNode> m_nextOp;
};

class ValueNode : public BaseNode
{
public:
    int m_value;
};

class OperationGraph
{
public:
    std::unique_ptr<BaseNode> m_start;
};

int main()
{
    OperationGraph operations;
    
    std::string inputLine;
    for (; std::getline(std::cin, inputLine);)
    {
        for (const auto& c : inputLine)
        {
            std::unique_ptr<BaseNode> firstOperandValue;

            if (c == '+' || c == '*')
            {
                auto operationNode = std::make_unique<OperationNode>();
                if (c == '+')
                {
                    operationNode->m_opType = OperationType::Sum;
                }
                else
                {
                    operationNode->m_opType = OperationType::Mult;
                }

                // Insert left part of the operation if it exists
                if (firstOperandValue)
                {
                    operationNode->m_leftOperand = std::move(operationNode);
                }

                if (operations.m_start == nullptr)
                {
                    operations.m_start = std::move(operationNode);
                }

                // Insert Operation
            }
            else if (c == '(')
            {
                // Open parenthesis
            }
            else if (c == ')')
            {
                // Close parenthesis
            }
            else 
            {
                // need to insert to right of operation if it exists
                // check firstOperandValue == nullptr
                auto valueNode = std::make_unique<ValueNode>();
                valueNode->m_value = std::atoi(&c);
                firstOperandValue = std::move(valueNode);
                //It's a digit
            }
        }
    }

    std::cout << "Hello World!\n";
}