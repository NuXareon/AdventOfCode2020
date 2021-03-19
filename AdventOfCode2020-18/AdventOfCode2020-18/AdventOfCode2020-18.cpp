// AdventOfCode2020-18.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <memory>
#include <cassert>
#include <optional>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

enum class OperationType
{
    Sum,
    Mult
};

class BaseNode
{
public:
    virtual ~BaseNode() = default;
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

    // A bit old school. Could use some sort of smart pointer but raw pointers are fastest - specially since we can't use unique_ptr because of circular dependencies, which also make using shared_ptr a bit awkward.
    ~OperationNode()
    {
        // Break circular pointer references
        if (m_nextOp && m_nextOp->m_leftOperand == this)
        {
            m_nextOp->m_leftOperand = nullptr;
        }

        // Deallocate children
        delete m_leftOperand;
        m_leftOperand = nullptr;
        delete m_rightOperand;
        m_rightOperand = nullptr;
        delete m_nextOp;
        m_nextOp = nullptr;
    }

    long long Process() final
    {
        if (m_resultValue)
        {
            // Return result value if we have already calculated it. This happens when a "next operation" node tries to evaluate it's left side argument.
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
            assert(("Invalid operator", false));
        }

        if (m_nextOp)
        {
            return m_nextOp->Process();
        }

        return *m_resultValue;
    }

    OperationType m_opType = OperationType::Sum;

    BaseNode* m_leftOperand = nullptr;
    BaseNode* m_rightOperand = nullptr;
    OperationNode* m_nextOp = nullptr;

private:
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

private:
    long long m_value;
};

// This function ended up being a bit messy and could use some cleanup
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
    std::atomic<bool> finished = false;
    std::atomic<long long> result = 0;
    std::string inputLine;

    bool inputReady = false;
    std::mutex inputMutexThread;
    std::mutex inputMutexMain;
    std::condition_variable cv_thread;
    std::condition_variable cv_main;

    constexpr unsigned int numthreads = 8;
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < numthreads; ++i)
    {
        threads.emplace_back([&result, &inputLine, &inputReady, &inputMutexThread, &inputMutexMain, &cv_thread, &cv_main, &finished]()
            {
                for (;;)
                {
                    // Wait for main to provide an input
                    std::unique_lock inputLock(inputMutexThread);
                    cv_thread.wait(inputLock, [&inputReady, &finished]() { return inputReady || finished; });

                    if (finished)
                    {
                        return;
                    }

                    // Consume the input
                    const std::string inputLineCopy = inputLine;

                    {
                        std::lock_guard<std::mutex> lk(inputMutexMain);
                        inputReady = false;
                    }

                    inputLock.unlock();

                    // Notify main we can read the next line
                    cv_main.notify_one();

                    // Process this line
                    const auto [startingNode, endIt] = ProcessInput(inputLineCopy);
                    result.fetch_add(startingNode->Process(), std::memory_order_relaxed);

                    delete startingNode;
                }
            });
    }


    for (; std::getline(std::cin, inputLine);)
    {
        if (inputLine == "")
        {
            continue;
        }

        // Notify thread of new input
        {
            std::lock_guard<std::mutex> lk(inputMutexThread);
            inputReady = true;
        }

        cv_thread.notify_one();

        // Block main thread while worker is reading input
        std::unique_lock inputLock(inputMutexMain);
        cv_main.wait(inputLock, [&inputReady]() {return !inputReady; });
    }

    {
        std::lock_guard<std::mutex> lk(inputMutexThread);
        finished = true;
    }
    cv_thread.notify_all();

    for (auto& thread : threads)
    {
        thread.join();
    }

    // Right now we have some sort of race condition since we get different results
    std::cout << "Sum total = " << result << "\n";
}